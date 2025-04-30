#include "LogicSystem.h"
#include "HttpConnection.h"
#include "VerifyGrpcClient.h"
#include "RedisManager.h"

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
        // 3. 用户是否存在 查找数据库判断用户是否存在 todo

        root["error"] = ErrorCodes::Success;
        root["email"] = srcRoot["email"];
        root["user"] = srcRoot["user"].asString();
        root["password"] = srcRoot["password"].asString();
        root["confirm"] = srcRoot["confirm"].asString();
        root["verifycode"] = srcRoot["verifycode"].asString();
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

