#pragma once
#include <unordered_map>
#include <memory>
#include <mutex>

class CSession;
class UserManager {
public:
    // 禁止拷贝构造和赋值
    UserManager(const UserManager&) = delete;
    UserManager& operator=(const UserManager&) = delete;

    ~UserManager();
    static UserManager& GetInstance();

    std::shared_ptr<CSession> GetSession(int uid);
    void SetUserSession(int uid, std::shared_ptr<CSession> session);
    void RemoveUserSession(int uid);

private:
    UserManager() = default;

    std::mutex m_mutex;
    std::unordered_map<int, std::shared_ptr<CSession>> m_uid_session;

};
