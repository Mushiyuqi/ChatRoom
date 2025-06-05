#pragma once
#include "const.h"

class StatusGrpcClient {
public:
    ~StatusGrpcClient();
    StatusGrpcClient(const StatusGrpcClient&) = delete;
    StatusGrpcClient& operator=(const StatusGrpcClient&) = delete;
    // 获取单例对象
    static StatusGrpcClient& GetInstance();

    GetChatServerRsp GetChatServer(int uid);
    LoginRsp Login(int uid, std::string token);

private:
    StatusGrpcClient();

    GRPCConnPool<StatusService::Stub> m_connPool;
};

