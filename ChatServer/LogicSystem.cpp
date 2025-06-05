#include "LogicSystem.h"
#include "StatusGrpcClient.h"
#include "MysqlManager.h"

LogicSystem::LogicSystem(): m_is_stop(false) {
    // 注册回调函数
    RegisterCallBacks();
    // 启动工作线程
    m_worker_thread = std::thread(&LogicSystem::DealMsg, this);
}

LogicSystem::~LogicSystem() {
    m_is_stop = true;
    m_cond.notify_one();
    m_worker_thread.join();
    std::cerr << "LogicSystem::~LogicSystem()" << std::endl;
}

void LogicSystem::PostMsgToQue(std::shared_ptr<LogicNode> msg) {
    std::unique_lock<std::mutex> lock(m_mutex);
    // todo 接收队列满时，发送服务器繁忙信息
    m_msg_que.push(std::move(msg));
    if (m_msg_que.size() == 1) {
        m_cond.notify_one();
    }
}

LogicSystem& LogicSystem::GetInstance() {
    static LogicSystem instance;
    return instance;
}

void LogicSystem::DealMsg() {
    while (!m_is_stop) {
        std::unique_lock<std::mutex> lock(m_mutex);
        // ReSharper disable once CppDFAConstantConditions
        while (m_msg_que.empty() && !m_is_stop) {
            m_cond.wait(lock);
        }

        // 1.队列为空 线程停止
        // 2.队列非空 线程停止
        // ReSharper disable once CppDFAConstantConditions
        if (m_is_stop) {
            // 线程停止时，需要清空队列
            // ReSharper disable once CppDFAUnreachableCode
            while (!m_msg_que.empty()) {
                // 获取消息
                auto msg_node = m_msg_que.front();
                std::cout << "LogicSystem::DealMsg recv msg id is : " << msg_node->_recv_node->m_msg_id << std::endl;
                // 处理消息
                auto call_back_iter = m_fun_callbacks.find(msg_node->_recv_node->m_msg_id);
                if (call_back_iter == m_fun_callbacks.end()) {
                    m_msg_que.pop();
                    continue;
                }

                call_back_iter->second(msg_node->_session, msg_node->_recv_node->m_msg_id,
                                       std::string(msg_node->_recv_node->m_data, msg_node->_recv_node->m_total_len));

                m_msg_que.pop();
            }
            // 退出DealMsg()
            break;
        }

        // 3.队列非空 线程继续
        // 获取消息
        auto msg_node = m_msg_que.front();
        std::cout << "LogicSystem::DealMsg recv msg id is : " << msg_node->_recv_node->m_msg_id << std::endl;
        // 处理消息
        auto call_back_iter = m_fun_callbacks.find(msg_node->_recv_node->m_msg_id);
        if (call_back_iter == m_fun_callbacks.end()) {
            m_msg_que.pop();
            continue;
        }

        call_back_iter->second(msg_node->_session, msg_node->_recv_node->m_msg_id,
                               std::string(msg_node->_recv_node->m_data, msg_node->_recv_node->m_total_len));

        m_msg_que.pop();
    }
}

void LogicSystem::RegisterCallBacks() {
    // 测试ChatRoom的登陆功能
    m_fun_callbacks[ReqId::ID_CHAT_LOGIN] =
        [this](std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data) {
            // 处理Json数据
            Json::Reader reader;
            Json::Value value;
            reader.parse(msg_data, value);
            // 获取数据
            int uid = value["uid"].asInt();
            std::string token = value["token"].asString();

            std::cout << "LogicSystem::ID_CHAT_LOGIN recv msg id is : " << uid << std::endl;
            std::cout << "LogicSystem::ID_CHAT_LOGIN recv msg is    : " << token << std::endl;

            // 注册返回函数
            Json::Value rspJson;
            Defer defer([this, &rspJson, session]() {
                const std::string rspStr = rspJson.toStyledString();
                session->Send(rspStr, ReqId::ID_CHAT_LOGIN);
            });

            // 从状态服务器获取Token进行匹配
            auto rspRpc = StatusGrpcClient::GetInstance().Login(uid, token);
            rspJson["error"] = rspRpc.error();
            if (rspRpc.error() != ErrorCodes::Success) {
                return;
            }

            std::shared_ptr<UserInfo> userInfo = nullptr;
            // 从内存中查询用户信息
            if (!m_users.contains(uid)) {
                // 从数据库中查询用户信息
                userInfo = MysqlManager::GetInstance().GetUser(uid);
                if (userInfo == nullptr) {
                    rspJson["error"] = ErrorCodes::UidInvalid;
                    return;
                }
                m_users[uid] = userInfo;
            }
            else {
                userInfo = m_users[uid];
            }

            rspJson["uid"] = uid;
            rspJson["token"] = token;
            rspJson["name"] = userInfo->name;
        };
}

