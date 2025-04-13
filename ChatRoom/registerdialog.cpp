#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "global.h"
#include "httpmanager.h"
#include "registerdialog.h"

RegisterDialog::RegisterDialog(QWidget* parent)
    : QDialog(parent)
      , ui(new Ui::RegisterDialog) {
    ui->setupUi(this);
    ui->pass_edit->setEchoMode(QLineEdit::Password);
    ui->confirm_edit->setEchoMode(QLineEdit::Password);
    ui->err_tip->setProperty("state", "normal");
    repolish(ui->err_tip);

    // 连接信号与槽
    connect(&HttpManager::GetInstance(), &HttpManager::sig_reg_mod_finish, this, &RegisterDialog::slot_reg_mod_finish);

    // 注册Register模块回调函数
    InitHttpHandlers();
}

RegisterDialog::~RegisterDialog() {
    delete ui;
}

void RegisterDialog::on_get_code_clicked() {
    // 正则判断邮箱
    auto email = ui->email_edit->text();
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch();
    if (match) {
        // 发送验证码
        QJsonObject json_obj;
        json_obj["email"] = email;
        HttpManager::GetInstance().PostHttpReq(QUrl(gate_url_prefix + "/get_verifycode"),
                                               json_obj,
                                               ReqId::ID_GET_VARIFY_CODE,
                                               Modules::REGISTER_MOD);
    }
    else {
        ShowTip(tr("邮箱地址不正确"), match);
    }
}

void RegisterDialog::slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err) {
    if (err != ErrorCodes::SUCCESS) {
        ShowTip("网络请求错误", false);
        return;
    }

    // 解析Json 字符串, res 转化为QByteArray
    QJsonDocument json_doc = QJsonDocument::fromJson(res.toUtf8());
    if (json_doc.isNull()) {
        ShowTip(tr("json解析失败"), false);
        return;
    }
    // Json 解析错误
    if (!json_doc.isObject()) {
        ShowTip(tr("json解析失败"), false);
        return;
    }

    // 根据id处理Json数据
    m_handlers[id](json_doc.object());
    return;
}

void RegisterDialog::ShowTip(QString str, bool b_ok) {
    // 更改状态
    if (b_ok)
        ui->err_tip->setProperty("state", "normal");
    else
        ui->err_tip->setProperty("state", "err");

    // 刷新状态
    repolish(ui->err_tip);
    // 更改文字
    ui->err_tip->setText(str);
}

void RegisterDialog::InitHttpHandlers() {
    // 注册获取验证码回包的逻辑
    m_handlers[ReqId::ID_GET_VARIFY_CODE] = [this](const QJsonObject& json_obj) {
        int error = json_obj["error"].toInt();
        if(error != ErrorCodes::SUCCESS) {
            ShowTip(tr("验证码获取失败"), false);
            return;
        }

        auto email = json_obj["email"].toString();
        ShowTip(tr("验证码发送成功, 请查收"), true);
        qDebug() << "RegisterDialog::InitHttpHandlers Handlers:ID_GET_VARIFY_CODE email is " << email;
    };
}

