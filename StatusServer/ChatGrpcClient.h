#pragma once
#include "ConfigManager.h"
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "message.pb.h"
#include "GRPCConnPool.hpp"

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::AddFriendReq;
using message::AddFriendRsp;

using message::GetChatServerRsp;
using message::LoginRsp;
using message::LoginReq;
using message::ChatService;

class ChatGrpcClient {
public:
    // 禁止拷贝构造和赋值
    ChatGrpcClient(const ChatGrpcClient&) = delete;
    ChatGrpcClient& operator=(const ChatGrpcClient&) = delete;

    ~ChatGrpcClient() = default;
    static ChatGrpcClient& GetInstance();

    AddFriendRsp NotifyAddFriend(const AddFriendReq& req);

private:
    ChatGrpcClient();
    std::unordered_map<std::string, std::unique_ptr<GRPCConnPool<ChatService::Stub>>> m_pools;
};
