#pragma once
#include "const.h"
#include "ConfigManager.h"
#include "message.grpc.pb.h"
#include "message.pb.h"
#include "data.h"
#include <grpcpp/grpcpp.h>
#include <json/json.h>
#include "GRPCConnPool.hpp"
#include <unordered_map>

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::AddFriendReq;
using message::AddFriendRsp;

using message::AuthFriendReq;
using message::AuthFriendRsp;

using message::GetChatServerRsp;
using message::LoginRsp;
using message::LoginReq;
using message::ChatService;

using message::TextChatMsgReq;
using message::TextChatMsgRsp;
using message::TextChatData;

class ChatGrpcClient {
public:
    // 禁止拷贝构造和赋值
    ChatGrpcClient(const ChatGrpcClient&) = delete;
    ChatGrpcClient& operator=(const ChatGrpcClient&) = delete;

    ~ChatGrpcClient() = default;
    static ChatGrpcClient& GetInstance();

    AddFriendRsp NotifyAddFriend(std::string server_ip, const AddFriendReq& req);
    AuthFriendRsp NotifyAuthFriend(std::string server_ip, const AuthFriendReq& req);
    bool GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo);
    TextChatMsgRsp NotifyTextChatMsg(std::string server_ip, const TextChatMsgReq& req, const Json::Value& rtvalue);

private:
    ChatGrpcClient();
    std::unordered_map<std::string, std::unique_ptr<GRPCConnPool<ChatService::Stub>>> m_pools;
};


