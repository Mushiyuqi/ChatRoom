#include "mainwindow.h"
#include "./ui_mainwindow.h"

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
}

