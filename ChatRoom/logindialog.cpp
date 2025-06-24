#include "logindialog.h"
#include "ui_logindialog.h"
#include "clickedlabel.h"
#include "httpmanager.h"
#include "tcpmanager.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    // 初始化头像
    InitHead();
    // 初始化回调函数
    InitHttpHandlers();

    connect(&HttpManager::GetInstance(), &HttpManager::sig_login_mod_finish,
            this, &LoginDialog::slot_login_mod_finish);
    connect(ui->reg_btn, &QPushButton::clicked, this, &LoginDialog::switchRegister);

    // 设置忘记密码按钮的状态
    ui->forget_label->SetState("normal", "hover", "", "selected", "selected_hover", "");
    connect(ui->forget_label, &ClickedLabel::clicked, this, &LoginDialog::slot_forget_pwd);

    // 连接tcp连接请求的信号和槽函数
    connect(this, &LoginDialog::sig_connect_tcp,
            &(TcpManager::GetInstance()), &TcpManager::slot_tcp_connect);
    // 断开tcp连接请求的信号和槽函数
    connect(this, &LoginDialog::sig_disconnect_tcp,
            &(TcpManager::GetInstance()), &TcpManager::slot_tcp_disconnect);
    // 连接tcp管理者发出的连接成功信号
    connect(&(TcpManager::GetInstance()), &TcpManager::sig_con_success,
            this, &LoginDialog::slot_tcp_con_finish);
    // 连接tcp管理者发出的登陆失败信号
    connect(&(TcpManager::GetInstance()), &TcpManager::sig_login_failed,
            this, &LoginDialog::slot_login_failed);
    // 连接tcp管理者请求发送数据的信号
    connect(this, &LoginDialog::sig_tcp_send_data,
            &(TcpManager::GetInstance()), &TcpManager::slot_tcp_send_data);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::InitHead()
{
    // 加载图片
    QPixmap originalPixmap(":/resource/head_0.png");
    originalPixmap = originalPixmap.scaled(ui->head_label->size(),
                                           Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // 创建一个和原始图片相同大小的QPixmap，用于绘制圆角图片
    QPixmap roundedPixmap(originalPixmap.size());
    roundedPixmap.fill(Qt::transparent); // 用透明色填充

    QPainter painter(&roundedPixmap);
    painter.setRenderHint(QPainter::Antialiasing);// 设置抗锯齿，使圆角更加平滑
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    // 使用QPainterPath设置圆角
    QPainterPath path;
    path.addRoundedRect(0,0,originalPixmap.width(), originalPixmap.height(), 15, 15);
    painter.setClipPath(path);

    // 将原始图片绘制到roundedPixmap上
    painter.drawPixmap(0, 0, originalPixmap);

    // 设置绘制好的圆角图片到QLabel上
    ui->head_label->setPixmap(roundedPixmap);
}

bool LoginDialog::CheckEmailValid()
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

bool LoginDialog::CheckPassValid()
{
    auto password = ui->pass_edit->text();

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

void LoginDialog::AddTipErr(TipErr e, QString tips)
{
    m_tip_errs[e] = tips;
    ShowTip(tips, false);
}

void LoginDialog::DelTipErr(TipErr e)
{
    m_tip_errs.remove(e);
    if(m_tip_errs.empty()){
        ui->err_tip->clear();
        return;
    }

    ShowTip(m_tip_errs.first(), false);
}

void LoginDialog::ShowTip(QString str, bool state)
{
    if(state){
        ui->err_tip->setProperty("state", "normal");
    }else{
        ui->err_tip->setProperty("state", "err");
    }

    ui->err_tip->setText(str);

    repolish(ui->err_tip);
}

void LoginDialog::EnableBtn(bool state)
{
    // 设置按钮
    ui->login_btn->setEnabled(state);
    ui->reg_btn->setEnabled(state);
    ui->forget_label->setEnabled(state);
    return;
}

void LoginDialog::InitHttpHandlers()
{
    m_handlers.insert(ReqId::ID_LOGIN_USER, [this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            ShowTip(tr("参数错误"),false);
            // 恢复按钮状态
            EnableBtn(true);
            return;
        }
        auto email = jsonObj["email"].toString();

        // 发送信号通知tcpMgr发送长链接
        ServerInfo si;
        si.Uid = jsonObj["uid"].toInt();
        si.Host = jsonObj["host"].toString();
        si.Port = jsonObj["port"].toString();
        si.Token = jsonObj["token"].toString();

        // 缓存数据
        m_tmp_info = si;

        qDebug() << "LoginDialog::InitHttpHandlers ReqId::ID_LOGIN_USER : "
                 << "email is " << email
                 << " uid is " << si.Uid
                 << " host is "<< si.Host
                 << " Port is " << si.Port
                 << " Token is " << si.Token;

        emit sig_connect_tcp(si);
    });
}

void LoginDialog::slot_forget_pwd()
{
    emit switchReset();
}

void LoginDialog::on_login_btn_clicked()
{
    if(CheckEmailValid() == false) return;
    if(CheckPassValid() == false) return;

    // 设置相关按钮的状态，防止多次点击
    EnableBtn(false);

    auto email = ui->email_edit->text();
    auto pwd = ui->pass_edit->text();

    // 发送http登陆请求
    QJsonObject jsonObj;
    jsonObj["email"] = email;
    jsonObj["pwd"] = Sha256Hash(pwd);
    HttpManager::GetInstance().PostHttpReq(QUrl(gate_url_prefix+"/user_login"), jsonObj, ReqId::ID_LOGIN_USER, Modules::LOGINMOD);
}

void LoginDialog::slot_login_mod_finish(ReqId id, QString res, ErrorCodes err)
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

void LoginDialog::slot_tcp_con_finish(bool state)
{
    if(state){
        ShowTip(tr("正在登陆......"), true);
        QJsonObject jsonObj;
        jsonObj["uid"] = m_tmp_info.Uid;
        jsonObj["token"] = m_tmp_info.Token;

        QJsonDocument jsonDoc(jsonObj);
        QString jsonString = jsonDoc.toJson();

        //发送tcp请求给chat server
        emit sig_tcp_send_data(ReqId::ID_CHAT_LOGIN, jsonString);
    }else{
        ShowTip(tr("网络异常......"), false);
        EnableBtn(true);
    }
}

void LoginDialog::slot_login_failed(int error)
{
    // 显示错误提示
    QString result = QString("登录失败, err is %1").arg(error);
    ShowTip(result,false);
    // 关闭tcp连接
    emit sig_disconnect_tcp();
    // 恢复按钮
    EnableBtn(true);
}

