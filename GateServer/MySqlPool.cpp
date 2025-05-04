#include "MySqlPool.h"

SqlConnection::SqlConnection(sql::Connection* conn, const int64_t lastTime) : m_conn(conn), m_last_oper_time(lastTime) {
}

MySqlPool::MySqlPool(const std::string& url, const std::string& user, const std::string& pass,
                     const std::string& schema, const int poolSize) : m_url(url), m_user(user), m_pass(pass),
                                                                      m_schema(schema), m_pool_size(poolSize),
                                                                      m_b_stop(false) {
    try {
        for (int i = 0; i < m_pool_size; ++i) {
            sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
            auto* con = driver->connect(m_url, m_user, m_pass);
            con->setSchema(m_schema);
            // 获取当前时间戳
            auto currentTime = std::chrono::system_clock::now().time_since_epoch();
            // 将时间戳转换为秒
            long long timeStamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();
            // 用SqlConnection来管理连接
            m_pool.push(std::make_unique<SqlConnection>(con, timeStamp));
        }

        // 启动定时检测
        m_check_thread = std::thread([this]() {
            while (!m_b_stop) {
                // 检测连接
                checkConnection();
                // 每隔60秒检测一次
                std::this_thread::sleep_for(std::chrono::seconds(60));
            }
        });
        // 线程分离
        m_check_thread.detach();
    }
    catch (sql::SQLException& e) {
        // 处理异常
        std::cerr << "MySqlPool::MySqlPool mysql pool init failed, error is " << e.what() << std::endl;
    }
}

void MySqlPool::checkConnection() {
    std::lock_guard<std::mutex> lock(m_mutex);
    int poolsize = m_pool.size();
    // 获取当前时间戳
    auto currentTime = std::chrono::system_clock::now().time_since_epoch();
    // 将时间戳转换为秒
    long long timeStamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();
    for (int i = 0; i < poolsize; i++) {
        auto conn = std::move(m_pool.front());
        m_pool.pop();
        // 创建Defer对象，在函数退出时将连接放回连接池
        Defer defer([this, &conn]() {
            m_pool.push(std::move(conn));
        });

        if (timeStamp - conn->m_last_oper_time < 5) continue;

        try {
            // 执行一个查询，保持连接活动
            std::unique_ptr<sql::Statement> stmt(conn->m_conn->createStatement());
            stmt->executeQuery("SELECT 1");
            conn->m_last_oper_time = timeStamp;
        }catch (sql::SQLException& e) {
            std::cerr << "MySqlPool::checkConnection Error keeping connection alive: " << e.what() << std::endl;
            // 重新创建连接并替换旧的连接
            sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
            auto* newcon = driver->connect(m_url, m_user, m_pass);
            newcon->setSchema(m_schema);
            conn->m_conn.reset(newcon);
            conn->m_last_oper_time = timeStamp;
        }
    }
}

std::unique_ptr<SqlConnection> MySqlPool::getConnection() {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cond.wait(lock, [this] {
        if (m_b_stop) return true;
        return !m_pool.empty();
    });
    // 1. 连接池关闭
    if (m_b_stop) return nullptr;
    // 2. 连接池未关闭 && 连接池非空
    std::unique_ptr<SqlConnection> conn(std::move(m_pool.front()));
    m_pool.pop();
    return conn;
}

void MySqlPool::returnConnection(std::unique_ptr<SqlConnection> con) {
    std::unique_lock<std::mutex> lock(m_mutex);
    // 1. 连接池关闭
    if (m_b_stop) return;
    // 2. 连接池未关闭
    m_pool.push(std::move(con));
    m_cond.notify_one();
}

void MySqlPool::Close() {
    m_b_stop = true;
    m_cond.notify_all();
}

MySqlPool::~MySqlPool() {
    std::unique_lock<std::mutex> lock(m_mutex);
    while (!m_pool.empty()) {
        m_pool.pop();
    }
}
