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

bool UserManager::CheckFriendById(int uid)
{

}

int UserManager::GetUid()
{
    return m_uid;
}

QString UserManager::GetName()
{
    return m_name;
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
