#pragma once
#include "const.h"

class IOContextPool {
public:
    using IOContext = boost::asio::io_context;
    using Work = boost::asio::executor_work_guard<IOContext::executor_type>;
    using WorkPtr = std::unique_ptr<Work>;

    ~IOContextPool();
    IOContextPool(const IOContextPool&) = delete;
    IOContextPool& operator=(const IOContextPool&) = delete;

    static IOContextPool& GetInstance();
    boost::asio::io_context& GetIOContext();
private:
    void Stop();

    explicit IOContextPool(std::size_t size = std::thread::hardware_concurrency());
    std::vector<IOContext> m_io_contexts;
    std::vector<WorkPtr> m_works;
    std::vector<std::thread> m_threads;
    std::size_t m_next_io_context{};

};

