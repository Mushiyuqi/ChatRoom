#pragma once
#include "const.h"

class RedisManager {
public:
    ~RedisManager();
    RedisManager& operator=(const RedisManager&) = delete;
    RedisManager(const RedisManager&) = delete;

    static RedisManager& getInstance();

private:
    RedisManager();
};

