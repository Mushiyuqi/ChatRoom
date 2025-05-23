#ifndef RESETDIALOG_H
#define RESETDIALOG_H

#include <QDialog>
#include "global.h"

namespace Ui {
class ResetDialog;
}

class ResetDialog : public QDialog
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
    explicit ResetDialog(QWidget *parent = nullptr);
    ~ResetDialog();

private:
    bool CheckUserValid();
    bool CheckPassValid();
    bool CheckEmailValid();
    bool CheckVerifyValid();
    void AddTipErr(TipErr te,QString tips);
    void DelTipErr(TipErr te);
    void ShowTip(QString str,bool b_ok);
    void InitHandlers();


    Ui::ResetDialog *ui;
    QMap<TipErr, QString> m_tip_errs;
    QMap<ReqId, std::function<void(const QJsonObject&)>> m_handlers;

signals:
    void switchLogin();

private slots:

    void on_verify_btn_clicked();
    void on_return_btn_clicked();
    void on_sure_btn_clicked();
    void slot_reset_mod_finish(ReqId id, QString res, ErrorCodes err);
};

#endif // RESETDIALOG_H
