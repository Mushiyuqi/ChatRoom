#include "LogicSystem.h"
#include "HttpConnection.h"
#include "VerifyGrpcClient.h"

LogicSystem::LogicSystem() {
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
            std::cout << "LogicSystem::LogicSystem Url:/get_varifycode Failed to prase Json data!" << std::endl;
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

