#include "contactuserlist.h"
#include "global.h"
#include "tcpmanager.h"
#include "contactuseritem.h"
#include "grouptipitem.h"
#include "usermanager.h"
#include <QRandomGenerator>
#include "loadingdlg.h"
#include <QTimer>

ContactUserList::ContactUserList(QWidget *parent): m_add_friend_item(nullptr), m_load_pending(false)
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

    //链接对端同意认证后通知的信号
    connect(&TcpManager::GetInstance(), &TcpManager::sig_add_auth_friend, this,
            &ContactUserList::slot_add_auth_firend);

    //链接自己点击同意认证后界面刷新
    connect(&TcpManager::GetInstance(), &TcpManager::sig_auth_rsp, this,
            &ContactUserList::slot_auth_rsp);

}

void ContactUserList::ShowRedPoint(bool bshow /*= true*/)
{
    m_add_friend_item->ShowRedPoint(bshow);
}

void ContactUserList::AddContactUserList()
{
    //获取好友列表

    auto * groupTip = new GroupTipItem();
    groupTip->SetGroupTip(tr("新的朋友"));
    QListWidgetItem *item = new QListWidgetItem;
    item->setSizeHint(groupTip->sizeHint());
    this->addItem(item);
    this->setItemWidget(item, groupTip);
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);

    m_add_friend_item = new ContactUserItem();
    m_add_friend_item->setObjectName("new_friend_item");
    m_add_friend_item->SetInfo(0,tr("新的朋友"),":/resource/add_friend.png");
    m_add_friend_item->SetItemType(ListItemType::Apply_Friend_Item);
    QListWidgetItem *add_item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    add_item->setSizeHint(m_add_friend_item->sizeHint());
    this->addItem(add_item);
    this->setItemWidget(add_item, m_add_friend_item);
    //默认设置新的朋友申请条目被选中
    this->setCurrentItem(add_item);

    auto * groupCon = new GroupTipItem();
    groupCon->SetGroupTip(tr("联系人"));
    m_groupitem = new QListWidgetItem;
    m_groupitem->setSizeHint(groupCon->sizeHint());
    this->addItem(m_groupitem);
    this->setItemWidget(m_groupitem, groupCon);
    m_groupitem->setFlags(m_groupitem->flags() & ~Qt::ItemIsSelectable);
}

bool ContactUserList::eventFilter(QObject *watched, QEvent *event)
{
    // 检查事件是否是鼠标悬浮进入或离开
    if (watched == this->viewport()) {
        if (event->type() == QEvent::Enter) {
            // 鼠标悬浮，显示滚动条
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        } else if (event->type() == QEvent::Leave) {
            // 鼠标离开，隐藏滚动条
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }

    // 检查事件是否是鼠标滚轮事件
    if (watched == this->viewport() && event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
        int numDegrees = wheelEvent->angleDelta().y() / 8;
        int numSteps = numDegrees / 15; // 计算滚动步数

        // 设置滚动幅度
        this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - numSteps);

        // 检查是否滚动到底部
        QScrollBar *scrollBar = this->verticalScrollBar();
        int maxScrollValue = scrollBar->maximum();
        int currentValue = scrollBar->value();
        //int pageSize = 10; // 每页加载的联系人数量

        if (maxScrollValue - currentValue <= 0) {
            auto b_loaded = UserManager::GetInstance().IsLoadConFin();
            if(b_loaded){
                return true;
            }

            if(m_load_pending){
                return true;
            }
            // 滚动到底部，加载新的联系人
            qDebug()<<"load more chat user";
            m_load_pending = true;

            QTimer::singleShot(100, [this](){
                m_load_pending = false;
            });
            //发送信号通知聊天界面加载更多聊天内容
            emit sig_loading_contact_user();
        }

        return true; // 停止事件传递
    }

    return QListWidget::eventFilter(watched, event);
}

void ContactUserList::slot_item_clicked(QListWidgetItem *item)
{
    QWidget *widget = this->itemWidget(item); // 获取自定义widget对象
    if(!widget){
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }

    // 对自定义widget进行操作， 将item 转化为基类ListItemBase
    ListItemBase *customItem = qobject_cast<ListItemBase*>(widget);
    if(!customItem){
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }

    auto itemType = customItem->GetItemType();
    if(itemType == ListItemType::Invalid_Item
        || itemType == ListItemType::Group_Tip_Item){
        qDebug()<< "slot invalid item clicked ";
        return;
    }

    if(itemType == ListItemType::Apply_Friend_Item){

        // 创建对话框，提示用户
        qDebug()<< "apply friend item clicked ";
        //跳转到好友申请界面
        emit sig_switch_apply_friend_page();
        return;
    }

    if(itemType == ListItemType::Contact_User_Item){
        // 创建对话框，提示用户
        qDebug()<< "contact user item clicked ";

        auto con_item = qobject_cast<ContactUserItem*>(customItem);
        auto user_info = con_item->GetInfo();
        //跳转到好友申请界面
        emit sig_switch_friend_info_page(user_info);
        return;
    }
}

void ContactUserList::slot_add_auth_firend(std::shared_ptr<AuthInfo> auth_info)
{
    bool isFriend = UserManager::GetInstance().CheckFriendById(auth_info->m_uid);
    if(isFriend) return;

    auto *con_user_wid = new ContactUserItem();
    con_user_wid->SetInfo(auth_info);
    QListWidgetItem* item = new QListWidgetItem;
    item->setSizeHint(con_user_wid->sizeHint());

    // 获取groupitem 的索引
    int index = this->row(m_groupitem);
    this->insertItem(index+1, item);

    this->setItemWidget(item, con_user_wid);
}

void ContactUserList::slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp)
{
    bool isFriend = UserManager::GetInstance().CheckFriendById(auth_rsp->m_uid);
    if(isFriend) return;

    auto *con_user_wid = new ContactUserItem();
    con_user_wid->SetInfo(auth_rsp->m_uid, auth_rsp->m_name, auth_rsp->m_icon);
    QListWidgetItem* item = new QListWidgetItem;
    item->setSizeHint(con_user_wid->sizeHint());

    // 获取groupitem 的索引
    int index = this->row(m_groupitem);
    this->insertItem(index+1, item);

    this->setItemWidget(item, con_user_wid);
}

