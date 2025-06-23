#include "contactuseritem.h"
#include "ui_contactuseritem.h"

ContactUserItem::ContactUserItem(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ContactUserItem)
{
    ui->setupUi(this);
}

ContactUserItem::~ContactUserItem()
{
    delete ui;
}
