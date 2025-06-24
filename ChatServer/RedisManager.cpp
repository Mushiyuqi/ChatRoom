#include "RedisManager.h"
#include "RedisConnPool.h"
#include "ConfigManager.h"

RedisManager::~RedisManager() {
    // 关闭连接
    Close();
    std::cout << "RedisManager::~RedisManager destructed" << std::endl;
}

RedisManager::RedisManager() {
    // 初始化连接池
    auto host = ConfigManager::GetInstance()["Redis"]["Host"];
    auto port = ConfigManager::GetInstance()["Redis"]["Port"];
    auto password = ConfigManager::GetInstance()["Redis"]["Password"];
    m_pool.reset(new RedisConnPool(RedisPoolSize, host.c_str(), atoi(port.c_str()), password.c_str()));
    std::cout << "RedisManager::RedisManager constructed" << std::endl;
}

RedisManager& RedisManager::GetInstance() {
    static RedisManager instance;
    return instance;
}

bool RedisManager::Get(const std::string& key, std::string& value) {
    auto connect = m_pool->GetConnection();
    if (connect == nullptr) return false;
    // 获取值
    auto m_reply = static_cast<redisReply*>(redisCommand(connect, "GET %s", key.c_str()));
    if (m_reply == nullptr) {
        std::cerr << "RedisManager::Get [ GET  " << key << " ] error" << std::endl;
        freeReplyObject(m_reply);
        m_pool->ReturnConnection(connect);
        return false;
    }
    if (m_reply->type != REDIS_REPLY_STRING) {
        std::cerr << "RedisManager::Get [ GET  " << key << " ] error" << std::endl;
        freeReplyObject(m_reply);
        m_pool->ReturnConnection(connect);
        return false;
    }
    value = m_reply->str;
    // 释放内存
    freeReplyObject(m_reply);
    m_pool->ReturnConnection(connect);
    return true;
}

bool RedisManager::Set(const std::string& key, const std::string& value) {
    auto connect = m_pool->GetConnection();
    if (connect == nullptr) return false;
    auto* m_reply = static_cast<redisReply*>(redisCommand(connect, "SET %s %s", key.c_str(), value.c_str()));
    if (m_reply == nullptr) {
        std::cerr << "RedisManager::Set [ SET  " << key << " : " << value << " ] error" << std::endl;
        freeReplyObject(m_reply);
        m_pool->ReturnConnection(connect);
        return false;
    }
    if (!(m_reply->type == REDIS_REPLY_STATUS && (strcmp(m_reply->str, "OK") == 0 || strcmp(m_reply->str, "ok") ==
        0))) {
        std::cerr << "RedisManager::Set [ SET  " << key << " : " << value << " ] error" << std::endl;
        freeReplyObject(m_reply);
        m_pool->ReturnConnection(connect);
        return false;
    }

    freeReplyObject(m_reply);
    m_pool->ReturnConnection(connect);
    return true;
}

bool RedisManager::LPush(const std::string& key, const std::string& value) {
    auto connect = m_pool->GetConnection();
    if (connect == nullptr) return false;
    auto* m_reply = static_cast<redisReply*>(redisCommand(connect, "LPUSH %s %s", key.c_str(), value.c_str()));
    if (m_reply == nullptr) {
        std::cerr << "RedisManager::LPush [ LPUSH  " << key << " : " << value << " ] error" << std::endl;
        freeReplyObject(m_reply);
        m_pool->ReturnConnection(connect);
        return false;
    }

    if (m_reply->type != REDIS_REPLY_INTEGER || m_reply->integer <= 0) {
        std::cerr << "RedisManager::LPush [ LPUSH  " << key << " : " << value << " ] error" << std::endl;
        freeReplyObject(m_reply);
        m_pool->ReturnConnection(connect);
        return false;
    }

    freeReplyObject(m_reply);
    m_pool->ReturnConnection(connect);
    return true;
}

bool RedisManager::LPop(const std::string& key, std::string& value) {
    auto connect = m_pool->GetConnection();
    if (connect == nullptr) return false;
    auto* m_reply = static_cast<redisReply*>(redisCommand(connect, "LPOP %s ", key.c_str()));
    if (m_reply == nullptr || m_reply->type == REDIS_REPLY_NIL) {
        std::cerr << "RedisManager::LPop [ LPOP " << key << " ] error" << std::endl;
        freeReplyObject(m_reply);
        m_pool->ReturnConnection(connect);
        return false;
    }

    value = m_reply->str;
    freeReplyObject(m_reply);
    m_pool->ReturnConnection(connect);
    return true;
}

bool RedisManager::RPush(const std::string& key, const std::string& value) {
    auto connect = m_pool->GetConnection();
    if (connect == nullptr) return false;
    auto* m_reply = static_cast<redisReply*>(redisCommand(connect, "RPUSH %s %s", key.c_str(), value.c_str()));
    if (m_reply == nullptr) {
        std::cerr << "RedisManager::RPush [ RPUSH  " << key << " : " << value << " ] error" << std::endl;
        freeReplyObject(m_reply);
        m_pool->ReturnConnection(connect);
        return false;
    }

    if (m_reply->type != REDIS_REPLY_INTEGER || m_reply->integer <= 0) {
        std::cerr << "RedisManager::RPush [ RPUSH  " << key << " : " << value << " ] error" << std::endl;
        freeReplyObject(m_reply);
        m_pool->ReturnConnection(connect);
        return false;
    }

    freeReplyObject(m_reply);
    m_pool->ReturnConnection(connect);
    return true;
}

