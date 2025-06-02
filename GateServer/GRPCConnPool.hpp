#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <atomic>
#include <string>
#include <sstream>
#include <grpcpp/grpcpp.h>
#include <functional>

using grpc::Channel;

template <typename StubType>
class GRPCConnPool {
public:
    using StubFactory = std::function<std::unique_ptr<StubType>(std::shared_ptr<grpc::Channel>)>;

    GRPCConnPool(const GRPCConnPool&) = delete;
    GRPCConnPool& operator=(const GRPCConnPool&) = delete;

    ~GRPCConnPool();
    GRPCConnPool(std::size_t size, std::string host, std::string port, StubFactory factory);
    // 获取连接
    std::unique_ptr<StubType> GetConnection();
    // 回收连接
    void RecycleConnection(std::unique_ptr<StubType> conn);
    // 关闭连接池
    void Close();

private:
    std::atomic<bool> m_stop;
    std::size_t m_pool_size;
    std::string m_host;
    std::string m_port;
    std::queue<std::unique_ptr<StubType>> m_connections;
    std::condition_variable m_cond;
    std::mutex m_mutex;
};

template <typename StubType>
GRPCConnPool<StubType>::~GRPCConnPool() {
    std::unique_lock<std::mutex> lock(m_mutex);
    // 通知线程gRPC服务关闭
    Close();
    // 释放资源 RAII
    while (!m_connections.empty()) {
        m_connections.pop();
    }
    std::cout << "GRPCConnPool::GRPCConnPool destructed" << std::endl;
}
template <typename StubType>
GRPCConnPool<StubType>::GRPCConnPool(const std::size_t size, std::string host, std::string port, StubFactory factory)
    : m_stop(false), m_pool_size(size), m_host(std::move(host)), m_port(std::move(port)) {
    for (std::size_t i = 0; i < m_pool_size; ++i) {
        // 创建通道
        std::stringstream ipSStrm;
        ipSStrm<<m_host<<":"<<m_port;
        const std::shared_ptr<Channel> channel = grpc::CreateChannel(ipSStrm.str(), grpc::InsecureChannelCredentials());
        // 将创建的通道放入队列中
        // 注意stub绑定的是验证码服务VerifyService
        m_connections.push(factory(channel));
    }
    std::cout << "GRPCConnPool::GRPCConnPool constructed" << std::endl;
}
template <typename StubType>
std::unique_ptr<StubType> GRPCConnPool<StubType>::GetConnection() {
    std::unique_lock<std::mutex> lock(m_mutex);
    // 当条件为true 或 收到通知时停止等待, 判断条件时会获取到锁
    m_cond.wait(lock, [this] { return m_stop || !m_connections.empty(); });
    // 1.m_stop == true, !m_connections.empty() == true
    // 2.m_stop == true, !m_connections.empty() == false
    if(m_stop) return nullptr;
    // 3.m_stop == false, !m_connections.empty() == true
    auto conn = std::move(m_connections.front());
    m_connections.pop();
    return conn;
}

template <typename StubType>
void GRPCConnPool<StubType>::RecycleConnection(std::unique_ptr<StubType> conn) {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_stop) return;

    m_connections.push(std::move(conn));
    m_cond.notify_one();
}

template <typename StubType>
void GRPCConnPool<StubType>::Close() {
    m_stop = true;
    m_cond.notify_all();
}
