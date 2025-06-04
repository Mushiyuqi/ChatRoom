#pragma once
#include <boost/asio.hpp>
#include <vector>

class AsioIOContextPool {
public:
    using IOContext = boost::asio::io_context;
    using Work = boost::asio::executor_work_guard<IOContext::executor_type>;
    using WorkPtr = std::unique_ptr<Work>;
    ~AsioIOContextPool();
    AsioIOContextPool(const AsioIOContextPool&) = delete;
    AsioIOContextPool& operator=(const AsioIOContextPool&) = delete;
    boost::asio::io_context& GetIOContext();
    void Stop();
    static AsioIOContextPool& GetInstance();

private:
    explicit AsioIOContextPool(std::size_t size = std::thread::hardware_concurrency());
    std::vector<IOContext> m_io_contexts;
    std::vector<WorkPtr> m_works;
    std::vector<std::thread> m_threads;
    std::size_t m_next_io_context{};

};

