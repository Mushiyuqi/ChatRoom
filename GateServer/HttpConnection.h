#pragma once
#include "const.h"


class HttpConnection:public std::enable_shared_from_this<HttpConnection>{
    friend class LogicSystem;
public:
    explicit HttpConnection(boost::asio::io_context&);
    ~HttpConnection();
    tcp::socket& GetSocket();
    void Start();

private:
    // 超时检测
    void CheckDeadline();
    // 读写请求和回复
    void WriteResponse();
    void HandleRequest();

    // 处理URL
    void PreParseGetParam();

    // HttpConnection 维护的连接
    tcp::socket m_socket;

    // Get 请求路由
    std::string m_get_url;
    // Get 请求参数
    std::unordered_map<std::string, std::string> m_get_params;

    // 读写缓冲区 8kb
    beast::flat_buffer m_buffer{8192};
    // 接收图片, 文字等动态的请求体和回复体
    http::request<http::dynamic_body> m_request;
    http::response<http::dynamic_body> m_response;

    // 定时器
    net::steady_timer m_deadline{m_socket.get_executor(), std::chrono::seconds(60)};

};

