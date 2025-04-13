#pragma once
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "message.pb.h"
#include "const.h"

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;
using message::GetVerifyReq;
using message::GetVerifyRsp;
using message::VerifyService;
class VerifyGrpcClient {
public:
    ~VerifyGrpcClient();
    VerifyGrpcClient(const VerifyGrpcClient&) = delete;
    VerifyGrpcClient& operator=(const VerifyGrpcClient&) = delete;
    // 获取单例对象
    static VerifyGrpcClient& GetInstance();

    GetVerifyRsp GetVerifyCode(std::string email) const;

private:
    VerifyGrpcClient();
    // rpc信使
    std::unique_ptr<VerifyService::Stub> m_stub;
};

