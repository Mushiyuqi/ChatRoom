#include "mainwindow.h"
#include "global.h"
#include <QApplication>

/**
 * 大驼峰命名: 函数名, 类名
 * 小驼峰命名: 局部变量
 * 下划线命名: 类成员变量, 信号槽
 */

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 加载qss
    QFile qss(":/style/stylesheet.qss");
    if(qss.open(QFile::ReadOnly)){
        qDebug("Open success");
        QString style = QLatin1String(qss.readAll());
        a.setStyleSheet(style);
        qss.close();
    }else{
        qDebug("Open failed");
    }

    // 读取config.ini
    QString fileName = "config.ini";
    QString appPath = QCoreApplication::applicationDirPath();
    QString configPath = QDir::toNativeSeparators(appPath + QDir::separator() + fileName);
    QSettings settings(configPath, QSettings::IniFormat);
    QString gateHost = settings.value("GateServer/host").toString();
    QString gatePort = settings.value("GateServer/port").toString();
    gate_url_prefix = "http://" + gateHost + ":" + gatePort;

    MainWindow w;
    w.show();
    return a.exec();
}
