#include "chatuseritem.h"
#include "ui_chatuseritem.h"
#include "userdata.h"

ChatUserItem::ChatUserItem(QWidget *parent)
    : ListItemBase(parent)
    , ui(new Ui::ChatUserItem)
{
    ui->setupUi(this);
    SetItemType(ListItemType::Chat_User_Item);
}

ChatUserItem::~ChatUserItem()
{
    delete ui;
}

QSize ChatUserItem::sizeHint() const
{
    return QSize(250, 70);
}

void ChatUserItem::SetInfo(std::shared_ptr<UserInfo> user_info)
{
    m_user_info = user_info;
    QPixmap pixmap(m_user_info->m_icon);

    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(),
                                         Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->user_name_lb->setText(m_user_info->m_name);
    ui->user_chat_lb->setText(m_user_info->m_last_msg);
}

void ChatUserItem::SetInfo(std::shared_ptr<FriendInfo> friend_info)
{
    SetInfo(std::make_shared<UserInfo>(friend_info));
}

std::shared_ptr<UserInfo> ChatUserItem::GetUserInfo()
{
    return m_user_info;
}

void ChatUserItem::UpdateLastMsg(std::vector<std::shared_ptr<TextChatData>> msgs)
{
    QString last_msg = "";
    for (auto& msg : msgs) {
        last_msg = msg->m_msg_content;
        m_user_info->m_chat_msgs.push_back(msg);
    }

    m_user_info->m_last_msg = last_msg;
    ui->user_chat_lb->setText(m_user_info->m_last_msg);
}
