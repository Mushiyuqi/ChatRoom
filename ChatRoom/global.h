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
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>
#include <unordered_map>

/**
 * @brief repolish 刷新qss
 */
extern std::function<void(QWidget*)> repolish;
extern std::function<QString(QString)> Sha256Hash;  // 加密函数
extern QString gate_url_prefix;

enum ReqId{
    ID_GET_VERIFY_CODE = 1001, // 获取验证码
    ID_REG_USER = 1002, // 注册用户
    ID_RESET_PWD = 1003, //重置密码
    ID_LOGIN_USER = 1004, //用户登录
    ID_CHAT_LOGIN = 1005, //登陆聊天服务器
    ID_CHAT_LOGIN_RSP= 1006, //登陆聊天服务器回包

};

enum Modules{
    REGISTER_MOD = 0, // 注册模块
    RESET_MOD = 1,  // 重置密码模块
    LOGINMOD = 2,   // 登陆模块
};

enum ErrorCodes{
    SUCCESS = 0,
    ERR_JSON = 1, // json解析失败
    ERR_NETWORK = 2, // 网络错误
};

