#include "usermanager.h"

UserManager &UserManager::GetInstance()
{
    static UserManager instance;
    return instance;
}

void UserManager::SetName(QString name)
{
    m_name = name;
}

void UserManager::SetUid(int uid)
{
    m_uid = uid;
}

void UserManager::SetToken(QString token)
{
    m_token = token;
}

int UserManager::GetUid()
{
    return m_uid;
}

QString UserManager::GetName()
{
    return m_name;
}
