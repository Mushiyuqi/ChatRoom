#include "chatuseritem.h"
#include "ui_chatuseritem.h"

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

void ChatUserItem::SetInfo(QString name, QString head, QString msg){
    m_name = name;
    m_head = head;
    m_msg = msg;
    // 加载图片
    QPixmap pixmap(m_head);

    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(),
                                         Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->user_name_lb->setText(m_name);
    ui->user_chat_lb->setText(m_msg);
}
