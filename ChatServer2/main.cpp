#include "const.h"
#include "CServer.h"
#include "ConfigManager.h"
#include "RedisManager.h"
#include "ChatServiceImpl.h"

int main() {
    try {
        // 将登陆数量设置为0
        auto serverName = ConfigManager::GetInstance()["SelfServer"]["Name"];
        RedisManager::GetInstance().HSet(LOGIN_COUNT, serverName, "0");

        // 启动Grpc服务器
        auto Host = ConfigManager::GetInstance()["SelfServer"]["Host"];
        auto Port = ConfigManager::GetInstance()["SelfServer"]["RpcPort"];
        std::string serverAddress(Host + ":" + Port);
        ChatServiceImpl service{};
        grpc::ServerBuilder builder;
        // 添加监听端口和服务
        builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
        builder.RegisterService(&service);
        std::unique_ptr<grpc::Server> serverRpc(builder.BuildAndStart());
        // 单独启动一个线程处理Rpc服务
        std::thread rpcThread([&serverRpc]() {
            serverRpc->Wait();  // 等待服务器关闭
        });

        // 启动Tcp服务器
        boost::asio::io_context io_context{};
        auto portStr = ConfigManager::GetInstance()["SelfServer"]["Port"];
        CServer serverTcp{io_context, static_cast<short>(std::stoi(portStr))};

        // 用于监听退出请求
        boost::asio::signal_set signals{io_context, SIGINT, SIGTERM};
        signals.async_wait([&io_context, &serverRpc](auto, auto) {
            io_context.stop();
            serverRpc->Shutdown();
        });
        io_context.run();

        RedisManager::GetInstance().HDel(LOGIN_COUNT, serverName);
        rpcThread.join();
    }
    catch (std::exception& e) {
        std::cerr << "Exception : " << e.what() << "\n";
    }
    return 0;
}
