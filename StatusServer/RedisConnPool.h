#pragma once
#include "const.h"

/**
 * 使用 RAII 来管理资源
 */
class RedisConnPool {
public:
    RedisConnPool(size_t poolSize, std::string host, int port, std::string pwd);
    ~RedisConnPool();

    redisContext* GetConnection();
    void ReturnConnection(redisContext* context);
    void Close();

private:
    std::atomic<bool> m_b_stop;
    size_t m_pool_size;
    std::string m_host;
    int m_port;
    std::string pwd;
    std::queue<redisContext*> m_connections;
    std::mutex m_mutex;
    std::condition_variable m_cv;

};

