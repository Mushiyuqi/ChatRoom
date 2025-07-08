#include "LogicSystem.h"
#include "StatusGrpcClient.h"
#include "MysqlManager.h"
#include "RedisManager.h"
#include "ConfigManager.h"
#include "UserManager.h"
#include "ChatGrpcClient.h"

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

bool LogicSystem::GetFriendList(int uid, std::vector<std::shared_ptr<UserInfo>>& friendList) {
    return MysqlManager::GetInstance().GetFriendList(uid, friendList);
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

            // 获取好友申请列表
            std::vector<std::shared_ptr<ApplyInfo>> applyList;
            flag = GetFriendApplyList(uid, applyList);
            if(flag) {
                for (auto& applyInfo : applyList) {
                    Json::Value applyJson;
                    applyJson["name"] = applyInfo->name;
                    applyJson["uid"] = applyInfo->uid;
                    applyJson["icon"] = applyInfo->icon;
                    applyJson["nick"] = applyInfo->nick;
                    applyJson["sex"] = applyInfo->sex;
                    applyJson["desc"] = applyInfo->desc;
                    applyJson["status"] = applyInfo->status;
                    rspJson["apply_list"].append(applyJson);
                 }
            }

            // 获取好友列表
            std::vector<std::shared_ptr<UserInfo>> friendList;
            flag = GetFriendList(uid, friendList);
            for (auto& e : friendList) {
                Json::Value friendJson;
                friendJson["name"] = e->name;
                friendJson["uid"] = e->uid;
                friendJson["icon"] = e->icon;
                friendJson["nick"] = e->nick;
                friendJson["sex"] = e->sex;
                friendJson["desc"] = e->desc;
                friendJson["back"] = e->back;
                rspJson["friend_list"].append(friendJson);
            }

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
    // 查询用户
    m_fun_callbacks[ReqId::ID_SEARCH_USER] =
        [this](std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data) {
            // 注册返回函数
            Json::Value returnJson;
            returnJson["error"] = ErrorCodes::Success;
            Defer defer([this, &returnJson, session]() {
                const std::string rspStr = returnJson.toStyledString();
                session->Send(rspStr, ReqId::ID_SEARCH_USER);
            });

            // 处理Json数据
            Json::Reader reader;
            Json::Value value;
            reader.parse(msg_data, value);
            // 获取数据
            auto uid = value["uid"].asString();
            std::cout << "LogicSystem::ID_SEARCH_USER recv msg is : " << uid << std::endl;

            if(IsPureDigit(uid)) {
                GetUserByUid(uid, returnJson);
            }else {
                GetUserByName(uid, returnJson);
            }
    };
    // 添加好友申请
    m_fun_callbacks[ReqId::ID_ADD_FRIEND] =
        [this](std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data) {
            // 注册返回函数
            Json::Value returnJson;
            returnJson["error"] = ErrorCodes::Success;
            Defer defer([this, &returnJson, session]() {
                const std::string rspStr = returnJson.toStyledString();
                session->Send(rspStr, ReqId::ID_ADD_FRIEND);
            });

            // 处理Json数据
            Json::Reader reader;
            Json::Value value;
            reader.parse(msg_data, value);
            // 获取数据
            auto uid = value["uid"].asInt();
            auto applyname = value["applyname"].asString();
            auto backname = value["backname"].asString();
            auto touid = value["touid"].asInt();

            // 先更新数据库
            MysqlManager::GetInstance().AddFriendApply(uid, touid);

            // 查找redis 查找touid对应的server ip
            auto touidStr = std::to_string(touid);
            auto toipKey = USERIPPREFIX + touidStr;
            std::string toipValue;
            bool flag = RedisManager::GetInstance().Get(toipKey, toipValue);
            if(!flag) {return;} // 用户不在线则退出

            // 通知好友申请
            // 查询用户信息
            std::string baseKey = USER_BASE_INFO + touidStr;
            auto userInfo = std::make_shared<UserInfo>();
            flag = GetBaseInfo(baseKey, touid, userInfo);
            auto selfName = ConfigManager::GetInstance()["SelfServer"]["Name"];
            // 在本服务器
            if(toipValue == selfName) {
                auto toSession = UserManager::GetInstance().GetSession(touid);
                if(toSession !=  nullptr) {
                    Json::Value notify;
                    notify["error"] = ErrorCodes::Success;
                    notify["applyuid"] = uid;
                    notify["name"] = applyname;
                    notify["desc"] = "";
                    if(flag) {
                        notify["icon"] = userInfo->icon;
                        notify["sex"] = userInfo->sex;
                        notify["nick"] = userInfo->nick;
                    }
                    toSession->Send(notify.toStyledString(), ReqId::ID_NOTIFY_ADD_FRIEND);
                }
                return;
            }
            // 在其他服务器
            AddFriendReq addReq;
            addReq.set_applyuid(uid);
            addReq.set_name(applyname);
            addReq.set_touid(touid);
            addReq.set_desc("");
            if (flag) {
                addReq.set_icon(userInfo->icon);
                addReq.set_sex(userInfo->sex);
                addReq.set_nick(userInfo->nick);
            }

            ChatGrpcClient::GetInstance().NotifyAddFriend(toipValue, addReq);
    };
    // 确认好友申请
    m_fun_callbacks[ReqId::ID_AUTH_FRIEND] =
        [this](std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data) {
            // 注册返回函数
            Json::Value returnJson;
            returnJson["error"] = ErrorCodes::Success;
            Defer defer([this, &returnJson, session]() {
                const std::string rspStr = returnJson.toStyledString();
                session->Send(rspStr, ReqId::ID_AUTH_FRIEND);
            });

            // 处理Json数据
            Json::Reader reader;
            Json::Value value;
            reader.parse(msg_data, value);
            // 获取数据
            auto uid = value["fromuid"].asInt();
            auto touid = value["touid"].asInt();
            auto backname = value["back"].asString();
            auto userInfo = std::make_shared<UserInfo>();

            std::string base_key = USER_BASE_INFO + std::to_string(touid);
            bool flag = GetBaseInfo(base_key, touid, userInfo);
            if(flag) {
                returnJson["name"] = userInfo->name;
                returnJson["icon"] = userInfo->icon;
                returnJson["nick"] = userInfo->nick;
                returnJson["sex"] = userInfo->sex;
                returnJson["uid"] = touid;
            }else {
                returnJson["error"] = ErrorCodes::UidInvalid;
            }

            // 先更新数据库
            MysqlManager::GetInstance().AuthFriendApply(uid, touid);

            // 更新数据库添加好友
            MysqlManager::GetInstance().AddFriend(uid, touid, backname);

            // 查找redis 查询touid对应的server ip
            auto touidStr = std::to_string(touid);
            auto toipKey = USERIPPREFIX + touidStr;
            std::string toipValue;
            flag = RedisManager::GetInstance().Get(toipKey, toipValue);
            if(!flag) {
                returnJson["error"] = ErrorCodes::UidInvalid;
                return;
            }

            auto selfName = ConfigManager::GetInstance()["SelfServer"]["Name"];
            // 直接通知对方有认证通过消息
            if(toipValue == selfName) {
                auto toSession = UserManager::GetInstance().GetSession(touid);
                if(toSession !=  nullptr) {
                    // 在内存直接通知对方
                    Json::Value notify;
                    notify["error"] = ErrorCodes::Success;
                    notify["touid"] = touid;
                    notify["fromuid"] = uid;
                    std::string baseKey = USER_BASE_INFO + std::to_string(uid);
                    auto user_info = std::make_shared<UserInfo>();
                    flag = GetBaseInfo(baseKey, uid, user_info);
                    if(flag) {
                        notify["name"] = user_info->name;
                        notify["icon"] = user_info->icon;
                        notify["nick"] = user_info->nick;
                        notify["sex"] = user_info->sex;
                    }else {
                        notify["error"] = ErrorCodes::UidInvalid;
                    }

                    toSession->Send(notify.toStyledString(), ReqId::ID_NOTIFY_AUTH_FRIEND);
                }

                return ;
            }

            // 通知该服务器发送添加好友通知
            AuthFriendReq auth_req;
            auth_req.set_fromuid(uid);
            auth_req.set_touid(touid);
            ChatGrpcClient::GetInstance().NotifyAuthFriend(toipValue, auth_req);
    };
    // 发送文本数据
    m_fun_callbacks[ReqId::ID_TEXT_CHAT_MSG] =
        [this](std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data) {
            // 注册返回函数
            Json::Value returnJson;
            returnJson["error"] = ErrorCodes::Success;
            Defer defer([this, &returnJson, session]() {
                const std::string rspStr = returnJson.toStyledString();
                session->Send(rspStr, ReqId::ID_TEXT_CHAT_MSG);
            });

            // 处理Json数据
            Json::Reader reader;
            Json::Value value;
            reader.parse(msg_data, value);
            // 获取数据
            auto uid = value["fromuid"].asInt();
            auto touid = value["touid"].asInt();

            const Json::Value arrays = value["text_array"];

            returnJson["error"] = ErrorCodes::Success;
            returnJson["text_array"] = arrays;
            returnJson["fromuid"] = uid;
            returnJson["touid"] = touid;

            // 查询redis 查询touid对应的server ip
            auto touidStr = std::to_string(touid);
            auto toipKey = USERIPPREFIX + touidStr;
            std::string toipValue;
            bool flag = RedisManager::GetInstance().Get(toipKey, toipValue);
            if(!flag) {
                returnJson["error"] = ErrorCodes::UidInvalid;
                return;
            }

            auto selfName = ConfigManager::GetInstance()["SelfServer"]["Name"];
            // 直接通知对方有消息
            if(selfName == toipValue) {
                auto toSession = UserManager::GetInstance().GetSession(touid);
                if(toSession !=  nullptr) {
                    toSession->Send(returnJson.toStyledString(), ReqId::ID_NOTIFY_TEXT_CHAT_MSG);
                }

                return;
            }

            TextChatMsgReq req;
            req.set_fromuid(uid);
            req.set_touid(touid);
            for(const auto& item : arrays) {
                auto* text_msg = req.add_textmsgs();
                text_msg->set_msgid(item["msgid"].asString());
                text_msg->set_msgcontent(item["content"].asString());
            }

            // 发送通知
            ChatGrpcClient::GetInstance().NotifyTextChatMsg(toipValue, req, returnJson);
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

bool LogicSystem::GetFriendApplyList(int uid, std::vector<std::shared_ptr<ApplyInfo>>& applyList) {
    // 从Mysql中获取
    return MysqlManager::GetInstance().GetFriendApplyList(uid, applyList, 0, 10);  // 简单模拟分页
}

bool LogicSystem::IsPureDigit(const std::string& str) {
    for(auto& ch : str) {
        if(!std::isdigit(ch))
            return false;
    }
    return true;
}

void LogicSystem::GetUserByUid(const std::string& uid_str, Json::Value& user_info) {
    user_info["error"] = ErrorCodes::Success;
    std::string base_key = USER_BASE_INFO + uid_str;
    // 优先在Redis中获取
    std::string info_str;
    bool flag = RedisManager::GetInstance().Get(base_key, info_str);
    if(flag) {
        Json::Reader reader;
        Json::Value root;
        reader.parse(info_str, root);
        user_info["uid"] = root["uid"].asInt();
        user_info["name"] = root["name"].asString();
        user_info["pwd"] = root["pwd"].asString();
        user_info["email"] = root["email"].asString();
        user_info["nick"] = root["nick"].asString();
        user_info["desc"] = root["desc"].asString();
        user_info["sex"] = root["sex"].asInt();
        user_info["icon"] = root["icon"].asString();
        return;
    }

    // Redis中没有，则从Mysql中获取
    std::shared_ptr<UserInfo> tmp = MysqlManager::GetInstance().GetUser(std::stoi(uid_str));
    if(tmp == nullptr) {
        user_info["error"] = ErrorCodes::UidInvalid;
        return;
    }
    user_info["uid"] = tmp->uid;
    user_info["name"] = tmp->name;
    user_info["pwd"] = tmp->pwd;
    user_info["email"] = tmp->email;
    user_info["nick"] = tmp->nick;
    user_info["desc"] = tmp->desc;
    user_info["sex"] = tmp->sex;
    user_info["icon"] = tmp->icon;
    // 缓存到Redis中
    RedisManager::GetInstance().Set(base_key, user_info.toStyledString());
}

void LogicSystem::GetUserByName(const std::string& name, Json::Value& user_info) {
    user_info["error"] = ErrorCodes::Success;
    std::string base_key = NAME_INFO + name;

    // 优先在Redis中获取
    std::string info_str;
    bool flag = RedisManager::GetInstance().Get(base_key, info_str);
    if(flag) {
        Json::Reader reader;
        Json::Value root;
        reader.parse(info_str, root);
        user_info["uid"] = root["uid"].asInt();
        user_info["name"] = root["name"].asString();
        user_info["pwd"] = root["pwd"].asString();
        user_info["email"] = root["email"].asString();
        user_info["nick"] = root["nick"].asString();
        user_info["desc"] = root["desc"].asString();
        user_info["sex"] = root["sex"].asInt();
        user_info["icon"] = root["icon"].asString();
        return;
    }

    // Redis中没有，则从Mysql中获取
    std::shared_ptr<UserInfo> tmp = MysqlManager::GetInstance().GetUser(name);
    if(tmp == nullptr) {
        user_info["error"] = ErrorCodes::UidInvalid;
        return;
    }
    user_info["uid"] = tmp->uid;
    user_info["name"] = tmp->name;
    user_info["pwd"] = tmp->pwd;
    user_info["email"] = tmp->email;
    user_info["nick"] = tmp->nick;
    user_info["desc"] = tmp->desc;
    user_info["sex"] = tmp->sex;
    user_info["icon"] = tmp->icon;
    RedisManager::GetInstance().Set(base_key, user_info.toStyledString());
}

