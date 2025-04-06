#pragma once
#include <QString>
#include <QUrl>
#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonDocument>
#include "global.h"

class HttpManager : public QObject, public std::enable_shared_from_this<HttpManager>{
    Q_OBJECT
public:
    ~HttpManager() = default;
    HttpManager(const HttpManager&) = delete;
    HttpManager& operator=(const HttpManager&) = delete;

    static HttpManager& GetInstance();

private:
    HttpManager();
    QNetworkAccessManager m_manager;
    // 发送post请求
    void PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod);

private slots:
    // 向不同模块分发http请求的结果
    void slot_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);

signals:
    // 完成http接收
    void sig_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);
    // 向注册模块发送信号
    void sig_reg_mod_finish(ReqId id, QString res, ErrorCodes err);
};

