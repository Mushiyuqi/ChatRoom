#include <iostream>
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
#include <boost/asio.hpp>
#include <csignal>
#include "message.grpc.pb.h"
#include "message.pb.h"
#include "GRPCConnPool.hpp"

extern "C" {
#include <hiredis/hiredis.h>
}

using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
using boost::asio::as_tuple;
namespace this_coro = boost::asio::this_coro;

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

#define RedisPoolSize 5
#define GRPCPoolSize 6
#define MysqlPoolSize 5
#define USERIPPREFIX  "uip_"
#define USERTOKENPREFIX  "utoken_"
#define IPCOUNTPREFIX  "ipcount_"
#define USER_BASE_INFO "ubaseinfo_"
#define LOGIN_COUNT  "logincount"
#define NAME_INFO  "nameinfo_"


// Tcp服务器需要的宏
#pragma once
#define MAX_MSG_LENGTH (1024*2)
#define HEAD_TOTAL_LEN 4
#define HEAD_ID_LEN 2
#define HEAD_DATA_LEN 2
#define MAX_RECVQUE 10000
#define MAX_SENDQUE 1000

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
    TokenInvalid = 1012, // Token无效
    UidInvalid = 1013, //uid无效
};

// 来自Qt客户端的请求Id
enum ReqId{
    ID_GET_VERIFY_CODE = 1001, // 获取验证码
    ID_REG_USER = 1002, // 注册用户
    ID_RESET_PWD = 1003, //重置密码
    ID_LOGIN_USER = 1004, //用户登录
    ID_CHAT_LOGIN = 1005, //登陆聊天服务器
    ID_SEARCH_USER = 1006, //用户搜索请求
    ID_ADD_FRIEND = 1007,  //添加好友申请
    ID_NOTIFY_ADD_FRIEND = 1008,  //通知用户添加好友申请
    ID_AUTH_FRIEND = 1009,  //认证好友请求
    ID_NOTIFY_AUTH_FRIEND = 1010, //通知用户认证好友申请
    ID_TEXT_CHAT_MSG  = 1011,  //文本聊天信息请求
    ID_NOTIFY_TEXT_CHAT_MSG = 1012, //通知用户文本聊天信息
};

// Defer类
class Defer {
public:
    // 接受一个lambda表达式或者函数指针
    explicit Defer(std::function<void()> func) : m_func(func) {}
    // 析构函数中执行传入的函数
    ~Defer() {m_func();}
private:
    std::function<void()> m_func;
};