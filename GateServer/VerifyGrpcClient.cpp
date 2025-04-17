#include "VerifyGrpcClient.h"
#include "ConfigManager.h"

VerifyGrpcClient::~VerifyGrpcClient() {
    std::cout << "VerifyGrpcClient::~VerifyGrpcClient() destructed" << std::endl;
}

VerifyGrpcClient::VerifyGrpcClient()
    : m_connPool(6, ConfigManager::GetInstance()["VerifyServer"]["Host"],
                 ConfigManager::GetInstance()["VerifyServer"]["Port"]) {
    std::cout << "VerifyGrpcClient::VerifyGrpcClient() constructed" << std::endl;
}

VerifyGrpcClient& VerifyGrpcClient::GetInstance() {
    static VerifyGrpcClient instance;
    return instance;
}

GetVerifyRsp VerifyGrpcClient::GetVerifyCode(std::string email) {
    ClientContext context;
    GetVerifyRsp response;
    GetVerifyReq request;
    // 编辑发送请求
    request.set_email(email);
    // 获取连接
    auto stub = m_connPool.GetConnection();
    // 发送请求
    Status status = stub->GetVerifyCode(&context, request, &response);
    // 回收连接
    m_connPool.RecycleConnection(std::move(stub));
    // 处理返回
    if (status.ok())
        return response;
    else {
        response.set_error(ErrorCodes::RPCFailed);
        return response;
    }
}


