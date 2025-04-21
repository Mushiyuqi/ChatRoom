#include "RedisManager.h"

RedisManager::~RedisManager() {
    std::cout << "RedisManager::~RedisManager destructed" << std::endl;
}

RedisManager::RedisManager(): m_connect(nullptr), m_reply(nullptr) {
    std::cout << "RedisManager::RedisManager constructed" << std::endl;
}

RedisManager& RedisManager::GetInstance() {
    static RedisManager instance;
    return instance;
}

bool RedisManager::Connect(const std::string& host, const int port) {
    // 连接redis
    m_connect = redisConnect(host.c_str(), port);
    if (m_connect == nullptr) {
        std::cerr << "RedisManager::Connect redisConnect error m_connect is nullptr" << std::endl;
        return false;
    }
    if (m_connect->err) {
        std::cerr << "RedisManager::Connect redisConnect error " << m_connect->errstr << std::endl;
        return false;
    }
    return true;
}

bool RedisManager::Get(const std::string& key, std::string& value) {
    // 获取值
    m_reply = static_cast<redisReply*>(redisCommand(m_connect, "GET %s", key.c_str()));
    if (m_reply == nullptr) {
        std::cerr << "RedisManager::Get [GET  " << key << "] redisCommand error" << std::endl;
        freeReplyObject(m_reply);
        return false;
    }
    if (m_reply->type != REDIS_REPLY_STRING) {
        std::cerr << "RedisManager::Get [GET  " << key << "] redisCommand error" << std::endl;
        freeReplyObject(m_reply);
        return false;
    }
    value = m_reply->str;
    // 释放内存
    freeReplyObject(m_reply);
    return true;
}

bool RedisManager::Set(const std::string& key, const std::string& value) {
    m_reply = static_cast<redisReply*>(redisCommand(m_connect, "SET %s %s", key.c_str(), value.c_str()));
    if (m_reply == nullptr) {
        std::cerr << "RedisManager::Set [SET  " << key << " : " << value << "] redisCommand error" << std::endl;
        freeReplyObject(m_reply);
        return false;
    }
    if (!(m_reply->type == REDIS_REPLY_STATUS && (strcmp(m_reply->str, "OK") == 0 || strcmp(m_reply->str, "ok") ==
        0))) {
        std::cerr << "RedisManager::Set [SET  " << key << " : " << value << "] redisCommand error" << std::endl;
        freeReplyObject(m_reply);
        return false;
    }

    freeReplyObject(m_reply);
    return true;
}

bool RedisManager::Auth(const std::string& password) {
    m_reply = static_cast<redisReply*>(redisCommand(m_connect, "AUTH %s", password.c_str()));
    if (m_reply->type == REDIS_REPLY_ERROR) {
        std::cerr << "RedisManager::Auth [AUTH  " << password << "] redisCommand error" << std::endl;
        freeReplyObject(m_reply);
        return false;
    }
    freeReplyObject(m_reply);
    return true;
}


