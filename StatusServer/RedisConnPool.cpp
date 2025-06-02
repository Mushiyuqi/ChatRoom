#include "RedisConnPool.h"

RedisConnPool::RedisConnPool(size_t poolSize, std::string host, int port, std::string pwd)
    : m_b_stop(false), m_pool_size(poolSize), m_host(host), m_port(port) {
    // 初始化连接池
    for (size_t i = 0; i < m_pool_size; ++i) {
        // 连接
        auto* context = redisConnect(host.c_str(), port);
        if (context == nullptr || context->err != 0) {
            if (context != nullptr) {
                redisFree(context);
            }
            continue;
        }
        // 认证
        auto reply = static_cast<redisReply*>(redisCommand(context, "AUTH %s", pwd.c_str()));
        if (reply->type == REDIS_REPLY_ERROR) {
            std::cerr << "RedisConnPool::RedisConnPool redisConnect 认证失败" << std::endl;
            freeReplyObject(reply);
            redisFree(context);
            continue;
        }

        freeReplyObject(reply);
        m_connections.push(context);
    }
}

RedisConnPool::~RedisConnPool() {
    Close();
    std::lock_guard<std::mutex> lock(m_mutex);
    // 释放资源
    while (!m_connections.empty()) {
        auto* context = m_connections.front();
        m_connections.pop();
        redisFree(context);
    }
}

redisContext* RedisConnPool::GetConnection() {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cv.wait(lock, [this] {
        if (m_b_stop) {
            return true;
        }
        return !m_connections.empty();
    });
    //如果停止则直接返回空指针
    if (m_b_stop) return nullptr;
    auto* context = m_connections.front();
    m_connections.pop();
    return context;
}

void RedisConnPool::ReturnConnection(redisContext* context) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_b_stop) {
        if (context != nullptr) redisFree(context);
        return;
    }
    m_connections.push(context);
    m_cv.notify_one();
}

void RedisConnPool::Close() {
    m_b_stop = true;
    m_cv.notify_all();
}
