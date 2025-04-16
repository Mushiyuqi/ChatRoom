cmake_minimum_required(VERSION 3.27)
project(GateServer)

set(CMAKE_CXX_STANDARD 20)

add_executable(GateServer main.cpp
        CServer.cpp
        CServer.h
        const.h
        config.ini
        HttpConnection.cpp
        HttpConnection.h
        LogicSystem.cpp
        LogicSystem.h
        message.proto
        message.grpc.pb.cc
        message.grpc.pb.h
        message.pb.cc
        message.pb.h
        VerifyGrpcClient.cpp
        VerifyGrpcClient.h
        ConfigManager.cpp
        ConfigManager.h
        IOContextPool.cpp
        IOContextPool.h)

# 将config.ini打包到构建目录方便调试
configure_file(
        "${CMAKE_SOURCE_DIR}/config.ini"
        "${CMAKE_BINARY_DIR}/config.ini"
        COPYONLY  # 纯拷贝，不替换变量
)

# 设置find package策略
cmake_policy(SET CMP0167 NEW)

# 添加Boost库的依赖
find_package(Boost REQUIRED COMPONENTS filesystem system thread)
target_link_libraries(GateServer PRIVATE
        Boost::filesystem
        Boost::system
        Boost::thread
        Boost::boost
)

# 添加Abseil库的依赖
find_package(absl REQUIRED)
target_link_libraries(GateServer PRIVATE
        absl::log_internal_check_op
        absl::cord
        absl::strings
        absl::base
        absl::synchronization
)

# 添加Protobuf库的依赖
find_package(Protobuf REQUIRED)
target_link_libraries(GateServer PRIVATE
        protobuf::libprotobuf
        protobuf::libprotoc
        protobuf::libprotobuf-lite
)

# 添加grpc库的依赖
target_link_libraries(GateServer PRIVATE
        libgpr.so
        libgrpc.so
        libgrpc++.so
        libgrpc++_reflection.so
        libaddress_sorting.so
)

# 添加JSONCPP库的依赖
target_link_libraries(GateServer PRIVATE jsoncpp.so)
