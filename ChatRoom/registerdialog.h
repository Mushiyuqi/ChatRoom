#pragma once
#include <QDialog>
#include "global.h"

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
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

    // 检测输入
    bool CheckUserValid();
    bool CheckEmailValid();
    bool CheckPasswordValid();
    bool CheckConfirmValid();
    bool CheckVerifyCodeValid();

    void AddTipErr(TipErr err, QString msg);
    void DelTipErr(TipErr err);

    QMap<TipErr, QString> m_tip_errs;

    Ui::RegisterDialog *ui;
    // 存储各id对应的处理函数
    QMap<ReqId, std::function<void(const QJsonObject&)>> m_handlers;
};
