#pragma once

#include <QWidget>
#include "userdata.h"
#include <memory.h>
#include <QJsonArray>
#include <unordered_map>
#include "applyfrienditem.h"

namespace Ui {
class ApplyFriendPage;
}

class ApplyFriendPage : public QWidget
{
    Q_OBJECT

public:
    explicit ApplyFriendPage(QWidget *parent = nullptr);
    ~ApplyFriendPage();
    void AddNewApply(std::shared_ptr<AddFriendApply> apply);

protected:
    void paintEvent(QPaintEvent *event);

private:
    void loadApplyList();

    Ui::ApplyFriendPage *ui;
    std::unordered_map<int, ApplyFriendItem*> m_unauth_items;

public slots:
    void slot_auth_rsp(std::shared_ptr<AuthRsp> );

signals:
    void sig_show_search(bool);

};
