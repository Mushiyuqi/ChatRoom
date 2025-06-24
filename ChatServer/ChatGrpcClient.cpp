#include "ChatGrpcClient.h"
#include "RedisManager.h"
#include "ConfigManager.h"
#include "CSession.h"
#include "MysqlManager.h"
#include "UserManager.h"

ChatGrpcClient& ChatGrpcClient::getInstance() {
    static ChatGrpcClient instance;
    return instance;
}

AddFriendRsp ChatGrpcClient::NotifyAddFriend(std::string server_ip, const AddFriendReq& req) {
    AddFriendRsp rsp;
    return rsp;
}

AuthFriendRsp ChatGrpcClient::NotifyAuthFriend(std::string server_ip, const AuthFriendReq& req) {
    AuthFriendRsp rsp;
    return rsp;
}

bool ChatGrpcClient::GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo) {
    return true;
}

TextChatMsgRsp ChatGrpcClient::NotifyTextChatMsg(std::string server_ip, const TextChatMsgReq& req,
    const Json::Value& rtvalue) {
    TextChatMsgRsp rsp;
    return rsp;
}

ChatGrpcClient::ChatGrpcClient() {
    // 获取服务器列表
    auto serverList = ConfigManager::GetInstance()["PeerServer"]["Servers"];
    std::vector<std::string> words;
    std::stringstream ss(serverList);
    std::string word;
    while (std::getline(ss, word, ',')) {words.push_back(word);}

    for(auto& tmp : words) {
        std::string serverName = ConfigManager::GetInstance()[tmp]["Name"];
        if(serverName.empty()) continue;

        m_pools[serverName] = std::make_unique<GRPCConnPool<ChatService::Stub>>(
            GRPCPoolSize,
            ConfigManager::GetInstance()[tmp]["Host"],
            ConfigManager::GetInstance()[tmp]["Port"],
            [](auto channel) {return ChatService::NewStub(channel);});
    }
}


