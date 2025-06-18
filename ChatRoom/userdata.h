#pragma once
#include <QString>
class SearchInfo {
public:
    SearchInfo(int uid, QString name, QString nick, QString desc, int sex);
    int m_uid;
    QString m_name;
    QString m_nick;
    QString m_desc;
    int m_sex;
};
