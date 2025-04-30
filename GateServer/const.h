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

extern "C" {
#include <hiredis/hiredis.h>
}

#define RedisPoolSize 5
#define GRPCPoolSize 6

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

enum ErrorCodes {
    Success = 0,
    ErrorJson = 1001, // Json 解析失败
    RPCFailed = 1002, //  RPC 请求失败
    VerifyExpired = 1003, // 验证码过期
    VerifyCodeErr = 1004, // 验证码错误
    UserExist = 1005, // 用户已存在
    PasswordErr = 1006, // 密码错误
    EmailNotMatch = 1007, //  邮箱不匹配
    PasswordUpFailed = 1008, // 密码修改失败
    PasswordInvalid = 1009, // 密码不合法
};

#define CodePrefix "code_"
