#include "CServer.h"
#include "HttpConnection.h"

CServer::CServer(net::io_context& ioc, const unsigned short port):
    m_acceptor(ioc, tcp::endpoint(tcp::v4(), port)), _ioc(ioc), m_socket(ioc) {
}

void CServer::Start() {
    auto self = shared_from_this();
    m_acceptor.async_accept(m_socket, [this, self](const std::error_code ec) {
        try {
            // 出错断开连接, 继续监听其他连接
            if (ec) {
                Start();
                return;
            }

            // 创建连接, HttpConnection 管理一个连接
            std::make_shared<HttpConnection>(std::move(m_socket))->Start();
            // 继续监听其他连接
            Start();
        }
        catch (std::exception& e) {
            std::cerr << "CServer::Start() Error " << e.what() << std::endl;
        }
    });
}
