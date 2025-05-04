#include "MysqlManager.h"

MysqlManager& MysqlManager::GetInstance() {
    static MysqlManager instance;
    return instance;
}

int MysqlManager::RegUser(const std::string& name, const std::string& email, const std::string& pwd) {
    return m_dao.RegUser(name, email, pwd);
}
