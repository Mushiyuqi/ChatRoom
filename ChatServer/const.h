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

using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
using boost::asio::as_tuple;
namespace this_coro = boost::asio::this_coro;

#pragma once
#define MAX_MSG_LENGTH (1024*2)
#define HEAD_TOTAL_LEN 4
#define HEAD_ID_LEN 2
#define HEAD_DATA_LEN 2
#define MAX_RECVQUE 10000
#define MAX_SENDQUE 1000

enum MSG_IDS {
    MSG_HELLO_WORLD = 1001,
};
