#include "CSession.h"
#include "CServer.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>

#include "LogicSystem.h"

CSession::CSession(boost::asio::io_context& io_context, CServer* server): _io_context(io_context), m_socket(io_context),
                                                                          _server(server), m_flag_close(false) {
    const boost::uuids::uuid uuid = boost::uuids::random_generator()();
    m_uuid = boost::uuids::to_string(uuid);

    // 初始化接收消息头节点
    m_recv_head_node = std::make_shared<MsgNode>(HEAD_TOTAL_LEN);
    std::cout << "CSession::CSession() uuid is : " << m_uuid << std::endl;
}

CSession::~CSession() {
    try {
        std::cerr << "CSession::~CSession() uuid is : " << m_uuid << std::endl;
    }
    catch (std::exception& e) {
        std::cerr << "CSession::~CSession() uuid is : " << m_uuid << "  Error: " << e.what() << std::endl;
    }
}

boost::asio::ip::tcp::socket& CSession::GetSocket() {
    return m_socket;
}

std::string& CSession::GetUUID() {
    return m_uuid;
}

void CSession::Start() {
    auto shared_this = shared_from_this();
    // 开启协程接收
    // [=] 以值的方式捕获 shared_this 引用计数加1
    co_spawn(_io_context, [this, shared_this]() -> awaitable<void> {
        try {
            while (!m_flag_close) {
                /**
                 * 读取消息头
                 */
                m_recv_head_node->Clear();
                auto [error, size] = co_await boost::asio::async_read(
                    m_socket, boost::asio::buffer(m_recv_head_node->m_data, HEAD_TOTAL_LEN),
                    as_tuple(use_awaitable));
                if (error) throw std::runtime_error(error.message());

                // 获取头部Msg数据
                short msg_id = 0;
                memcpy(&msg_id, m_recv_head_node->m_data, HEAD_ID_LEN);
                // 转换为本地字节序
                msg_id = boost::asio::detail::socket_ops::network_to_host_short(msg_id);
                if (msg_id > MAX_MSG_LENGTH) throw std::runtime_error("invalid msg id is " + std::to_string(msg_id));

                short msg_len = 0;
                memcpy(&msg_len, m_recv_head_node->m_data + HEAD_ID_LEN, HEAD_DATA_LEN);
                // 转换为本地字节序
                msg_len = boost::asio::detail::socket_ops::network_to_host_short(msg_len);
                if (msg_len > MAX_MSG_LENGTH) throw std::runtime_error("invalid msg len is " + std::to_string(msg_len));

                std::cout << "\033[32m## receive a head, msg id is : " << msg_id;
                std::cout << ", msg len is : " << msg_len;
                std::cout << " ##\033[0m" << std::endl;


                /**
                 * 读取消息体
                 */
                m_recv_msg_node = std::make_shared<RecvNode>(msg_len, msg_id);
                auto [msg_error, msg_size] = co_await boost::asio::async_read(
                    m_socket, boost::asio::buffer(m_recv_msg_node->m_data, msg_len),
                    as_tuple(use_awaitable));
                if (msg_error) throw std::runtime_error(msg_error.message());

                //投递消息到逻辑线程
                LogicSystem::GetInstance().PostMsgToQue(
                    std::make_shared<LogicNode>(shared_from_this(), m_recv_msg_node));
            }
            throw std::runtime_error("Start quit");
        }
        catch (std::exception& e) {
            std::cerr << "CSession::Start Error: " << e.what() << std::endl;
            Close();
            _server->ClearSession(m_uuid);
            co_return;
        }
    }, detached);
}

void CSession::Close() {
    m_socket.close();
    m_flag_close = true;
}

void CSession::Send(const char* msg, const short msg_len, const short msg_id) {
    std::unique_lock<std::mutex> lock(m_send_lock);
    const int send_que_size = m_send_que.size();
    if (send_que_size > MAX_SENDQUE) {
        std::cerr << "CSession::Send send queue is fulled, UUID is " << m_uuid << std::endl;
        return;
    }
    m_send_que.push(std::make_shared<SendNode>(msg, msg_len, msg_id));
    if (send_que_size > 0) {
        return;
    }

    // 发送队列的第一个数据
    auto msg_node = m_send_que.front();
    lock.unlock();

    boost::asio::async_write(m_socket, boost::asio::buffer(msg_node->m_data, msg_node->m_total_len),
                             std::bind(&CSession::HandleWrite, this, std::placeholders::_1, shared_from_this()));
}

void CSession::Send(const std::string& msg, const short msg_id) {
    Send(msg.c_str(), msg.size(), msg_id);
}

void CSession::HandleWrite(boost::system::error_code ec, std::shared_ptr<CSession> self) {
    try {
        // 出错抛出异常
        if (ec) throw std::runtime_error(ec.message());
        std::unique_lock<std::mutex> lock(m_send_lock);
        m_send_que.pop();
        // 队列为空停止发送
        if (m_send_que.empty()) return;
        const auto msg_node = m_send_que.front();
        lock.unlock();
        boost::asio::async_write(m_socket, boost::asio::buffer(msg_node->m_data, msg_node->m_total_len),
                                 std::bind(&CSession::HandleWrite, this, std::placeholders::_1, shared_from_this()));
    }
    catch (std::exception& e) {
        std::cerr << "CSession::HandleWrite Error: " << e.what() << std::endl;
        Close();
        _server->ClearSession(m_uuid);
    }
}

LogicNode::LogicNode(std::shared_ptr<CSession> session, std::shared_ptr<RecvNode> msg):
    _session(std::move(session)), _recv_node(std::move(msg)) {
}


