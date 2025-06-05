#include "AsioIOContextPool.h"
#include <iostream>

AsioIOContextPool::AsioIOContextPool(std::size_t size): m_io_contexts(size), m_works(size), m_next_io_context(0) {
    for (std::size_t i = 0; i < size; ++i) {
        m_works[i] = std::make_unique<Work>(make_work_guard(m_io_contexts[i]));
    }

    for (std::size_t i = 0; i < size; ++i) {
        m_threads.emplace_back([this, i] {
            m_io_contexts[i].run();
        });
    }
}

AsioIOContextPool::~AsioIOContextPool() {
    Stop();
    std::cerr << "AsioIOContextPool::~AsioIOContextPool()" << std::endl;
}

AsioIOContextPool& AsioIOContextPool::GetInstance() {
    /**
     * C++11以后静态变量的初始化在多线程下是线程安全的
     * 编译器会确保静态局部变量在多线程环境下只被初始化一次
     * 多个线程同时调用 getInstance() 时，只会有一个线程执行初始化，其他线程会等待初始化完成
     */

    static AsioIOContextPool instance{};
    return instance;
}

boost::asio::io_context& AsioIOContextPool::GetIOContext() {
    boost::asio::io_context& io_context = m_io_contexts[m_next_io_context++];
    if (m_next_io_context == m_io_contexts.size()) {
        m_next_io_context = 0;
    }
    return io_context;
}

void AsioIOContextPool::Stop() {
    for(auto& work : m_works) {
        work.reset();
    }

    for (auto& io_context : m_io_contexts) {
        io_context.stop();
    }

    for (auto& thread : m_threads) {
        thread.join();
    }
}
