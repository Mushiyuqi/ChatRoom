#include <iostream>
#include <boost/asio.hpp>
#include <csignal>
#include "CServer.h"
#include "AsioIOContextPool.h"
#include "LogicSystem.h"

using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
using boost::asio::as_tuple;
namespace this_coro = boost::asio::this_coro;


int main() {
    try {
        // 初始化LogicSystem
        LogicSystem::GetInstance();
        // 初始化IOContextPool
        AsioIOContextPool::GetInstance();

        // 用于监听连接请求和singal的io_context
        boost::asio::io_context io_context{};
        boost::asio::signal_set signals{io_context, SIGINT, SIGTERM};
        signals.async_wait([&io_context](auto, auto) {
            io_context.stop();
        });

        CServer server{io_context, 8091};
        io_context.run();
    }
    catch (std::exception& e) {
        std::cerr << "Exception : " << e.what() << "\n";
    }
    return 0;
}
