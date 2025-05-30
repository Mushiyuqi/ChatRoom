#pragma once

#include <QDialog>
#include <global.h>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

private:
    void InitHead();

    Ui::LoginDialog *ui;

signals:
    void switchRegister();
    void switchReset();

private slots:
    void slot_forget_pwd();

};

