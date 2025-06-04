#pragma once
#include <memory>
#include <boost/asio.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <queue>
#include <mutex>
#include "const.h"
#include "MsgNode.h"

using boost::asio::ip::tcp;
using boost::asio::co_spawn;
using boost::asio::awaitable;
using boost::asio::detached;
using boost::asio::use_awaitable;
using boost::asio::strand;
using boost::asio::as_tuple;
namespace this_coro = boost::asio::this_coro;

class CServer;

class CSession : public std::enable_shared_from_this<CSession> {
public:
    CSession(boost::asio::io_context& io_context, CServer* server);
    ~CSession();
    boost::asio::ip::tcp::socket& GetSocket();
    std::string& GetUUID();
    void Start();
    void Close();
    // 采用回调函数的形式来处理随机发送
    void Send(const char* msg, const short msg_len, const short msg_id);
    void Send(const std::string& msg, const short msg_id);

private:
    void HandleWrite(boost::system::error_code ec, std::shared_ptr<CSession> self);

private:
    /**
    * m_flag_close: 当该标志为true时，表示通信已关闭。
    * m_send_lock: 互斥锁，用于保护发送队列的并发访问
    * m_send_que: 发送队列
    * m_recv_msg_que: 接收消息队列，包含接收到的数据
    * m_recv_head_que: 接收头队列，包含消息的头部信息
    */
    boost::asio::io_context& _io_context;
    boost::asio::ip::tcp::socket m_socket;
    CServer* _server;
    std::string m_uuid;
    bool m_flag_close;
    std::mutex m_send_lock;
    std::queue<std::shared_ptr<SendNode>> m_send_que;
    std::shared_ptr<RecvNode> m_recv_msg_node;
    std::shared_ptr<MsgNode> m_recv_head_node;
};

class LogicSystem;

class LogicNode {
    friend class LogicSystem;

public:
    LogicNode(std::shared_ptr<CSession> session, std::shared_ptr<RecvNode> msg);
    std::shared_ptr<CSession> _session;
    std::shared_ptr<RecvNode> _recv_node;
};
