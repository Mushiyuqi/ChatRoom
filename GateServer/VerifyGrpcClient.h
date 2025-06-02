#pragma once
#include "const.h"

class VerifyGrpcClient {
public:
    ~VerifyGrpcClient();
    VerifyGrpcClient(const VerifyGrpcClient&) = delete;
    VerifyGrpcClient& operator=(const VerifyGrpcClient&) = delete;
    // 获取单例对象
    static VerifyGrpcClient& GetInstance();

    GetVerifyRsp GetVerifyCode(std::string email);

private:
    VerifyGrpcClient();

    GRPCConnPool<VerifyService::Stub> m_connPool;
};

