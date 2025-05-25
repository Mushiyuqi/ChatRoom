#include "resetdialog.h"
#include "ui_resetdialog.h"
#include "httpmanager.h"

ResetDialog::ResetDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ResetDialog)
{
    ui->setupUi(this);
    ui->err_tip->setProperty("state", "normal");
    repolish(ui->err_tip);

    connect(ui->user_edit,&QLineEdit::editingFinished,this,[this](){
        CheckUserValid();
    });

    connect(ui->email_edit, &QLineEdit::editingFinished, this, [this](){
        CheckEmailValid();
    });

    connect(ui->pwd_edit, &QLineEdit::editingFinished, this, [this](){
        CheckPassValid();
    });


    connect(ui->verify_edit, &QLineEdit::editingFinished, this, [this](){
        CheckVerifyValid();
    });

    //连接reset相关信号和注册处理回调
    InitHandlers();
    connect(&HttpManager::GetInstance(), &HttpManager::sig_reset_mod_finish, this,
            &ResetDialog::slot_reset_mod_finish);
}

ResetDialog::~ResetDialog()
{
    delete ui;
}

bool ResetDialog::CheckUserValid()
{
    if(ui->user_edit->text().isEmpty()){
        AddTipErr(TipErr::TIP_USER_ERR, tr("用户名不能为空"));
        return false;
    }
    DelTipErr(TipErr::TIP_USER_ERR);
    return true;
}

bool ResetDialog::CheckPassValid()
{
    auto password = ui->pwd_edit->text();

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

    return true;
}

bool ResetDialog::CheckEmailValid()
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

bool ResetDialog::CheckVerifyValid()
{
    auto code = ui->verify_edit->text();
    if(code.isEmpty()){
        AddTipErr(TipErr::TIP_VERIFY_ERR, tr("验证码不能为空"));
        return false;
    }
    DelTipErr(TipErr::TIP_VERIFY_ERR);
    return true;
}

void ResetDialog::AddTipErr(TipErr te, QString tips)
{
    m_tip_errs[te] = tips;
    ShowTip(tips, false);
}

void ResetDialog::DelTipErr(TipErr te)
{
    m_tip_errs.remove(te);
    if(m_tip_errs.empty()){
        ui->err_tip->clear();
        return;
    }

    ShowTip(m_tip_errs.first(), false);

}

void ResetDialog::ShowTip(QString str, bool b_ok)
{
    if(b_ok){
        ui->err_tip->setProperty("state","normal");
    }else{
        ui->err_tip->setProperty("state","err");
    }

    ui->err_tip->setText(str);

    repolish(ui->err_tip);
}

void ResetDialog::InitHandlers()
{
    //注册获取验证码回包逻辑
    m_handlers.insert(ReqId::ID_GET_VERIFY_CODE, [this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            ShowTip(tr("参数错误"),false);
            return;
        }
        auto email = jsonObj["email"].toString();
        ShowTip(tr("验证码已发送到邮箱，注意查收"), true);
        qDebug()<< "email is " << email ;
    });

    //注册重置密码回包逻辑
    m_handlers.insert(ReqId::ID_RESET_PWD, [this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            ShowTip(tr("参数错误"),false);
            return;
        }
        auto email = jsonObj["email"].toString();
        ShowTip(tr("重置成功,点击返回登录"), true);
        qDebug()<< "email is " << email ;
        qDebug()<< "user uuid is " <<  jsonObj["uuid"].toString();
    });

}

void ResetDialog::on_verify_btn_clicked()
{
    auto email = ui->email_edit->text();
    auto bcheck = CheckEmailValid();
    if(!bcheck){
        return;
    }

    //发送http请求获取验证码
    QJsonObject jsonObject;
    jsonObject["email"] = email;
    HttpManager::GetInstance().PostHttpReq(QUrl(gate_url_prefix+"/get_verifycode"),
                                        jsonObject, ReqId::ID_GET_VERIFY_CODE,Modules::RESET_MOD);

}

void ResetDialog::on_return_btn_clicked()
{
    emit switchLogin();
}


void ResetDialog::on_sure_btn_clicked()
{
    bool valid = CheckUserValid();
    if(!valid){
        return;
    }

    valid = CheckEmailValid();
    if(!valid){
        return;
    }

    valid = CheckPassValid();
    if(!valid){
        return;
    }

    valid = CheckVerifyValid();
    if(!valid){
        return;
    }

    //发送http重置用户请求
    QJsonObject json_obj;
    json_obj["user"] = ui->user_edit->text();
    json_obj["email"] = ui->email_edit->text();
    json_obj["password"] = Sha256Hash(ui->pwd_edit->text());
    json_obj["verifycode"] = ui->verify_edit->text();
    HttpManager::GetInstance().PostHttpReq(QUrl(gate_url_prefix + "/reset_pwd"),
                                        json_obj, ReqId::ID_RESET_PWD,Modules::RESET_MOD);
}

void ResetDialog::slot_reset_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    if(err != ErrorCodes::SUCCESS){
        ShowTip(tr("网络请求错误"),false);
        return;
    }

    // 解析 JSON 字符串,res需转化为QByteArray
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    //json解析错误
    if(jsonDoc.isNull()){
        ShowTip(tr("json解析错误"),false);
        return;
    }

    //json解析错误
    if(!jsonDoc.isObject()){
        ShowTip(tr("json解析错误"),false);
        return;
    }

    //调用对应的逻辑,根据id回调。
    m_handlers[id](jsonDoc.object());

    return;
}

