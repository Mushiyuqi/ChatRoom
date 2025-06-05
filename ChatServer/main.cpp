#include "const.h"
#include "CServer.h"
#include "ConfigManager.h"

int main() {
    try {
        // 进程关闭自动关闭AsioIOContextPool

        // 用于监听连接请求和singal的io_context
        boost::asio::io_context io_context{};
        boost::asio::signal_set signals{io_context, SIGINT, SIGTERM};
        signals.async_wait([&io_context](auto, auto) {
            io_context.stop();
        });

        auto portStr = ConfigManager::GetInstance()["SelfServer"]["Port"];
        CServer server{io_context, static_cast<short>(atoi(portStr.c_str()))};
        io_context.run();
    }
    catch (std::exception& e) {
        std::cerr << "Exception : " << e.what() << "\n";
    }
    return 0;
}
