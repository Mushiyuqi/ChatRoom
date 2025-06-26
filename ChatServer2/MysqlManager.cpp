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
    return  m_dao.CheckPwd(email, password, userInfo);
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
