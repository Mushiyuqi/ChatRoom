#pragma once
#include <memory>
#include <map>
#include <mutex>
#include <boost/asio.hpp>
#include "CSession.h"

using boost::asio::ip::tcp;
class CServer {
public:
    CServer(boost::asio::io_context& io_context, const short port);
    ~CServer();
    void ClearSession(std::string uuid);

private:
    void HandleAccept(std::shared_ptr<CSession> session, const boost::system::error_code& error);
    void StartAccept();
    boost::asio::io_context& _io_context;
    tcp::acceptor m_acceptor;
    short m_port;
    std::map<std::string, std::shared_ptr<CSession>> m_sessions;
    std::mutex m_mutex;
};
