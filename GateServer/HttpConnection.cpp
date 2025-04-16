#include "HttpConnection.h"
#include "LogicSystem.h"

unsigned char ToHex(unsigned char x);
unsigned char FromHex(unsigned char x);
std::string UrlEncode(const std::string& str);
std::string UrlDecode(const std::string& str);

HttpConnection::HttpConnection(boost::asio::io_context& ioc): m_socket(ioc) {
    std::cout << "HttpConnection::HttpConnection constructed" << std::endl;
}

HttpConnection::~HttpConnection() {
    std::cout << "HttpConnection::~HttpConnection destructed" << std::endl;
}

tcp::socket& HttpConnection::GetSocket() {
    return m_socket;
}

void HttpConnection::Start() {
    auto self = shared_from_this();
    http::async_read(m_socket, m_buffer, m_request, [this, self](auto ec, auto len) {
        try {
            if (ec) {
                std::cerr << "HttpConnection::Start() Error " << ec.message() << std::endl;
                return;
            }
            // 忽略len参数
            boost::ignore_unused(len);
            // 处理请求
            HandleRequest();
            CheckDeadline();
        }
        catch (std::exception& e) {
            // 处理beast异常
            std::cerr << "HttpConnection::Start() Error " << e.what() << std::endl;
        }
    });
}

void HttpConnection::CheckDeadline() {
    auto self = shared_from_this();
    // 设置定时器
    m_deadline.async_wait([this, self](auto ec) {
        if (!ec) {
            m_socket.close(ec);
        }
    });
}

void HttpConnection::WriteResponse() {
    auto self = shared_from_this();
    // 设置响应体长度
    m_response.content_length(m_response.body().size());
    http::async_write(m_socket, m_response, [this, self](auto ec, auto len) {
        // 关闭服务器发送端
        m_socket.shutdown(tcp::socket::shutdown_send, ec);
        // 关闭定时器
        m_deadline.cancel();
    });
}

void HttpConnection::HandleRequest() {
    // 设置版本
    m_response.version(m_request.version());
    // 设置短连接
    m_response.keep_alive(false);
    // 处理Get请求
    if (m_request.method() == http::verb::get) {
        // 解析URL
        PreParseGetParam();
        // 处理请求
        if (!LogicSystem::GetInstance().HandleGet(m_get_url, shared_from_this())) {
            // 处理失败设置返回内容
            m_response.result(http::status::not_found);
            m_response.set(http::field::content_type, "text/plain");
            beast::ostream(m_response.body()) << "Not Found";
            WriteResponse();
            return;
        }
        // 返回数据
        m_response.result(http::status::ok);
        m_response.set(http::field::server, "GateServer");
        WriteResponse();
    }
    // 处理Post请求
    if (m_request.method() == http::verb::post) {
        // 处理请求
        if (!LogicSystem::GetInstance().HandlePost(m_request.target(), shared_from_this())) {
            // 处理失败设置返回内容
            m_response.result(http::status::not_found);
            m_response.set(http::field::content_type, "text/plain");
            beast::ostream(m_response.body()) << "Not Found";
            WriteResponse();
            return;
        }
        // 返回数据
        m_response.result(http::status::ok);
        m_response.set(http::field::server, "GateServer");
        WriteResponse();
    }
}

void HttpConnection::PreParseGetParam() {
    // 提取 URI
    auto uri = m_request.target();
    // 查找查询字符串的开始位置（即 '?' 的位置）
    auto query_pos = uri.find('?');
    if (query_pos == std::string::npos) {
        m_get_url = uri;
        return;
    }

    m_get_url = uri.substr(0, query_pos);
    std::string query_string = uri.substr(query_pos + 1);
    std::string key;
    std::string value;
    size_t pos = 0;
    while ((pos = query_string.find('&')) != std::string::npos) {
        auto pair = query_string.substr(0, pos);
        size_t eq_pos = pair.find('=');
        if (eq_pos != std::string::npos) {
            key = UrlDecode(pair.substr(0, eq_pos)); // 假设有 url_decode 函数来处理URL解码
            value = UrlDecode(pair.substr(eq_pos + 1));
            m_get_params[key] = value;
        }
        query_string.erase(0, pos + 1);
    }
    // 处理最后一个参数对（如果没有 & 分隔符）
    if (!query_string.empty()) {
        size_t eq_pos = query_string.find('=');
        if (eq_pos != std::string::npos) {
            key = UrlDecode(query_string.substr(0, eq_pos));
            value = UrlDecode(query_string.substr(eq_pos + 1));
            m_get_params[key] = value;
        }
    }
}

// 将单个十进制数字转换为对应的十六进制字符
unsigned char ToHex(unsigned char x) {
    return x > 9 ? x + 55 : x + 48;
}

// 将单个十六进制字符转换为对应的十进制数字
unsigned char FromHex(unsigned char x) {
    unsigned char y;
    if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
    else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
    else if (x >= '0' && x <= '9') y = x - '0';
    else
        assert(0);
    return y;
}

// 实现url编码
std::string UrlEncode(const std::string& str) {
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++) {
        //判断是否仅有数字和字母构成
        if (isalnum((unsigned char)str[i]) ||
            (str[i] == '-') ||
            (str[i] == '_') ||
            (str[i] == '.') ||
            (str[i] == '~'))
            strTemp += str[i];
        else if (str[i] == ' ') //为空字符
            strTemp += "+";
        else {
            //其他字符需要提前加%并且高四位和低四位分别转为16进制
            strTemp += '%';
            strTemp += ToHex((unsigned char)str[i] >> 4);
            strTemp += ToHex((unsigned char)str[i] & 0x0F);
        }
    }
    return strTemp;
}

// 实现url解码
std::string UrlDecode(const std::string& str) {
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++) {
        //还原+为空
        if (str[i] == '+') strTemp += ' ';
            //遇到%将后面的两个字符从16进制转为char再拼接
        else if (str[i] == '%') {
            assert(i + 2 < length);
            unsigned char high = FromHex((unsigned char)str[++i]);
            unsigned char low = FromHex((unsigned char)str[++i]);
            strTemp += high * 16 + low;
        }
        else strTemp += str[i];
    }
    return strTemp;
}
