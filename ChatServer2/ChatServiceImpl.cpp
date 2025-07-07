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
    // 查找用户是否在本服务器
    auto touid = request->touid();
    auto fromuid = request->fromuid();
    auto session = UserManager::GetInstance().GetSession(touid);

    Defer defer([request, response]() {
        response->set_error(ErrorCodes::Success);
        response->set_fromuid(request->fromuid());
        response->set_touid(request->touid());
    });

    // 用户不在内存中直接返回
    if(session == nullptr) return Status::OK;

    // 在内存中则直接发送通知对方
    Json::Value returnValue;
    returnValue["error"] = ErrorCodes::Success;;
    returnValue["fromuid"] = request->fromuid();
    returnValue["touid"] = request->touid();

    std::string baseKey = USER_BASE_INFO + std::to_string(fromuid);
    auto userInfo = std::make_shared<UserInfo>();
    bool flag = GetBaseInfo(baseKey, fromuid, userInfo);
    if(flag) {
        returnValue["name"] = userInfo->name;
        returnValue["icon"] = userInfo->icon;
        returnValue["sex"] = userInfo->sex;
        returnValue["nick"] = userInfo->nick;
    }else {
        returnValue["error"] = ErrorCodes::UidInvalid;
    }

    session->Send(returnValue.toStyledString(), ID_NOTIFY_AUTH_FRIEND);
    return Status::OK;
}

Status ChatServiceImpl::NotifyTextChatMsg(grpc::ServerContext* context, const TextChatMsgReq* request,
    TextChatMsgRsp* response) {
    return Status::OK;
}

bool ChatServiceImpl::GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo) {
    //优先查redis中查询用户信息
    std::string info_str;
    if (RedisManager::GetInstance().Get(base_key, info_str)) {
        Json::Reader reader;
        Json::Value root;
        reader.parse(info_str, root);
        userinfo->uid = root["uid"].asInt();
        userinfo->name = root["name"].asString();
        userinfo->pwd = root["pwd"].asString();
        userinfo->email = root["email"].asString();
        userinfo->nick = root["nick"].asString();
        userinfo->desc = root["desc"].asString();
        userinfo->sex = root["sex"].asInt();
        userinfo->icon = root["icon"].asString();
    }
    else {
        //redis中没有则查询mysql
        //查询数据库
        std::shared_ptr<UserInfo> user_info = nullptr;
        user_info = MysqlManager::GetInstance().GetUser(uid);
        if (user_info == nullptr) {
            return false;
        }

        userinfo = user_info;

        //将数据库内容写入redis缓存
        Json::Value redis_root;
        redis_root["uid"] = uid;
        redis_root["pwd"] = userinfo->pwd;
        redis_root["name"] = userinfo->name;
        redis_root["email"] = userinfo->email;
        redis_root["nick"] = userinfo->nick;
        redis_root["desc"] = userinfo->desc;
        redis_root["sex"] = userinfo->sex;
        redis_root["icon"] = userinfo->icon;
        RedisManager::GetInstance().Set(base_key, redis_root.toStyledString());
    }
    return true;
}
