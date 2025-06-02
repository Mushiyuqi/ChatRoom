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