bool RedisManager::RPop(const std::string& key, std::string& value) {
    auto connect = m_pool->GetConnection();
    if (connect == nullptr) return false;
    auto* m_reply = static_cast<redisReply*>(redisCommand(connect, "RPOP %s ", key.c_str()));
    if (m_reply == nullptr || m_reply->type == REDIS_REPLY_NIL) {
        std::cerr << "RedisManager::RPop [ RPOP " << key << " ] error" << std::endl;
        freeReplyObject(m_reply);
        m_pool->ReturnConnection(connect);
        return false;
    }

    value = m_reply->str;
    freeReplyObject(m_reply);
    m_pool->ReturnConnection(connect);
    return true;
}

bool RedisManager::HSet(const std::string& key, const std::string& hkey, const std::string& value) {
    auto connect = m_pool->GetConnection();
    if (connect == nullptr) return false;
    auto* m_reply = static_cast<redisReply*>
        (redisCommand(connect, "HSET %s %s %s", key.c_str(), hkey.c_str(), value.c_str()));
    if (m_reply == nullptr || m_reply->type != REDIS_REPLY_INTEGER) {
        std::cerr << "RedisManager::HSet' [ HSet " << key << "  " << hkey << "  " << value << " ] error" << std::endl;
        freeReplyObject(m_reply);
        m_pool->ReturnConnection(connect);
        return false;
    }

    freeReplyObject(m_reply);
    m_pool->ReturnConnection(connect);
    return true;
}

bool RedisManager::HSet(const char* key, const char* hkey, const char* hvalue, const size_t hvaluelen) {
    auto connect = m_pool->GetConnection();
    if (connect == nullptr) return false;
    const char* argv[4];
    size_t argvlen[4];
    argv[0] = "HSET";
    argvlen[0] = 4;
    argv[1] = key;
    argvlen[1] = strlen(key);
    argv[2] = hkey;
    argvlen[2] = strlen(hkey);
    argv[3] = hvalue;
    argvlen[3] = hvaluelen;
    auto* m_reply = static_cast<redisReply*>(redisCommandArgv(connect, 4, argv, argvlen));
    if (m_reply == nullptr || m_reply->type != REDIS_REPLY_INTEGER) {
        std::cerr << "RedisManager::HSet'' [ HSet " << key << "  " << hkey << "  " << hvalue << " ] error" << std::endl;
        freeReplyObject(m_reply);
        m_pool->ReturnConnection(connect);
        return false;
    }

    freeReplyObject(m_reply);
    m_pool->ReturnConnection(connect);
    return true;
}

bool RedisManager::HGet(const std::string& key, const std::string& hkey, std::string& value) {
    auto connect = m_pool->GetConnection();
    if (connect == nullptr) return false;
    const char* argv[3];
    size_t argvlen[3];
    argv[0] = "HGET";
    argvlen[0] = 4;
    argv[1] = key.c_str();
    argvlen[1] = key.length();
    argv[2] = hkey.c_str();
    argvlen[2] = hkey.length();
    auto* m_reply = static_cast<redisReply*>(redisCommandArgv(connect, 3, argv, argvlen));
    if (m_reply == nullptr || m_reply->type == REDIS_REPLY_NIL) {
        freeReplyObject(m_reply);
        m_pool->ReturnConnection(connect);
        std::cerr << "RedisManager::HGet [ HGet " << key << " " << hkey << " ] error ! " << std::endl;
        return false;
    }

    value = std::move(std::string(m_reply->str));
    freeReplyObject(m_reply);
    m_pool->ReturnConnection(connect);
    return true;
}

bool RedisManager::Del(const std::string& key) {
    auto connect = m_pool->GetConnection();
    if (connect == nullptr) return false;
    auto* m_reply = static_cast<redisReply*>(redisCommand(connect, "DEL %s", key.c_str()));
    if (m_reply == nullptr || m_reply->type != REDIS_REPLY_INTEGER) {
        std::cerr << "RedisManager::Del [ Del " << key << " ] error ! " << std::endl;
        freeReplyObject(m_reply);
        m_pool->ReturnConnection(connect);
        return false;
    }

    freeReplyObject(m_reply);
    m_pool->ReturnConnection(connect);
    return true;
}

bool RedisManager::HDel(const std::string& key, const std::string& hkey) {
    auto connect = m_pool->GetConnection();
    if (connect == nullptr) return false;
    auto* m_reply = static_cast<redisReply*>(redisCommand(connect, "HDEL %s %s", key.c_str(), hkey.c_str()));
    if (m_reply == nullptr) {
        std::cerr << "RedisManager::HDEL [ HDEL " << key << hkey << " ] error ! " << std::endl;
        freeReplyObject(m_reply);
        m_pool->ReturnConnection(connect);
        return false;
    }

    bool success = false;
    if(m_reply->type == REDIS_REPLY_INTEGER)
        success = m_reply->integer > 0;

    freeReplyObject(m_reply);
    m_pool->ReturnConnection(connect);
    return success;
}

bool RedisManager::ExistsKey(const std::string& key) {
    auto connect = m_pool->GetConnection();
    if (connect == nullptr) return false;
    auto* m_reply = static_cast<redisReply*>(redisCommand(connect, "exists %s", key.c_str()));
    if (m_reply == nullptr || m_reply->type != REDIS_REPLY_INTEGER || m_reply->integer == 0) {
        std::cerr << "RedisManager::ExistsKey Not Found [ Key " << key << " ]" << std::endl;
        freeReplyObject(m_reply);
        m_pool->ReturnConnection(connect);
        return false;
    }
    freeReplyObject(m_reply);
    m_pool->ReturnConnection(connect);
    return true;
}

void RedisManager::Close() {
    m_pool->Close();
}



