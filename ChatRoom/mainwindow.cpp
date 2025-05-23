#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "resetdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_login_dlg = new LoginDialog(this);
    // 避免页面切换是出错
    m_login_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    setCentralWidget(m_login_dlg);
    // 创建和注册消息连接
    connect(m_login_dlg, &LoginDialog::switchRegister, this, &MainWindow::SlotSwitchReg);
    // 创建和重置密码连接
    connect(m_login_dlg, &LoginDialog::switchReset, this, &MainWindow::SlotSwitchReset);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SlotSwitchReg()
{
    m_reg_dlg = new RegisterDialog(this);
    m_reg_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    // 连接注册信息返回登陆界面信号
    connect(m_reg_dlg, &RegisterDialog::sigSwitchLogin, this, &MainWindow::SlotSwitchLogin);

    setCentralWidget(m_reg_dlg);
    m_login_dlg->hide();
    m_reg_dlg->show();
}

void MainWindow::SlotSwitchLogin()
{
    m_login_dlg = new LoginDialog(this);
    // 避免页面切换是出错
    m_login_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    setCentralWidget(m_login_dlg);

    m_reg_dlg->hide();
    m_login_dlg->show();

    // 创建和注册消息连接
    connect(m_login_dlg, &LoginDialog::switchRegister, this, &MainWindow::SlotSwitchReg);
    // 创建和重置密码连接
    connect(m_login_dlg, &LoginDialog::switchReset, this, &MainWindow::SlotSwitchReset);
}

void MainWindow::SlotSwitchReset()
{
    //创建一个CentralWidget, 并将其设置为MainWindow的中心部件
    m_reset_dlg = new ResetDialog(this);
    m_reset_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(m_reset_dlg);

    m_login_dlg->hide();
    m_reset_dlg->show();

    //注册返回登录信号和槽函数
    connect(m_reset_dlg, &ResetDialog::switchLogin, this, &MainWindow::SlotSwitchLogin2);
}

void MainWindow::SlotSwitchLogin2()
{
    //创建一个CentralWidget, 并将其设置为MainWindow的中心部件
    m_login_dlg = new LoginDialog(this);
    m_login_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(m_login_dlg);

    m_reset_dlg->hide();
    m_login_dlg->show();

    //连接登录界面注册信号
    connect(m_login_dlg, &LoginDialog::switchRegister, this, &MainWindow::SlotSwitchReg);
    //连接登录界面忘记密码信号
    connect(m_login_dlg, &LoginDialog::switchReset, this, &MainWindow::SlotSwitchReset);
}

