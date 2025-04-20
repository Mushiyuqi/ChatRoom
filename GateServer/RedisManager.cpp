#include "RedisManager.h"

RedisManager::~RedisManager() {
    std::cout << "RedisManager::~RedisManager destructed" << std::endl;
}

RedisManager::RedisManager() {
    std::cout << "RedisManager::RedisManager constructed" << std::endl;
}

RedisManager& RedisManager::getInstance() {
    static RedisManager instance;
    return instance;
}


