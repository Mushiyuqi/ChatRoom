#include "CServer.h"
#include "HttpConnection.h"
#include "IOContextPool.h"

CServer::CServer(net::io_context& ioc, const unsigned short port):
    m_acceptor(ioc, tcp::endpoint(tcp::v4(), port)){
}

void CServer::Start() {
    auto self = shared_from_this();
    auto& iocontext = IOContextPool::GetInstance().GetIOContext();
    const auto newConn = std::make_shared<HttpConnection>(iocontext);
    m_acceptor.async_accept(newConn->GetSocket(), [this, self, newConn](const std::error_code ec) {
        boost::ignore_unused(self);
        try {
            // 出错断开连接, 继续监听其他连接
            if (ec) {
                Start();
                return;
            }
            // 监听消息
            newConn->Start();
            // 继续监听其他连接
            Start();
        }
        catch (std::exception& e) {
            std::cerr << "CServer::Start() Error " << e.what() << std::endl;
        }
    });
}
