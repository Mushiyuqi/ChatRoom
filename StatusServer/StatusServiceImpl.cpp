#include "StatusServiceImpl.h"
#include <utility>
#include "ConfigManager.h"
#include "RedisManager.h"

std::string generate_unique_string() {
    // 创建UUID对象
    const boost::uuids::uuid uuid = boost::uuids::random_generator()();
    // 将UUID转换为字符串
    std::string unique_string = to_string(uuid);
    return unique_string;
}

StatusServiceImpl::StatusServiceImpl() {
    // 获取ChatServer列表
    auto& config = ConfigManager::GetInstance();
    std::stringstream serverList(config["ChatServers"]["Name"]);
    std::vector<std::string> servers;
    std::string tmp;
    while (std::getline(serverList, tmp, ',')) {servers.push_back(tmp);}

    // 注册ChatServer
    for (auto& word : servers) {
        if(config[word]["Name"].empty()) continue;

        ChatServer server;
        server.port = config[word]["Port"];
        server.host = config[word]["Host"];
        server.name = config[word]["Name"];
        server.con_count = 0;
        m_servers[server.name] = server;
    }
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
    std::cout<< "StatusServiceImpl GetChatServer uid:" << request->uid() << " token:" << reply->token() << std::endl;
    std::cout<< "StatusServiceImpl GetChatServer host:" << server.host << " port:" << server.port << std::endl;

    return Status::OK;
}

Status StatusServiceImpl::Login(ServerContext* context, const LoginReq* request, LoginRsp* reply) {
    auto uid = request->uid();
    auto token = request->token();

    std::string uidStr = std::to_string(uid);
    std::string tokenKey = USERTOKENPREFIX + uidStr;
    std::string tokenValue = "";
    bool success = RedisManager::GetInstance().Get(tokenKey, tokenValue);
    if (success) {
        reply->set_error(ErrorCodes::UidInvalid);
        return Status::OK;
    }

    if (tokenValue != token) {
        reply->set_error(ErrorCodes::TokenInvalid);
        return Status::OK;
    }

    reply->set_error(ErrorCodes::Success);
    reply->set_uid(uid);
    reply->set_token(token);
    //todo 是否需要删除已经使用过的token
    return Status::OK;
}

void StatusServiceImpl::InsertToken(const int uid, const std::string& token) {
    std::string uid_str = std::to_string(uid);
    std::string token_key = USERTOKENPREFIX + uid_str;
    RedisManager::GetInstance().Set(token_key, token);
}

ChatServer StatusServiceImpl::GetChatServer() {
    std::lock_guard<std::mutex> guard(m_server_mtx);
    auto minServer = m_servers.begin()->second;
    std::string countStr = "";
    RedisManager::GetInstance().HGet(LOGIN_COUNT, minServer.name, countStr);
    //不存在则默认设置为最大
    if (countStr.empty())
        minServer.con_count = INT_MAX;
    else
        minServer.con_count = std::stoi(countStr);


    // 使用范围基于for循环
    for ( auto& server : m_servers) {
        if (server.second.name == minServer.name) continue;

        countStr = "";
        RedisManager::GetInstance().HGet(LOGIN_COUNT, server.second.name, countStr);

        //不存在则默认设置为最大
        if (countStr.empty())
            server.second.con_count = INT_MAX;
        else
            server.second.con_count = std::stoi(countStr);

        if (server.second.con_count < minServer.con_count)
            minServer = server.second;
    }

    return minServer;
}
