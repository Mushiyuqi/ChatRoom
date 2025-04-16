#include <iostream>
#include "CServer.h"
#include "ConfigManager.h"
#include "VerifyGrpcClient.h"

int main() {
    // 初始化获取验证码服务客户端
    VerifyGrpcClient::GetInstance();
    // 读取配置文件
    const std::string gatePortStr = ConfigManager::GetInstance()["GateServer"]["Port"];
    try {
        // 端口号
        unsigned short port = std::stoi(gatePortStr);
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
