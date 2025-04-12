#pragma once

#include <memory>
#include <iostream>
#include <mutex>
#include <QWidget>
#include <functional>
#include <QStyle>
#include <QRegularExpression>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QFile>
#include <QDir>
#include <QSettings>

/**
 * @brief repolish 刷新qss
 */
extern std::function<void(QWidget*)> repolish;

enum ReqId{
    ID_GET_VARIFY_CODE = 1001, // 获取验证码
    ID_REG_USER = 1002, // 注册用户
};

enum Modules{
    REGISTER_MOD = 0,
};

enum ErrorCodes{
    SUCCESS = 0,
    ERR_JSON = 1, // json解析失败
    ERR_NETWORK = 2, // 网络错误
};

extern QString gate_url_prefix;
