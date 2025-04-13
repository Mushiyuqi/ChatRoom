#include "VerifyGrpcClient.h"

VerifyGrpcClient::~VerifyGrpcClient() {
    std::cout << "VerifyGrpcClient::~VerifyGrpcClient() destructed" << std::endl;
}

VerifyGrpcClient::VerifyGrpcClient() {
    // 创建通道
    const std::shared_ptr<Channel> channel = grpc::CreateChannel("0.0.0.0:50051", grpc::InsecureChannelCredentials());
    // sub绑定channel
    m_stub = VerifyService::NewStub(channel);
    std::cout << "VerifyGrpcClient::VerifyGrpcClient() constructed" << std::endl;
}

VerifyGrpcClient& VerifyGrpcClient::GetInstance() {
    static VerifyGrpcClient instance;
    return instance;
}

GetVerifyRsp VerifyGrpcClient::GetVerifyCode(std::string email) const {
    ClientContext context;
    GetVerifyRsp reply;
    GetVerifyReq request;
    request.set_email(email);
    // 发送请求
    Status status = m_stub->GetVerifyCode(&context, request, &reply);

    if (status.ok())
        return reply;
    else {
        reply.set_error(ErrorCodes::RPCFailed);
        return reply;
    }
}


