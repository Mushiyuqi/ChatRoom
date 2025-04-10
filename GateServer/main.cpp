#include <iostream>
#include "CServer.h"

int main() {
    try {
        // 端口号
        unsigned short port = static_cast<unsigned short>(10086);
        net::io_context ioc{};
        // 检测程序关闭信号
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](auto ec, int signal_number) {
            if (ec) {
                return;
            }
            // 关闭io_context
            ioc.stop();
        });
        // 启动Server
        std::make_shared<CServer>(ioc, port)->Start();
        std::cout << "Gate Server listen on port: " << port << std::endl;
        // 启动io事件循环
        ioc.run();
    }
    catch (std::exception& e) {
        std::cerr << "Main Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}
