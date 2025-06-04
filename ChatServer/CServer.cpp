#include "CServer.h"
#include "AsioIOContextPool.h"
#include <iostream>

CServer::CServer(boost::asio::io_context& io_context, const short port): _io_context(io_context),
                                                                         m_acceptor(_io_context,
                                                                             tcp::endpoint(tcp::v4(), port)),
                                                                         m_port(port) {
    std::cout << "CServer::CServer() server is running" << std::endl
        << "ip is   : " << m_acceptor.local_endpoint().address() << std::endl
        << "port is : " << m_acceptor.local_endpoint().port() << std::endl;

    StartAccept();
}

CServer::~CServer() {
    m_acceptor.close();
    std::cerr << "CServer::~CServer()" << std::endl;
}

void CServer::ClearSession(std::string uuid) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_sessions.erase(uuid);
}

void CServer::HandleAccept(std::shared_ptr<CSession> session, const boost::system::error_code& error) {
    if (!error) {
        session->Start();
        std::lock_guard<std::mutex> lock(m_mutex);
        m_sessions[session->GetUUID()] = session;
    }
    else {
        std::cerr << "CServer::HandleAccept error: " << error.what() << std::endl;
    }
    // 继续接收连接请求
    StartAccept();
}

void CServer::StartAccept() {
    // 获取一个io_context
    auto& io_context = AsioIOContextPool::GetInstance().GetIOContext();
    auto session = std::make_shared<CSession>(io_context, this);
    m_acceptor.async_accept(session->GetSocket(),
                            std::bind(&CServer::HandleAccept, this, session, std::placeholders::_1));
}

