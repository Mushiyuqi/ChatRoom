#include "StatusServiceImpl.h"

#include <utility>
#include "ConfigManager.h"

std::string generate_unique_string() {
    // 创建UUID对象
    const boost::uuids::uuid uuid = boost::uuids::random_generator()();
    // 将UUID转换为字符串
    std::string unique_string = to_string(uuid);
    return unique_string;
}

StatusServiceImpl::StatusServiceImpl() {
    auto& config = ConfigManager::GetInstance();
    ChatServer server;
    server.port = config["ChatServer1"]["Port"];
    server.host = config["ChatServer1"]["Host"];
    server.name = config["ChatServer1"]["Name"];
    server.con_count = 0;
    m_servers[server.name] = server;

    server.port = config["ChatServer2"]["Port"];
    server.host = config["ChatServer2"]["Host"];
    server.name = config["ChatServer2"]["Name"];
    server.con_count = 0;
    m_servers[server.name] = server;
}

Status StatusServiceImpl::GetChatServer(ServerContext* context, const GetChatServerReq* request,
    GetChatServerRsp* reply) {
    // 获取聊天服务器
    const auto& server = GetChatServer();
    // 编辑回复信息
    reply->set_host(server.host);
    reply->set_port(server.port);
    reply->set_error(ErrorCodes::Success);
    reply->set_token(generate_unique_string());
    // 记录Token
    InsertToken(request->uid(), reply->token());
    return Status::OK;
}

Status StatusServiceImpl::Login(ServerContext* context, const LoginReq* request, LoginRsp* reply) {
    auto uid = request->uid();
    auto token = request->token();
    std::lock_guard<std::mutex> guard(m_token_mtx);
    auto iter = m_tokens.find(uid);
    if (iter == m_tokens.end()) {
        reply->set_error(ErrorCodes::UidInvalid);
        return Status::OK;
    }
    if (iter->second != token) {
        reply->set_error(ErrorCodes::TokenInvalid);
        return Status::OK;
    }
    reply->set_error(ErrorCodes::Success);
    reply->set_uid(uid);
    reply->set_token(token);
    //todo m_tokens内是否需要删除已经使用过的token
    return Status::OK;
}

void StatusServiceImpl::InsertToken(const int uid, std::string token) {
    std::lock_guard<std::mutex> guard(m_token_mtx);
    m_tokens[uid] = std::move(token);
}

ChatServer StatusServiceImpl::GetChatServer() {
    std::lock_guard<std::mutex> guard(m_server_mtx);
    auto minServer = m_servers.begin()->second;
    // 使用范围基于for循环
    for (const auto& [fst, snd] : m_servers) {
        if (snd.con_count < minServer.con_count) {
            minServer = snd;
        }
    }
    // 返回连接数最小的服务器
    return minServer;
}
