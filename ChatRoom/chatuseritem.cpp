#include "chatuseritem.h"
#include "ui_chatuseritem.h"

ChatUserItem::ChatUserItem(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatUserItem)
{
    ui->setupUi(this);
}

ChatUserItem::~ChatUserItem()
{
    delete ui;
}
