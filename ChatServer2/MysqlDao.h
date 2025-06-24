#pragma once
#include "const.h"
#include "data.h"

class MySqlPool;
class MysqlDao
{
public:
    MysqlDao();
    ~MysqlDao();
    int RegUser(const std::string& name, const std::string& email, const std::string& pwd);
    bool CheckEmail(const std::string& name, const std::string & email);
    bool UpdatePwd(const std::string& name, const std::string& newpwd);
    bool CheckPwd(const std::string& email, const std::string& pwd, UserInfo& userInfo);
    std::shared_ptr<UserInfo> GetUser(int uid);

    // bool TestProcedure(const std::string& email, int& uid, std::string& name);
private:
    std::unique_ptr<MySqlPool> m_pool;
};

