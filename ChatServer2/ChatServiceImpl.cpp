#include "ChatServiceImpl.h"
#include "UserManager.h"
#include "CSession.h"
#include <json/json.h>
#include "RedisManager.h"
#include "MysqlManager.h"

ChatServiceImpl::ChatServiceImpl() {

}

Status ChatServiceImpl::NotifyAddFriend(ServerContext* context, const AddFriendReq* request, AddFriendRsp* reply) {
    auto touid = request->touid();
    auto session = UserManager::GetInstance().GetSession(touid);

    Defer defer([request, reply]() {
        reply->set_error(ErrorCodes::Success);
        reply->set_applyuid(request->applyuid());
        reply->set_touid(request->touid());
    });

    // 用户不在内存中直接返回 todo
    if(session == nullptr) {
        return Status::OK;
    }

    // 在内存中则直接发送通知对方
    Json::Value returnValue;
    returnValue["error"] = ErrorCodes::Success;;
    returnValue["applyuid"] = request->applyuid();
    returnValue["name"] = request->name();
    returnValue["desc"] = request->desc();
    returnValue["icon"] = request->icon();
    returnValue["sex"] = request->sex();
    returnValue["nick"] = request->nick();

    session->Send(returnValue.toStyledString(), ReqId::ID_NOTIFY_ADD_FRIEND);

    return Status::OK;
}

Status ChatServiceImpl::
NotifyAuthFriend(ServerContext* context, const AuthFriendReq* request, AuthFriendRsp* response) {
    return Status::OK;
}

Status ChatServiceImpl::NotifyTextChatMsg(grpc::ServerContext* context, const TextChatMsgReq* request,
    TextChatMsgRsp* response) {
    return Status::OK;
}

bool ChatServiceImpl::GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo) {
    return true;
}
