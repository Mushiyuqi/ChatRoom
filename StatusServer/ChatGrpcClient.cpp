#include "ChatGrpcClient.h"

ChatGrpcClient::ChatGrpcClient()
{
    auto& cfg = ConfigManager::GetInstance();
    auto server_list = cfg["ChatServers"]["Name"];
    std::vector<std::string> words;
    std::stringstream ss(server_list);
    std::string word;

    while (std::getline(ss, word, ',')) {
        words.push_back(word);
    }

    // todo !!!!! cfg[tmp]["Port"]为TCP端口
    for (auto& tmp : words) {
        if (cfg[tmp]["Name"].empty()) {continue;}
        m_pools[cfg[tmp]["Name"]] = std::make_unique<GRPCConnPool<ChatService::Stub>>(
            GRPCPoolSize,
            cfg[tmp]["Host"],
            cfg[tmp]["Port"],
            [](auto channel){return ChatService::NewStub(channel);});
    }
}

ChatGrpcClient& ChatGrpcClient::GetInstance() {
    static ChatGrpcClient instance;
    return instance;
}

AddFriendRsp ChatGrpcClient::NotifyAddFriend(const AddFriendReq& req)
{
    AddFriendRsp rsp;
    return rsp;
}

