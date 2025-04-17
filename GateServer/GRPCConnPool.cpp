#include "GRPCConnPool.h"

GRPCConnPool::~GRPCConnPool() {
    std::unique_lock<std::mutex> lock(m_mutex);
    // 通知线程gRPC服务关闭
    Close();
    // 释放资源 RAII
    while (!m_connections.empty()) {
        m_connections.pop();
    }
    std::cout << "GRPCConnPool::GRPCConnPool destructed" << std::endl;
}

GRPCConnPool::GRPCConnPool(const std::size_t size, std::string host, std::string port)
    : m_stop(false), m_pool_size(size), m_host(std::move(host)), m_port(std::move(port)) {
    for (std::size_t i = 0; i < m_pool_size; ++i) {
        // 创建通道
        std::stringstream ipSStrm;
        ipSStrm<<m_host<<":"<<m_port;
        const std::shared_ptr<Channel> channel = grpc::CreateChannel(ipSStrm.str(), grpc::InsecureChannelCredentials());
        // 将创建的通道放入队列中
        // 注意stub绑定的是验证码服务VerifyService
        m_connections.push(VerifyService::NewStub(channel));
    }
    std::cout << "GRPCConnPool::GRPCConnPool constructed" << std::endl;
}

std::unique_ptr<VerifyService::Stub> GRPCConnPool::GetConnection() {
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

void GRPCConnPool::RecycleConnection(std::unique_ptr<VerifyService::Stub> conn) {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_stop) return;

    m_connections.push(std::move(conn));
    m_cond.notify_one();
}

void GRPCConnPool::Close() {
    m_stop = true;
    m_cond.notify_all();
}
