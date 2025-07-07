#include "MysqlManager.h"

MysqlManager& MysqlManager::GetInstance() {
    static MysqlManager instance;
    return instance;
}

int MysqlManager::RegUser(const std::string& name, const std::string& email, const std::string& pwd) {
    return m_dao.RegUser(name, email, pwd);
}

bool MysqlManager::CheckEmail(const std::string& name, const std::string& email) {
    return m_dao.CheckEmail(name, email);
}

bool MysqlManager::UpdatePwd(const std::string& name, const std::string& password) {
    return m_dao.UpdatePwd(name, password);
}

bool MysqlManager::CheckPassword(const std::string& email, const std::string& password, UserInfo& userInfo) {
    return m_dao.CheckPwd(email, password, userInfo);
}

std::shared_ptr<UserInfo> MysqlManager::GetUser(const int uid) {
    return m_dao.GetUser(uid);
}

std::shared_ptr<UserInfo> MysqlManager::GetUser(const std::string& name) {
    return m_dao.GetUser(name);
}

bool MysqlManager::AddFriendApply(int uid, int touid) {
    return m_dao.AddFriendApply(uid, touid);
}

bool MysqlManager::GetFriendApplyList(int uid, std::vector<std::shared_ptr<ApplyInfo>>& applyList, int begin,
                                      int limit) {
    return m_dao.GetFriendApplyList(uid, applyList, begin, limit);
}

bool MysqlManager::AuthFriendApply(const int from, const int to) {
    return m_dao.AuthFriendApply(from, to);
}

bool MysqlManager::AddFriend(const int from, const int to, const std::string& back_name) {
    return m_dao.AddFriend(from, to, back_name);
}
