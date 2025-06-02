#pragma once
#include "const.h"

struct ChatServer {
    std::string host;
    std::string port;
    std::string name;
    int con_count;
};

class StatusServiceImpl final : public StatusService::Service {
public:
    StatusServiceImpl();
    Status GetChatServer(ServerContext* context, const GetChatServerReq* request, GetChatServerRsp* reply) override;
    Status Login(ServerContext* context, const LoginReq* request, LoginRsp* reply) override;

private:
    void InsertToken(int uid, std::string token);
    ChatServer GetChatServer();

    std::unordered_map<std::string, ChatServer> m_servers;
    std::mutex m_server_mtx;
    std::unordered_map<int, std::string> m_tokens;
    std::mutex m_token_mtx;
};
