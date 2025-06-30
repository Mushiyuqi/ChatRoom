#pragma once
#include <queue>
#include <thread>
#include <map>
#include <functional>
#include <memory>
#include <vector>
#include <json/json.h>
#include "CSession.h"
#include "const.h"
#include "data.h"

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
    bool GetBaseInfo(const std::string& baseKey, int uid, std::shared_ptr<UserInfo> &userInfo);
    bool GetFriendApplyList(int uid, std::vector<std::shared_ptr<ApplyInfo>>& vector);

    static bool IsPureDigit(const std::string& str);
    void GetUserByUid(const std::string& uid_str, Json::Value& user_info);
    void GetUserByName(const std::string& name, Json::Value& user_info);

    std::thread m_worker_thread;
    std::queue<std::shared_ptr<LogicNode>> m_msg_que;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    bool m_is_stop;
    std::map<short, FunCallBack> m_fun_callbacks;
};

