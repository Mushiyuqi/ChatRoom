#include "contactuseritem.h"
#include "ui_contactuseritem.h"

ContactUserItem::ContactUserItem(QWidget *parent) :
    ListItemBase(parent),
    ui(new Ui::ContactUserItem)
{
    ui->setupUi(this);
    SetItemType(ListItemType::Contact_User_Item);
    ui->red_point->raise();
    ShowRedPoint(true);
}

ContactUserItem::~ContactUserItem()
{
    delete ui;
}

QSize ContactUserItem::sizeHint() const
{
    return QSize(250, 70); // 返回自定义的尺寸
}

void ContactUserItem::SetInfo(std::shared_ptr<AuthInfo> auth_info)
{
    m_info = std::make_shared<UserInfo>(auth_info);
    // 加载图片
    QPixmap pixmap(m_info->m_icon);

    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->user_name_lb->setText(m_info->m_name);
}

void ContactUserItem::SetInfo(int uid, QString name, QString icon)
{
    m_info = std::make_shared<UserInfo>(uid,name, name, icon, 0);

    // 加载图片
    QPixmap pixmap(m_info->m_icon);

    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->user_name_lb->setText(m_info->m_name);
}

void ContactUserItem::SetInfo(std::shared_ptr<AuthRsp> auth_rsp){
    m_info = std::make_shared<UserInfo>(auth_rsp);

    // 加载图片
    QPixmap pixmap(m_info->m_icon);

    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->user_name_lb->setText(m_info->m_name);
}

void ContactUserItem::ShowRedPoint(bool show)
{
    if(show){
        ui->red_point->show();
    }else{
        ui->red_point->hide();
    }

}

std::shared_ptr<UserInfo> ContactUserItem::GetInfo()
{
    return m_info;
}
