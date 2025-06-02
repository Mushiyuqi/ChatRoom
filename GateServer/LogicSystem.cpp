#include "LogicSystem.h"
#include "HttpConnection.h"
#include "VerifyGrpcClient.h"
#include "StatusGrpcClient.h"
#include "RedisManager.h"
#include "MysqlManager.h"

LogicSystem::LogicSystem() {
    // 服务测试
    RegisterGet("/get_test", [](std::shared_ptr<HttpConnection> connection) {
        beast::ostream(connection->m_response.body()) << "receive get_test req" << std::endl;
        int i = 0;
        for (auto& element : connection->m_get_params) {
            i++;
            beast::ostream(connection->m_response.body())
                << std::endl
                << "param " << i << " key  : " << element.first
                << std::endl
                << "param " << i << " value: " << element.second
                << std::endl << std::endl;
        }
    });
    // 获取验证码
    RegisterPost("/get_verifycode", [](std::shared_ptr<HttpConnection> connection) {
        auto body_str = beast::buffers_to_string(connection->m_request.body().data());
        std::cout << "LogicSystem::LogicSystem Url:/get_verifycode receive body is \n" << body_str << std::endl;
        // 设置返回数据类型
        connection->m_response.set(http::field::content_type, "text/json");
        // 解析json数据
        Json::Value root;
        Json::Reader reader;
        Json::Value src_root;
        // 读取json数据
        bool ret = reader.parse(body_str, src_root);
        // 判断 1.读取是否成功 2.是否有email字段
        if (!ret || !src_root.isMember("email")) {
            std::cout << "LogicSystem::LogicSystem Url:/get_verifycode Failed to prase Json data!" << std::endl;
            root["error"] = ErrorCodes::ErrorJson;
            std::string json_str = root.toStyledString();
            beast::ostream(connection->m_response.body()) << json_str;
            return true;
        }
        // 读取数据
        auto email = src_root["email"].asString();
        // 发送并获取验证码
        auto response = VerifyGrpcClient::GetInstance().GetVerifyCode(email);
        std::cout << "LogicSystem::LogicSystem Url:/get_verifycode VerfifyCode is " << response.code() << std::endl;
        // 设置返回数据
        root["error"] = response.error();
        root["email"] = email;
        std::string json_str = root.toStyledString();
        beast::ostream(connection->m_response.body()) << json_str;
        return true;
    });
    // 注册用户服务
    RegisterPost("/user_register", [](std::shared_ptr<HttpConnection> connection) {
        // 将body的数据转换成string
        auto bodyStr = boost::beast::buffers_to_string(connection->m_request.body().data());
        // 设置回包格式
        connection->m_response.set(http::field::content_type, "text/json");
        // 解析json数据
        Json::Value root;
        Json::Reader reader;
        Json::Value srcRoot;
        bool parseSuccess = reader.parse(bodyStr, srcRoot);
        if (!parseSuccess) {
            // 解析失败编辑回包
            std::cerr << "LogicSystem::LogicSystem Url:/user_register Failed to parse JSON data!" << std::endl;
            root["error"] = ErrorCodes::ErrorJson;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->m_response.body()) << jsonstr;
            return true;
        }
        // 1. 验证码是否过期
        std::string verifyCode;
        bool b_get_verify = RedisManager::GetInstance().Get(CodePrefix + srcRoot["email"].asString(), verifyCode);
        if (!b_get_verify) {
            // 验证码获取过期
            std::cerr << "LogicSystem::LogicSystem Url:/user_register get verify code expired" << std::endl;
            root["error"] = ErrorCodes::VerifyExpired;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->m_response.body()) << jsonstr;
            return true;
        }
        // 2. 验证码是否错误
        if (verifyCode != srcRoot["verifycode"].asString()) {
            // 验证码错误
            std::cerr << "LogicSystem::LogicSystem Url:/user_register verify code error" << std::endl;
            root["error"] = ErrorCodes::VerifyCodeErr;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->m_response.body()) << jsonstr;
            return true;
        }
        // 3. 用户是否存在 查找数据库判断用户是否存在
        static MysqlManager& mysql = MysqlManager::GetInstance();
        int uid = mysql.RegUser(srcRoot["user"].asString(), srcRoot["email"].asString(),
                                srcRoot["password"].asString());
        if (uid == -1 || uid == -2 || uid == -3) {
            // 注册用户错误
            std::cerr << "LogicSystem::LogicSystem Url:/user_register user or email exist" << std::endl;
            if (uid == -3)
                root["error"] = ErrorCodes::EmailUsed; // 邮箱已存在
            else if (uid == -2)
                root["error"] = ErrorCodes::UserExist; // 用户已存在
            else
                root["error"] = ErrorCodes::SqlFailed; // 数据库错误
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->m_response.body()) << jsonstr;
            return true;
        }

        root["error"] = ErrorCodes::Success;
        root["uid"] = uid;
        root["email"] = srcRoot["email"];
        root["user"] = srcRoot["user"].asString();
        root["password"] = srcRoot["password"].asString();
        root["confirm"] = srcRoot["confirm"].asString();
        root["verifycode"] = srcRoot["verifycode"].asString();
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->m_response.body()) << jsonstr;
        return true;
    });
    // 重置密码服务
    RegisterPost("/reset_pwd", [](std::shared_ptr<HttpConnection> connection) {
        // 将body的数据转换成string
        auto bodyStr = boost::beast::buffers_to_string(connection->m_request.body().data());
        // 设置回包格式
        connection->m_response.set(http::field::content_type, "text/json");
        // 解析json数据
        Json::Value root;
        Json::Reader reader;
        Json::Value srcRoot;
        bool parseSuccess = reader.parse(bodyStr, srcRoot);
        if (!parseSuccess) {
            // 解析失败编辑回包
            std::cerr << "LogicSystem::LogicSystem Url:/reset_pwd Failed to parse JSON data!" << std::endl;
            root["error"] = ErrorCodes::ErrorJson;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->m_response.body()) << jsonstr;
            return true;
        }

        // 1. 验证码是否过期
        std::string verifyCode;
        bool b_get_verify = RedisManager::GetInstance().Get(CodePrefix + srcRoot["email"].asString(), verifyCode);
        if (!b_get_verify) {
            // 验证码获取过期
            std::cerr << "LogicSystem::LogicSystem Url:/reset_pwd get verify code expired" << std::endl;
            root["error"] = ErrorCodes::VerifyExpired;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->m_response.body()) << jsonstr;
            return true;
        }
        // 2. 验证码是否错误
        if (verifyCode != srcRoot["verifycode"].asString()) {
            // 验证码错误
            std::cerr << "LogicSystem::LogicSystem Url:/reset_pwd verify code error" << std::endl;
            root["error"] = ErrorCodes::VerifyCodeErr;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->m_response.body()) << jsonstr;
            return true;
        }
        // 3. 判断用户名和邮箱是否匹配
        bool flag = MysqlManager::GetInstance().
            CheckEmail(srcRoot["user"].asString(), srcRoot["email"].asString());
        if (!flag) {
            std::cerr << "LogicSystem::LogicSystem Url:/reset_pwd user or email error" << std::endl;
            root["error"] = ErrorCodes::EmailNotMatch;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->m_response.body()) << jsonstr;
            return true;
        }

        // 4. 修改密码
        flag = MysqlManager::GetInstance().
            UpdatePwd(srcRoot["user"].asString(), srcRoot["password"].asString());
        if (!flag) {
            std::cerr << "LogicSystem::LogicSystem Url:/reset_pwd update password failed" << std::endl;
            root["error"] = ErrorCodes::PasswordUpFailed;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->m_response.body()) << jsonstr;
            return true;
        }

        root["error"] = ErrorCodes::Success;
        root["email"] = srcRoot["email"];
        root["user"] = srcRoot["user"].asString();
        root["password"] = srcRoot["password"].asString();
        root["verifycode"] = srcRoot["verifycode"].asString();
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->m_response.body()) << jsonstr;
        return true;
    });
    // 登陆服务
    RegisterPost("/user_login", [](std::shared_ptr<HttpConnection> connection) {
        // 将body的数据转换成string
        auto bodyStr = boost::beast::buffers_to_string(connection->m_request.body().data());
        // 设置回包格式
        connection->m_response.set(http::field::content_type, "text/json");
        // 解析json数据
        Json::Value root;
        Json::Reader reader;
        Json::Value srcRoot;
        bool parseSuccess = reader.parse(bodyStr, srcRoot);
        if (!parseSuccess) {
            // 解析失败编辑回包
            std::cerr << "LogicSystem::LogicSystem Url:/user_login Failed to parse JSON data!" << std::endl;
            root["error"] = ErrorCodes::ErrorJson;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->m_response.body()) << jsonstr;
            return true;
        }
        auto email = srcRoot["email"].asString();
        auto pwd = srcRoot["password"].asString();
        UserInfo userInfo;
        //查询数据库判断用户名和密码是否匹配
        bool pwd_valid = MysqlManager::GetInstance().CheckPassword(email, pwd, userInfo);
        if (!pwd_valid) {
            std::cerr << "LogicSystem::LogicSystem Url:/user_login user pwd not match" << std::endl;
            root["error"] = ErrorCodes::PasswordInvalid;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->m_response.body()) << jsonstr;
            return true;
        }

        //查询StatusServer找到合适的连接
        auto reply = StatusGrpcClient::GetInstance().GetChatServer(userInfo.uid);
        if (reply.error()) {
            std::cerr << "LogicSystem::LogicSystem Url:/user_login grpc get chat server failed, error is "
                << reply.error() << std::endl;
            root["error"] = ErrorCodes::RPCFailed;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->m_response.body()) << jsonstr;
            return true;
        }

        std::cout << "succeed to load userinfo uid is " << userInfo.uid << std::endl;
        root["error"] = 0;
        root["email"] = email;
        root["uid"] = userInfo.uid;
        root["token"] = reply.token();
        root["host"] = reply.host();
        root["port"] = reply.port();
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->m_response.body()) << jsonstr;
        return true;
    });
}

LogicSystem& LogicSystem::GetInstance() {
    static LogicSystem instance;
    return instance;
}

bool LogicSystem::HandleGet(const std::string& path, std::shared_ptr<HttpConnection> connection) {
    if (!m_get_handlers.contains(path)) {
        return false;
    }
    m_get_handlers[path](std::move(connection));
    return true;
}

bool LogicSystem::HandlePost(const std::string& path, std::shared_ptr<HttpConnection> connection) {
    if (!m_post_handlers.contains(path)) {
        return false;
    }
    m_post_handlers[path](std::move(connection));
    return true;
}

void LogicSystem::RegisterGet(const std::string& url, const HttpHandler& handler) {
    m_get_handlers.insert(std::make_pair(url, handler));
}

void LogicSystem::RegisterPost(const std::string& url, const HttpHandler& handler) {
    m_post_handlers.insert(std::make_pair(url, handler));
}

