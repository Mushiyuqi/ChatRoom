#include "LogicSystem.h"
#include "HttpConnection.h"

LogicSystem::LogicSystem() {
    RegisterGet("/get_test", [](std::shared_ptr<HttpConnection> connection) {
        beast::ostream(connection->m_response.body()) << "receive get_test req";
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

void LogicSystem::RegisterGet(const std::string& url, const HttpHandler& handler) {
    m_get_handlers.insert(std::make_pair(url, handler));
}

