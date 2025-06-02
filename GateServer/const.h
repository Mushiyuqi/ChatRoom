#pragma once
#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <iostream>
#include <map>
#include <queue>
#include <unordered_map>
#include <cctype>
#include <cassert>
#include <json/json.h>
#include <boost/filesystem.hpp>
#include <boost/operators.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "message.pb.h"
#include "GRPCConnPool.hpp"

extern "C" {
#include <hiredis/hiredis.h>
}

#define RedisPoolSize 5
#define GRPCPoolSize 6
#define MysqlPoolSize 5
#define CodePrefix "code_"

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http; // from <boost/beast/http.hpp>
namespace net = boost::asio; // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetVerifyReq;
using message::GetVerifyRsp;
using message::VerifyService;

using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::LoginReq;
using message::LoginRsp;
using message::StatusService;

enum ErrorCodes {
    Success = 0,
    ErrorJson = 1001, // Json 解析失败
    RPCFailed = 1002, //  RPC 请求失败
    VerifyExpired = 1003, // 验证码过期
    VerifyCodeErr = 1004, // 验证码错误
    SqlFailed = 1005, //  数据库操作失败
    UserExist = 1006, // 用户已存在
    EmailUsed = 1007, // 邮箱以被使用
    PasswordErr = 1008, // 密码错误
    EmailNotMatch = 1009, //  邮箱不匹配
    PasswordUpFailed = 1010, // 密码修改失败
    PasswordInvalid = 1011, // 密码不合法
};

// Defer类
class Defer {
public:
    // 接受一个lambda表达式或者函数指针
    explicit Defer(std::function<void()> func) : m_func(func) {}

    // 析构函数中执行传入的函数
    ~Defer() {
        m_func();
    }

private:
    std::function<void()> m_func;
};

