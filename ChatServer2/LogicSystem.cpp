#include "LogicSystem.h"
#include "StatusGrpcClient.h"
#include "MysqlManager.h"
#include "RedisManager.h"
#include "ConfigManager.h"
#include "UserManager.h"

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
            rspJson["error"] = ErrorCodes::Success;

            // 从Redis获取Token进行匹配
            std::string uidStr = std::to_string(uid);
            std::string tokenKey = USERTOKENPREFIX + uidStr;
            std::string tokenValue;
            bool success = RedisManager::GetInstance().Get(tokenKey, tokenValue);
            if (!success) {
                rspJson["error"] = ErrorCodes::UidInvalid;
                return;
            }
            if (tokenValue != token) {
                rspJson["error"] = ErrorCodes::TokenInvalid;
                return;
            }

            // 获取基础信息
            std::string baseKey = USER_BASE_INFO + uidStr;
            auto userInfo = std::make_shared<UserInfo>();
            bool flag = GetBaseInfo(baseKey, uid, userInfo);
            if (!flag) {
                rspJson["error"] = ErrorCodes::UidInvalid;
                return;
            }

            rspJson["uid"] = uid;
            rspJson["pwd"] = userInfo->pwd;
            rspJson["name"] = userInfo->name;
            rspJson["email"] = userInfo->email;
            rspJson["nick"] = userInfo->nick;
            rspJson["desc"] = userInfo->desc;
            rspJson["sex"] = userInfo->sex;
            rspJson["icon"] = userInfo->icon;

            // todo 从数据库获取申请列表

            // todo 获取好友列表

            // 将登陆数量增加
            auto serverName = ConfigManager::GetInstance()["SelfServer"]["Name"];
            std::string tmp;
            RedisManager::GetInstance().HGet(LOGIN_COUNT, serverName, tmp);
            int count = 0;
            if(!tmp.empty()) {
                count = std::stoi(tmp);
            }
            ++count;
            auto countStr = std::to_string(count);
            RedisManager::GetInstance().HSet(LOGIN_COUNT, serverName, countStr);

            // 将session绑定用户
            session->SetUserId(uid);

            // 为用户设置登陆ip server名字
            std::string ipKey = USERIPPREFIX + uidStr;
            RedisManager::GetInstance().Set(ipKey, serverName);

            // 绑定uid与session
            UserManager::GetInstance().SetUserSession(uid, session);
        };

    m_fun_callbacks[ReqId::ID_SEARCH_USER] =
        [this](std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data) {
            // 处理Json数据
            Json::Reader reader;
            Json::Value value;
            reader.parse(msg_data, value);
            // 获取数据
            auto uid = value["uid"].asString();

            std::cout << "LogicSystem::ID_SEARCH_USER recv msg id is : " << uid << std::endl;

            // 注册返回函数
            Json::Value rspJson;
            Defer defer([this, &rspJson, session]() {
                const std::string rspStr = rspJson.toStyledString();
                session->Send(rspStr, ReqId::ID_SEARCH_USER);
            });
            rspJson["error"] = ErrorCodes::Success;


            // 获取基础信息
            std::string uidStr = std::to_string(uid);
            std::string baseKey = USER_BASE_INFO + uidStr;
            auto userInfo = std::make_shared<UserInfo>();
            bool flag = GetBaseInfo(baseKey, uid, userInfo);
            if (!flag) {
                rspJson["error"] = ErrorCodes::UidInvalid;
                return;
            }

            rspJson["uid"] = uid;
            rspJson["pwd"] = userInfo->pwd;
            rspJson["name"] = userInfo->name;
            rspJson["email"] = userInfo->email;
            rspJson["nick"] = userInfo->nick;
            rspJson["desc"] = userInfo->desc;
            rspJson["sex"] = userInfo->sex;
            rspJson["icon"] = userInfo->icon;

        };
}

bool LogicSystem::GetBaseInfo(const std::string& baseKey, int uid, std::shared_ptr<UserInfo>& userInfo) {
    // 优先在Redis中获取
    std::string infoStr;
    bool flag = RedisManager::GetInstance().Get(baseKey, infoStr);
    if(flag) {
        Json::Reader reader;
        Json::Value root;
        reader.parse(infoStr, root);
        userInfo->uid = root["uid"].asInt();
        userInfo->name = root["name"].asString();
        userInfo->pwd = root["pwd"].asString();
        userInfo->email = root["email"].asString();
        userInfo->nick = root["nick"].asString();
        userInfo->desc = root["desc"].asString();
        userInfo->sex = root["sex"].asInt();
        userInfo->icon = root["icon"].asString();
        return true;
    }
    // Redis中没有，则从Mysql中获取
    std::shared_ptr<UserInfo> tmp = MysqlManager::GetInstance().GetUser(uid);
    if(tmp == nullptr) return false;
    userInfo = tmp;

    // 缓存到Redis中
    Json::Value root;
    root["uid"] = userInfo->uid;
    root["name"] = userInfo->name;
    root["pwd"] = userInfo->pwd;
    root["email"] = userInfo->email;
    root["nick"] = userInfo->nick;
    root["desc"] = userInfo->desc;
    root["sex"] = userInfo->sex;
    root["icon"] = userInfo->icon;
    RedisManager::GetInstance().Set(baseKey, root.toStyledString());
    return true;
}

