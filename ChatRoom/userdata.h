#pragma once
#include <QString>
#include <memory>

class SearchInfo {
public:
    SearchInfo(int uid, QString name, QString nick, QString desc, int sex, QString icon);
    int m_uid;
    QString m_name;
    QString m_nick;
    QString m_desc;
    int m_sex;
    QString m_icon;
};

class AddFriendApply {
public:
    AddFriendApply(int from_uid, QString name, QString desc);
    int m_from_uid;
    QString m_name;
    QString m_desc;
};

struct ApplyInfo {
    ApplyInfo(int uid, QString name, QString desc,
              QString icon, QString nick, int sex, int status)
        :m_uid(uid),m_name(name),m_desc(desc),
        m_icon(icon),m_nick(nick),m_sex(sex),m_status(status){}

    void SetIcon(QString head){
        m_icon = head;
    }
    int m_uid;
    QString m_name;
    QString m_desc;
    QString m_icon;
    QString m_nick;
    int m_sex;
    int m_status;
};

struct AuthInfo {
    AuthInfo(int uid, QString name,
             QString nick, QString icon, int sex):
        m_uid(uid), m_name(name), m_nick(nick), m_icon(icon),
        m_sex(sex){}
    int m_uid;
    QString m_name;
    QString m_nick;
    QString m_icon;
    int m_sex;
};

struct AuthRsp {
    AuthRsp(int peer_uid, QString peer_name,
            QString peer_nick, QString peer_icon, int peer_sex)
        :m_uid(peer_uid),m_name(peer_name),m_nick(peer_nick),
        m_icon(peer_icon),m_sex(peer_sex)
    {}

    int m_uid;
    QString m_name;
    QString m_nick;
    QString m_icon;
    int m_sex;
};

struct FriendInfo {
    FriendInfo(int uid, QString name, QString nick, QString icon,
               int sex, QString desc, QString back, QString last_msg=""):m_uid(uid),
        m_name(name),m_nick(nick),m_icon(icon),m_sex(sex),m_desc(desc),
        m_back(back),m_last_msg(last_msg){}

    FriendInfo(std::shared_ptr<AuthInfo> auth_info):m_uid(auth_info->m_uid),
        m_nick(auth_info->m_nick),m_icon(auth_info->m_icon),m_name(auth_info->m_name),
        m_sex(auth_info->m_sex){}

    FriendInfo(std::shared_ptr<AuthRsp> auth_rsp):m_uid(auth_rsp->m_uid),
        m_nick(auth_rsp->m_nick),m_icon(auth_rsp->m_icon),m_name(auth_rsp->m_name),
        m_sex(auth_rsp->m_sex){}

    int m_uid;
    QString m_name;
    QString m_nick;
    QString m_icon;
    int m_sex;
    QString m_desc;
    QString m_back;
    QString m_last_msg;

};

struct UserInfo {
    UserInfo(int uid, QString name, QString nick, QString icon, int sex, QString last_msg = ""):
        m_uid(uid),m_name(name),m_nick(nick),m_icon(icon),m_sex(sex),m_last_msg(last_msg){}

    UserInfo(std::shared_ptr<AuthInfo> auth):
        m_uid(auth->m_uid),m_name(auth->m_name),m_nick(auth->m_nick),
        m_icon(auth->m_icon),m_sex(auth->m_sex),m_last_msg(""){}

    UserInfo(int uid, QString name, QString icon):
        m_uid(uid), m_name(name), m_icon(icon),m_nick(m_name),
        m_sex(0),m_last_msg(""){

    }

    UserInfo(std::shared_ptr<AuthRsp> auth):
        m_uid(auth->m_uid),m_name(auth->m_name),m_nick(auth->m_nick),
        m_icon(auth->m_icon),m_sex(auth->m_sex),m_last_msg(""){}

    UserInfo(std::shared_ptr<SearchInfo> search_info):
        m_uid(search_info->m_uid),m_name(search_info->m_name),m_nick(search_info->m_nick),
        m_icon(search_info->m_icon),m_sex(search_info->m_sex),m_last_msg(""){

    }

    UserInfo(std::shared_ptr<FriendInfo> friend_info):
        m_uid(friend_info->m_uid),m_name(friend_info->m_name),m_nick(friend_info->m_nick),
        m_icon(friend_info->m_icon),m_sex(friend_info->m_sex),m_last_msg(""){

    }

    int m_uid;
    QString m_name;
    QString m_nick;
    QString m_icon;
    int m_sex;
    QString m_last_msg;

};
