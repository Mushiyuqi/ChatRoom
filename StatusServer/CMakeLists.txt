cmake_minimum_required(VERSION 3.27)
project(StatusServer)

set(CMAKE_CXX_STANDARD 20)

add_executable(StatusServer
        main.cpp
        ConfigManager.h
        ConfigManager.cpp
        config.ini
        MysqlManager.cpp
        MysqlDao.cpp
        MysqlDao.h
        const.h
        MysqlPool.cpp
        MysqlManager.h
        MysqlPool.h
        RedisConnPool.cpp
        RedisConnPool.h
        RedisManager.cpp
        RedisManager.h
        IOContextPool.cpp
        IOContextPool.h
        StatusServiceImpl.cpp
        StatusServiceImpl.h
        message.grpc.pb.cc
        message.grpc.pb.h
        message.pb.cc
        message.pb.h
        ChatGrpcClient.cpp
        ChatGrpcClient.h
        GRPCConnPool.hpp
        message.proto
)

# 将config.ini打包到构建目录方便调试
configure_file(
        "${CMAKE_SOURCE_DIR}/config.ini"
        "${CMAKE_BINARY_DIR}/config.ini"
        COPYONLY  # 纯拷贝，不替换变量
)

# 设置find package策略
cmake_policy(SET CMP0167 NEW)
# 添加依赖
find_package(Boost REQUIRED COMPONENTS filesystem system thread)
find_package(absl REQUIRED)
find_package(Protobuf REQUIRED)
target_link_libraries(StatusServer PRIVATE
        # 添加Boost库的依赖
        Boost::filesystem
        Boost::system
        Boost::thread
        Boost::boost
        # 添加Abseil库的依赖
        absl::log_internal_check_op
        absl::cord
        absl::strings
        absl::base
        absl::synchronization
        # 添加Protobuf库的依赖
        protobuf::libprotobuf
        protobuf::libprotoc
        protobuf::libprotobuf-lite
        # 添加grpc库的依赖
        libgpr.so
        libgrpc.so
        libgrpc++.so
        libgrpc++_reflection.so
        libaddress_sorting.so
        # 添加JSONCPP库的依赖
        jsoncpp.so
        # 添加hiredis库的依赖
        libhiredis.so
        # 添加redis-plus-plus库的依赖
        libredis++.a
        # 添加Mysql Connector C++库的依赖
        /usr/local/lib/libmysqlcppconn.so
        /usr/local/lib/libmysqlcppconnx.so
)