#include <QRegularExpression>
#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "global.h"
#include "httpmanager.h"
#include "registerdialog.h"

RegisterDialog::RegisterDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::RegisterDialog) {
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
    m_handlers[ReqId::ID_GET_VARIFY_CODE] = [this](const QJsonObject& jsonObj) {
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS) {
            ShowTip(tr("验证码获取失败"), false);
            return;
        }

        auto email = jsonObj["email"].toString();
        ShowTip(tr("验证码发送成功, 请查收"), true);
        qDebug() << "RegisterDialog::InitHttpHandlers ID_GET_VARIFY_CODE email is " << email;
    };
    // 注册用户的回调函数
    m_handlers[ReqId::ID_REG_USER] = [this](const QJsonObject& jsonObj) {
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            ShowTip(tr("参数错误"),false);
            return;
        }
        auto uid = jsonObj["uid"].toInt();
        auto email = jsonObj["email"].toString();
        ShowTip(tr("用户注册成功"), true);
        qDebug()<< "RegisterDialog::InitHttpHandlers ID_REG_USER uid is " << uid ;
        qDebug()<< "RegisterDialog::InitHttpHandlers ID_REG_USER email is " << email ;
    };
}

bool RegisterDialog::ValidatePassword(const QString &password, QString &errorMsg)
{
    // 基础长度检查
    if (password.length() < 8) {
        errorMsg = "密码长度不足8位";
        return false;
    }

    // 各字符类型检查
    QRegularExpression letterReg("[A-Za-z]");
    QRegularExpression digitReg("\\d");
    QRegularExpression symbolReg("[!@#$%^&*()_+{}|:\"<>?~\\-\\[\\]\\\\';.,/=]");

    if (!password.contains(letterReg)) {
        errorMsg = "密码必须包含至少一个字母";
        return false;
    }
    if (!password.contains(digitReg)) {
        errorMsg = "密码必须包含至少一个数字";
        return false;
    }
    if (!password.contains(symbolReg)) {
        errorMsg = "密码必须包含至少一个特殊符号";
        return false;
    }

    return true;
}

void RegisterDialog::on_confirm_btn_clicked()
{
    // 验证用户名
    if(ui->user_edit->text() == ""){
        ShowTip(tr("用户名不能为空"), false);
        return;
    }
    // 验证邮箱
    if(ui->email_edit->text() == ""){
        ShowTip(tr("邮箱不能为空"), false);
        return;
    }
    // 验证密码
    QString errorMsg;
    if(!ValidatePassword(ui->pass_edit->text(), errorMsg)){
        ShowTip(errorMsg, false);
        return;
    }
    // 验证确认密码
    if(ui->confirm_edit->text() == ""){
        ShowTip(tr("确认密码不能为空"), false);
        return;
    }
    // 验证密码与确认密码匹配
    if(ui->confirm_edit->text() != ui->pass_edit->text()){
        ShowTip(tr("密码和确认密码不匹配"), false);
        return;
    }
    // 验证验证码
    if(ui->varify_edit->text() == ""){
        ShowTip(tr("验证码不能为空"), false);
        return;
    }
    // 发送http请求注册用户
    QJsonObject jsonObj;
    jsonObj["user"] = ui->user_edit->text();
    jsonObj["email"] = ui->email_edit->text();
    jsonObj["password"] = ui->pass_edit->text();
    jsonObj["confirm"] = ui->confirm_edit->text();
    jsonObj["verifycode"] = ui->varify_edit->text();
    HttpManager::GetInstance().PostHttpReq(QUrl(gate_url_prefix + "/user_register"),
                                           jsonObj,
                                           ReqId::ID_REG_USER, Modules::REGISTER_MOD);
}

