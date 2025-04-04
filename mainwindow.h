#pragma once

#include <QMainWindow>
#include "logindialog.h"
#include "registerdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow() override;

public slots:
    void SlotSwitchReg();

private:
    Ui::MainWindow *ui;
    LoginDialog* m_login_dlg;
    RegisterDialog* m_reg_dlg;
};
