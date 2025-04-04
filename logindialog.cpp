#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    ui->verticalLayout->setContentsMargins(5,5,5,5);

}

LoginDialog::~LoginDialog()
{
    delete ui;
}
