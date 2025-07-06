#pragma once
#include <memory>
#include <QObject>
#include "userdata.h"
#include <vector>


class UserManager:public QObject
{
public:
    ~UserManager() = default;
    UserManager(const UserManager&) = delete;
    UserManager& operator=(const UserManager&) = delete;

    static UserManager& GetInstance();

    void SetToken(QString token);

    bool CheckFriendById(int uid);

    int GetUid();
    QString GetName();

    std::vector<std::shared_ptr<ApplyInfo>> GetApplyList();
    bool AlreadyApply(int uid);
    void AddApplyList(std::shared_ptr<ApplyInfo> app);
    void SetUserInfo(std::shared_ptr<UserInfo> userInfo);
    void AppendApplyList(QJsonArray arr);

private:
    UserManager() = default;

    QString m_name;
    QString m_token;
    int m_uid;
    std::vector<std::shared_ptr<ApplyInfo>> m_apply_list;
    std::shared_ptr<UserInfo> m_user_info;
};

