#include "usermanager.h"
#include <QJsonArray>

UserManager &UserManager::GetInstance()
{
    static UserManager instance;
    return instance;
}

void UserManager::SetToken(QString token)
{
    m_token = token;
}

bool UserManager::CheckFriendById(int uid)
{
    auto iter = m_friend_map.find(uid);
    if(iter == m_friend_map.end()){
        return false;
    }
    return true;
}

int UserManager::GetUid()
{
    return m_user_info->m_uid;
}

QString UserManager::GetName()
{
    return m_user_info->m_name;
}

std::vector<std::shared_ptr<ApplyInfo> > UserManager::GetApplyList()
{
    return m_apply_list;
}

bool UserManager::AlreadyApply(int uid)
{
    for(auto& apply : m_apply_list){
        if(apply->m_uid == uid)
            return true;
    }
    return false;
}

void UserManager::AddApplyList(std::shared_ptr<ApplyInfo> app)
{
    m_apply_list.push_back(app);
}

void UserManager::SetUserInfo(std::shared_ptr<UserInfo> userInfo)
{
    m_user_info = userInfo;
}

void UserManager::AppendApplyList(QJsonArray arr)
{
    // 遍历 QJsonArray 并输出每个元素
    for (const QJsonValue &value : arr) {
        auto name = value["name"].toString();
        auto desc = value["desc"].toString();
        auto icon = value["icon"].toString();
        auto nick = value["nick"].toString();
        auto sex = value["sex"].toInt();
        auto uid = value["uid"].toInt();
        auto status = value["status"].toInt();
        auto info = std::make_shared<ApplyInfo>(uid, name,
                                                desc, icon, nick, sex, status);
        m_apply_list.push_back(info);
    }
}

void UserManager::AddFriend(std::shared_ptr<AuthRsp> auth_rsp)
{
    auto friend_info = std::make_shared<FriendInfo>(auth_rsp);
    m_friend_map[friend_info->m_uid] = friend_info;
}

void UserManager::AddFriend(std::shared_ptr<AuthInfo> auth_info)
{
    auto friend_info = std::make_shared<FriendInfo>(auth_info);
    m_friend_map[friend_info->m_uid] = friend_info;
}

std::shared_ptr<FriendInfo> UserManager::GetFriendById(int uid)
{
    auto find_it = m_friend_map.find(uid);
    if(find_it == m_friend_map.end()){
        return nullptr;
    }

    return *find_it;
}
