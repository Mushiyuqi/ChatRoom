#pragma once
#include "const.h"

/**
 * 使用 RAII 来管理资源
 */
class RedisManager {
public:
    ~RedisManager();
    RedisManager& operator=(const RedisManager&) = delete;
    RedisManager(const RedisManager&) = delete;

    static RedisManager& GetInstance();

    bool Connect(const std::string& host, int port);
    bool Get(const std::string &key, std::string& value);
    bool Set(const std::string &key, const std::string &value);
    bool Auth(const std::string &password);
    bool LPush(const std::string &key, const std::string &value);
    bool LPop(const std::string &key, std::string& value);
    bool RPush(const std::string& key, const std::string& value);
    bool RPop(const std::string& key, std::string& value);
    bool HSet(const std::string &key, const std::string  &hkey, const std::string &value);
    bool HSet(const char* key, const char* hkey, const char* hvalue, size_t hvaluelen);
    std::string HGet(const std::string &key, const std::string &hkey);
    bool Del(const std::string &key);
    bool ExistsKey(const std::string &key);
    void Close();

private:
    RedisManager();

    redisContext* m_connect;
    redisReply* m_reply;
};

