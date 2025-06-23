#include "contactuserlist.h"
#include "global.h"
#include "tcpmanager.h"


ContactUserList::ContactUserList(QWidget *parent): m_add_friend_item(nullptr)
{
    Q_UNUSED(parent);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 安装事件过滤器
    this->viewport()->installEventFilter(this);

    //模拟从数据库或者后端传输过来的数据,进行列表加载
    AddContactUserList();
    //连接点击的信号和槽
    connect(this, &QListWidget::itemClicked, this, &ContactUserList::slot_item_clicked);

    // //链接对端同意认证后通知的信号
    // connect(&TcpManager::GetInstance(), &TcpManager::sig_add_auth_friend, this,
    //         &ContactUserList::slot_add_auth_firend);

    // //链接自己点击同意认证后界面刷新
    // connect(&TcpManager::GetInstance(), &TcpManager::sig_auth_rsp, this,
    //         &ContactUserList::slot_auth_rsp);

}

