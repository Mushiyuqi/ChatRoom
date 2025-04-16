#include "IOContextPool.h"
#include <iostream>

IOContextPool::IOContextPool(std::size_t size): m_io_contexts(size), m_works(size), m_next_io_context(0) {
    // 为每一个iocontext绑定一个work
    for (std::size_t i = 0; i < size; ++i) {
        m_works[i] = std::make_unique<Work>(make_work_guard(m_io_contexts[i]));
    }
    // 将每一个iocontext运行在一个单独的线程内
    for (std::size_t i = 0; i < size; ++i) {
        m_threads.emplace_back([this, i] {
            m_io_contexts[i].run();
        });
    }
}

IOContextPool::~IOContextPool() {
    // 停止所有资源 符合RAII
    Stop();
    std::cout << "IOContextPool::~IOContextPool destructed" << std::endl;
}

IOContextPool& IOContextPool::GetInstance() {
    static IOContextPool instance{};
    return instance;
}

boost::asio::io_context& IOContextPool::GetIOContext() {
    boost::asio::io_context& io_context = m_io_contexts[m_next_io_context++];
    if (m_next_io_context == m_io_contexts.size()) {
        m_next_io_context = 0;
    }
    return io_context;
}

void IOContextPool::Stop() {
    // 停止每一个iocontext
    for (auto& io_context : m_io_contexts) {
        io_context.stop();
    }
    // 停止每一个work
    for(auto& work : m_works) {
        work.reset();
    }
    // 等待每一个线程结束
    for (auto& thread : m_threads) {
        thread.join();
    }
}
