#include "UserManager.h"
#include "CSession.h"
#include "RedisManager.h"

UserManager::~UserManager() {
    // 清理数据
    m_uid_session.clear();
}

UserManager& UserManager::GetInstance() {
    static UserManager instance;
    return instance;
}

std::shared_ptr<CSession> UserManager::GetSession(int uid) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto iter = m_uid_session.find(uid);
    if (iter == m_uid_session.end()) {
        return nullptr;
    }
    return iter->second;
}

void UserManager::SetUserSession(const int uid, std::shared_ptr<CSession> session) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_uid_session[uid] = session;
}

void UserManager::RemoveUserSession(const int uid) {
    auto uidStr = std::to_string(uid);
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_uid_session.erase(uid);
    }
}


