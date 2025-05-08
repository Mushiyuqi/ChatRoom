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
    connect(ui->user_edit, &QLineEdit::editingFinished, this, [this](){CheckUserValid();});
    connect(ui->email_edit, &QLineEdit::editingFinished, this, [this](){CheckEmailValid();});
    connect(ui->pass_edit, &QLineEdit::editingFinished, this, [this](){CheckPasswordValid();});
    connect(ui->confirm_edit, &QLineEdit::editingFinished, this, [this](){CheckConfirmValid();});
    connect(ui->verify_edit, &QLineEdit::editingFinished, this, [this](){CheckVerifyCodeValid();});
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
        // 重置定时按钮
        ui->get_code->UpDataTimer();
        ShowTip(tr("邮箱地址不正确"), match);
    }
}

void RegisterDialog::slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err) {
    if (err != ErrorCodes::SUCCESS) {
        ShowTip("网络请求错误", false);
        // 重置定时按钮
        ui->get_code->UpDataTimer();
        return;
    }

    // 解析Json 字符串, res 转化为QByteArray
    QJsonDocument json_doc = QJsonDocument::fromJson(res.toUtf8());
    if (json_doc.isNull()) {
        ShowTip(tr("json解析失败"), false);
        // 重置定时按钮
        ui->get_code->UpDataTimer();
        return;
    }
    // Json 解析错误
    if (!json_doc.isObject()) {
        ShowTip(tr("json解析失败"), false);
        // 重置定时按钮
        ui->get_code->UpDataTimer();
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

bool RegisterDialog::CheckUserValid()
{
    if(ui->user_edit->text().isEmpty()){
        AddTipErr(TipErr::TIP_USER_ERR, tr("用户名不能为空"));
        return false;
    }
    DelTipErr(TipErr::TIP_USER_ERR);
    return true;
}

bool RegisterDialog::CheckEmailValid()
{
    auto email = ui->email_edit->text();
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch();
    if(!match){
        AddTipErr(TipErr::TIP_EMAIL_ERR, tr("邮箱地址不正确"));
        return false;
    }
    DelTipErr(TipErr::TIP_EMAIL_ERR);
    return true;
}

bool RegisterDialog::CheckPasswordValid()
{
    auto password = ui->pass_edit->text();
    auto confirm = ui->confirm_edit->text();

    QRegularExpression letterReg("[A-Za-z]");
    QRegularExpression digitReg("\\d");
    QRegularExpression symbolReg("[!@#$%^&*()_+{}|:\"<>?~\\-\\[\\]\\\\';.,/=]");

    // 基础长度检查
    if (password.length() < 8) {
        AddTipErr(TipErr::TIP_PWD_ERR, tr("密码长度不足8位"));
        return false;
    }
    if (password.length() > 32) {
        AddTipErr(TipErr::TIP_PWD_ERR, tr("密码长度过长"));
        return false;
    }
    // 各字符类型检查
    if (!password.contains(letterReg)) {
        AddTipErr(TipErr::TIP_PWD_ERR, tr("密码必须包含至少一个字母"));
        return false;
    }
    if (!password.contains(digitReg)) {
        AddTipErr(TipErr::TIP_PWD_ERR, tr("密码必须包含至少一个数字"));
        return false;
    }
    if (!password.contains(symbolReg)) {
        AddTipErr(TipErr::TIP_PWD_ERR, tr("密码必须包含至少一个特殊符号"));
        return false;
    }
    DelTipErr(TipErr::TIP_PWD_ERR);

    // 判断与验证密码是否相同
    if (password != confirm){
        AddTipErr(TipErr::TIP_PWD_CONFIRM, tr("密码与确认密码不匹配"));
        return false;
    }else{
        DelTipErr(TipErr::TIP_PWD_CONFIRM);
    }
    return true;
}

bool RegisterDialog::CheckConfirmValid()
{
    auto confirm = ui->confirm_edit->text();
    auto password = ui->pass_edit->text();
    if(confirm != password){
        AddTipErr(TipErr::TIP_PWD_CONFIRM, tr("密码与确认密码不匹配"));
        return false;
    }else{
        DelTipErr(TipErr::TIP_PWD_CONFIRM);
    }
    return true;
}

bool RegisterDialog::CheckVerifyCodeValid()
{
    auto code = ui->verify_edit->text();
    if(code.isEmpty()){
        AddTipErr(TipErr::TIP_VERIFY_ERR, tr("验证码不能为空"));
        return false;
    }
    DelTipErr(TipErr::TIP_VERIFY_ERR);
    return true;
}

void RegisterDialog::AddTipErr(TipErr err, QString tip)
{
    // 添加错误
    m_tip_errs[err] = tip;
    // 显示信息
    ShowTip(tip, false);
}

void RegisterDialog::DelTipErr(TipErr err)
{
    // 移除错误
    m_tip_errs.remove(err);
    if(m_tip_errs.empty()){
        ui->err_tip->clear();
        return;
    }
    // 显示第一个错误
    ShowTip(m_tip_errs.first(), false);
}

void RegisterDialog::on_confirm_btn_clicked()
{
    // 验证用户名
    if(!CheckUserValid()) return;
    // 验证邮箱
    if(!CheckEmailValid()) return;
    // 验证密码
    if(!CheckPasswordValid()) return;
    // 验证确认密码
    if(!CheckConfirmValid()) return;
    // 验证验证码
    if(!CheckVerifyCodeValid()) return;
    // 发送http请求注册用户
    QJsonObject jsonObj;
    jsonObj["user"] = ui->user_edit->text();
    jsonObj["email"] = ui->email_edit->text();
    // 密码加密发送
    jsonObj["password"] = Sha256Hash(ui->pass_edit->text());
    jsonObj["confirm"] = Sha256Hash(ui->confirm_edit->text());
    jsonObj["verifycode"] = ui->verify_edit->text();
    HttpManager::GetInstance().PostHttpReq(QUrl(gate_url_prefix + "/user_register"),
                                           jsonObj,
                                           ReqId::ID_REG_USER, Modules::REGISTER_MOD);
}

