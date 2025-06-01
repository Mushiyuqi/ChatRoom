#pragma once

#include <QDialog>
#include <global.h>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT
    // 输入文本错误处理
    enum TipErr{
        TIP_SUCCESS = 0,
        TIP_EMAIL_ERR = 1,
        TIP_PWD_ERR = 2,
        TIP_CONFIRM_ERR = 3,
        TIP_PWD_CONFIRM = 4,
        TIP_VERIFY_ERR = 5,
        TIP_USER_ERR = 6
    };

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

private:
    void InitHead();
    bool CheckEmailValid();
    bool CheckPassValid();
    void AddTipErr(TipErr, QString);
    void DelTipErr(TipErr);
    void ShowTip(QString, bool);
    void EnableBtn(bool state);

    // 登陆模块回调函数
    void InitHttpHandlers();

    Ui::LoginDialog *ui;
    QMap<TipErr, QString> m_tip_errs;
    QMap<ReqId, std::function<void(const QJsonObject&)>> m_handlers;

signals:
    void switchRegister();
    void switchReset();
    void sig_connect_tcp(ServerInfo si);

private slots:
    void slot_forget_pwd();
    void on_login_btn_clicked();
    void slot_login_mod_finish(ReqId id, QString res, ErrorCodes err);
};

