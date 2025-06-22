#pragma once
#include <memory>
#include <QObject>

class UserManager:public QObject
{
public:
    ~UserManager() = default;
    UserManager(const UserManager&) = delete;
    UserManager& operator=(const UserManager&) = delete;

    static UserManager& GetInstance();

    void SetName(QString name);
    void SetUid(int uid);
    void SetToken(QString token);

    int GetUid();
    QString GetName();

private:
    UserManager() = default;

    QString m_name;
    QString m_token;
    int m_uid;

};

