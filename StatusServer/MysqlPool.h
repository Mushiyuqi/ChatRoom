#pragma once
#include "const.h"
#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/exception.h>

/**
* getConnection() 连接取出后要马上使用防止连接失效
* 全部使用智能指针管理资源
*/
class SqlConnection {
public:
    SqlConnection(sql::Connection* conn, const int64_t lastTime);
    std::unique_ptr<sql::Connection> m_conn;
    int64_t m_last_oper_time;
};

class MySqlPool {
public:
    MySqlPool(const std::string& url, const std::string& user, const std::string& pass, const std::string& schema,
              const int poolSize);

    void checkConnection();

    std::unique_ptr<SqlConnection> getConnection();
    void returnConnection(std::unique_ptr<SqlConnection> con);

    void Close();
    ~MySqlPool();

private:
    std::string m_url;
    std::string m_user;
    std::string m_pass;
    std::string m_schema;
    int m_pool_size;
    std::queue<std::unique_ptr<SqlConnection>> m_pool;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    std::atomic<bool> m_b_stop;
    std::thread m_check_thread;
};

