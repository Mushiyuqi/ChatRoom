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
#include <hiredis/hiredis.h>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;
using message::GetVerifyReq;
using message::GetVerifyRsp;
using message::VerifyService;

enum ErrorCodes {
    Success = 0,
    ErrorJson = 1001,
    RPCFailed = 1002,
};
