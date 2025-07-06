#pragma once
#include <QListWidget>
#include <QEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include <QDebug>
#include "userdata.h"

class ContactUserItem;
class AddFriendApply;
class AuthInfo;
class AuthRsp;
class UserInfo;

class ContactUserList : public QListWidget
{
    Q_OBJECT
public:
    ContactUserList(QWidget* parent = nullptr);
    void ShowRedPoint(bool bshow = true);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void AddContactUserList();

public slots:
    void slot_item_clicked(QListWidgetItem *item);
    void slot_add_auth_firend(std::shared_ptr<AuthInfo> auth_info);
    void slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp);

signals:
    void sig_loading_contact_user();
    void sig_switch_apply_friend_page();
    void sig_switch_friend_info_page(std::shared_ptr<UserInfo> user_info);

private:
    bool m_load_pending;
    ContactUserItem* m_add_friend_item;
    QListWidgetItem * m_groupitem;

};
