#include "StatusGrpcClient.h"
#include "ConfigManager.h"

StatusGrpcClient::~StatusGrpcClient() {
     std::cout << "StatusGrpcClient::~StatusGrpcClient() destructed" << std::endl;
}

StatusGrpcClient& StatusGrpcClient::GetInstance() {
     static StatusGrpcClient instance;
     return instance;
}

GetChatServerRsp StatusGrpcClient::GetChatServer(const int uid) {
     ClientContext context;
     GetChatServerRsp response;
     GetChatServerReq request;
     // 编辑发送请求
     request.set_uid(uid);
     // 获取连接
     auto stub = m_connPool.GetConnection();
     // 发送请求
     Status status = stub->GetChatServer(&context, request, &response);
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

LoginRsp StatusGrpcClient::Login(int uid, std::string token) {
     ClientContext context;
     LoginRsp response;
     LoginReq request;
     // 编辑发送请求
     request.set_uid(uid);
     request.set_token(token);
     // 获取连接
     auto stub = m_connPool.GetConnection();
     // 发送请求
     Status status = stub->Login(&context, request, &response);
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

StatusGrpcClient::StatusGrpcClient(): m_connPool(GRPCPoolSize, ConfigManager::GetInstance()["StatusServer"]["Host"],
                                                 ConfigManager::GetInstance()["StatusServer"]["Port"],
                                                 [](auto channel) {
                                                      return StatusService::NewStub(channel);
                                                 }) {
     std::cout << "StatusGrpcClient::StatusGrpcClient() constructed" << std::endl;
}
