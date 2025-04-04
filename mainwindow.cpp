#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_login_dlg = new LoginDialog();
    setCentralWidget(m_login_dlg);
    m_login_dlg->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

