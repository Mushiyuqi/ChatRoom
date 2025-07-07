#pragma once
#include <memory>
#include <QObject>
#include "userdata.h"
#include <vector>
#include <QMap>


class UserManager:public QObject
{
public:
    ~UserManager() = default;
    UserManager(const UserManager&);
    UserManager& operator=(const UserManager&) = delete;

    static UserManager& GetInstance();

    void SetToken(QString token);

    bool CheckFriendById(int uid);

    int GetUid();
    QString GetName();
    std::shared_ptr<UserInfo> GetUserInfo();

    std::vector<std::shared_ptr<ApplyInfo>> GetApplyList();
    bool AlreadyApply(int uid);
    void AddApplyList(std::shared_ptr<ApplyInfo> app);
    void SetUserInfo(std::shared_ptr<UserInfo> userInfo);
    void AppendApplyList(QJsonArray arr);
    void AppendFriendList(QJsonArray arr);
    void AddFriend(std::shared_ptr<AuthRsp> auth_rsp);
    void AddFriend(std::shared_ptr<AuthInfo> auth_info);
    std::shared_ptr<FriendInfo> GetFriendById(int uid);

    std::vector<std::shared_ptr<FriendInfo>> GetChatListPerPage();
    bool IsLoadChatFin();
    void UpdateChatLoadedCount();
    std::vector<std::shared_ptr<FriendInfo>> GetConListPerPage();
    bool IsLoadConFin();
    void UpdateContactLoadedCount();

    void AppendFriendChatMsg(int friend_id,std::vector<std::shared_ptr<TextChatData>>);
private:
    UserManager() = default;

    QString m_name;
    QString m_token;
    int m_uid;
    std::vector<std::shared_ptr<ApplyInfo>> m_apply_list;
    std::vector<std::shared_ptr<FriendInfo>> m_friend_list;
    std::shared_ptr<UserInfo> m_user_info;
    QMap<int, std::shared_ptr<FriendInfo>> m_friend_map;
    int m_chat_loaded;
    int m_contact_loaded;
};

