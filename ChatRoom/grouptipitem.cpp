#include "grouptipitem.h"
#include "ui_grouptipitem.h"

GroupTipItem::GroupTipItem(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GroupTipItem)
{
    ui->setupUi(this);
}

GroupTipItem::~GroupTipItem()
{
    delete ui;
}
