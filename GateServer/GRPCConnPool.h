#pragma once
#include "const.h"

class GRPCConnPool {
public:
    GRPCConnPool(const GRPCConnPool&) = delete;
    GRPCConnPool& operator=(const GRPCConnPool&) = delete;

    ~GRPCConnPool();
    GRPCConnPool(std::size_t size, std::string host, std::string port);
    // 获取连接
    std::unique_ptr<VerifyService::Stub> GetConnection();
    // 回收连接
    void RecycleConnection(std::unique_ptr<VerifyService::Stub> conn);
    // 关闭连接池
    void Close();

private:
    std::atomic<bool> m_stop;
    std::size_t m_pool_size;
    std::string m_host;
    std::string m_port;
    std::queue<std::unique_ptr<VerifyService::Stub>> m_connections;
    std::condition_variable m_cond;
    std::mutex m_mutex;
};

