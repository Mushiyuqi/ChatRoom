#ifndef CHATUSERITEM_H
#define CHATUSERITEM_H

#include <QWidget>
#include "listitembase.h"

class UserInfo;
namespace Ui {
class ChatUserItem;
}

class ChatUserItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit ChatUserItem(QWidget *parent = nullptr);
    ~ChatUserItem();

    QSize sizeHint() const override;

    void SetInfo(std::shared_ptr<UserInfo> user_info);

private:
    Ui::ChatUserItem *ui;

    QString m_name;
    QString m_head;
    QString m_msg;

    std::shared_ptr<UserInfo> m_user_info;
};

#endif // CHATUSERITEM_H
