#pragma once
#include <QDialog>
#include "global.h"

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

private slots:
    // 获取验证码
    void on_get_code_clicked();
    // 接收属于Register模块的槽
    void slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err);
    // 注册用户
    void on_confirm_btn_clicked();

private:
    void ShowTip(QString str, bool b_ok);
    // 注册Register模块回调函数
    void InitHttpHandlers();
    // 验证密码的正则表达式
    bool ValidatePassword(const QString &password, QString &errorMsg);

    Ui::RegisterDialog *ui;
    // 存储各id对应的处理函数
    QMap<ReqId, std::function<void(const QJsonObject&)>> m_handlers;
};
