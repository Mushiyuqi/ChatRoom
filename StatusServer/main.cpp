#include "const.h"
#include "ConfigManager.h"
#include "StatusServiceImpl.h"


void RunServer() {
    auto& config = ConfigManager::GetInstance();

    // 监听端口
    grpc::ServerBuilder builder;
    const std::string serverAddress(config["StatusServer"]["Host"] + ":" + config["StatusServer"]["Port"]);
    builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
    // 注册服务
    StatusServiceImpl service;
    builder.RegisterService(&service);
    // 构建并启动gRPC服务器
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << serverAddress << std::endl;

    // 创建Boost.Asio的io_context
    boost::asio::io_context io_context;
    // 创建signal_set用于捕获SIGINT
    boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
    // 设置异步等待SIGINT信号
    signals.async_wait([&server, &io_context](const boost::system::error_code& error, int signal_number) {
        if (!error) {
            std::cout << "Shutting down server..." << std::endl;
            server->Shutdown(); // 优雅地关闭服务器
            io_context.stop(); // 停止io_context
        }
    });

    // 在单独的线程中运行io_context
    std::thread([&io_context]() { io_context.run(); }).detach();

    // 等待服务器关闭
    server->Wait();
}

int main(int argc, char** argv) {
    try {
        RunServer();
    }
    catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}
