#pragma once
#include "const.h"
#include "MysqlDao.h"

class MysqlManager {
public:
    ~MysqlManager() = default;
    MysqlManager(const MysqlManager&) = delete;
    MysqlManager& operator=(const MysqlManager&) = delete;
    static MysqlManager& GetInstance();
    // 注册用户
    int RegUser(const std::string& name, const std::string& email, const std::string& pwd);
    bool CheckEmail(const std::string& name, const std::string& email);
    bool UpdatePwd(const std::string& name, const std::string& password);
    bool CheckPassword(const std::string& email, const std::string& password, UserInfo& userInfo);
    std::shared_ptr<UserInfo> GetUser(int uid);

private:
    MysqlManager() = default;
    MysqlDao m_dao;
};

