#pragma once
#include "const.h"

class HttpConnection;
typedef std::function<void(std::shared_ptr<HttpConnection>)>HttpHandler;
class LogicSystem {
public:
    ~LogicSystem() = default;
    LogicSystem(const LogicSystem&) = delete;
    LogicSystem& operator=(const LogicSystem&) = delete;
    // 获取单例对象
    static LogicSystem& GetInstance();

    bool HandleGet(const std::string&, std::shared_ptr<HttpConnection>);
    // 注册get请求的处理函数
    void RegisterGet(const std::string&, const HttpHandler&);

private:
    LogicSystem();

    std::map<std::string, HttpHandler> m_post_handlers;
    std::map<std::string, HttpHandler> m_get_handlers;

};

