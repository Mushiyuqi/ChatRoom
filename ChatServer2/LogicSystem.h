#pragma once
#include <queue>
#include <thread>
#include <map>
#include <functional>
#include <json/json.h>
#include "CSession.h"
#include "const.h"

struct UserInfo;

typedef std::function<void(std::shared_ptr<CSession>, const short& msg_id, const std::string& msg_data)> FunCallBack;
class LogicSystem {
public:
    ~LogicSystem();
    void PostMsgToQue(std::shared_ptr<LogicNode> msg);
    static LogicSystem& GetInstance();

    LogicSystem(const LogicSystem&) = delete;
    LogicSystem& operator=(const LogicSystem&) = delete;
private:
    LogicSystem();
    void DealMsg();
    void RegisterCallBacks();
    std::thread m_worker_thread;
    std::queue<std::shared_ptr<LogicNode>> m_msg_que;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    bool m_is_stop;
    std::map<short, FunCallBack> m_fun_callbacks;
    // 缓存用户信息
    std::unordered_map<int, std::shared_ptr<UserInfo>> m_users;
};

