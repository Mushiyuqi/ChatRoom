#include "HttpConnection.h"
#include "LogicSystem.h"

HttpConnection::HttpConnection(tcp::socket socket): m_socket(std::move(socket)) {
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

    if (m_request.method() == http::verb::get) {
        // 处理get请求
        bool success = LogicSystem::GetInstance().HandleGet(m_request.target(), shared_from_this());
        if (!success) {
            // 设置状态码为404
            m_response.result(http::status::not_found);
            // 设置响应类型为text/plain
            m_response.set(http::field::content_type, "text/plain");
            // 设置响应体
            beast::ostream(m_response.body()) << "Not Found";
            // 回包
            WriteResponse();
            return;
        }

        m_response.result(http::status::ok);
        m_response.set(http::field::server, "GateServer");
        WriteResponse();
    }
}
