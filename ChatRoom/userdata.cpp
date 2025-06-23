#include "userdata.h"
SearchInfo::SearchInfo(int uid, QString name,
                       QString nick, QString desc, int sex, QString icon):m_uid(uid)
    ,m_name(name), m_nick(nick),m_desc(desc),m_sex(sex),m_icon(icon){
}

AddFriendApply::AddFriendApply(int from_uid, QString name, QString desc):m_from_uid(from_uid),m_name(name),m_desc(desc)
{

}
