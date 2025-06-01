#include "httpmanager.h"

HttpManager::~HttpManager()
{
    qDebug() << "HttpManager::~HttpManager destructed" ;
}

HttpManager& HttpManager::GetInstance() {
    static HttpManager instance;
    return instance;
}

HttpManager::HttpManager() {
    // 绑定信号与槽
    connect(this, &HttpManager::sig_http_finish, this, &HttpManager::slot_http_finish);
    qDebug() << "HttpManager::HttpManager constructed" ;
}

void HttpManager::PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod)
{
    // 把json对象转变为json字符串
    QByteArray data = QJsonDocument(json).toJson();
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data.length()));

    // 静态成员变量程序结束时释放 做闭包
    // std::shared_ptr<HttpManager> self = shared_from_this();

    // 异步发送post请求, reply指针需要自己回收
    QNetworkReply* reply = m_manager.post(request, data);
    QObject::connect(reply, &QNetworkReply::finished, [this, reply, req_id, mod](){
        // 处理错误情况
        if(reply->error() != QNetworkReply::NoError){
            qDebug() << "HttpManager::PostHttpReq ERROR " << reply->errorString();
            // 发送信号通知完成
            emit sig_http_finish(req_id, "", ErrorCodes::ERR_NETWORK, mod);
            // 回收reply, 事件循环结束自动回收
            reply->deleteLater();

            return;
        }
        // 无错误
        QString res = reply->readAll();
        // 发送信号通知完成
        emit sig_http_finish(req_id, res, ErrorCodes::SUCCESS, mod);
        reply->deleteLater();
        return;
    });

}

void HttpManager::slot_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod)
{
    if(mod == Modules::REGISTER_MOD){
        // 通知注册模块
        emit sig_reg_mod_finish(id, res, err);
    }
    if(mod == Modules::RESET_MOD){
        // 通知注册模块
        emit sig_reset_mod_finish(id, res, err);
    }
    if(mod == Modules::LOGINMOD){
        // 通知登陆模块
        emit sig_login_mod_finish(id, res, err);
    }
}
