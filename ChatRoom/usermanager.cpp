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
