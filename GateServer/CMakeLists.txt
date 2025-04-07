cmake_minimum_required(VERSION 3.27)
project(GateServer)

set(CMAKE_CXX_STANDARD 20)

add_executable(GateServer main.cpp
        CServer.cpp
        CServer.h
        const.h
        HttpConnection.cpp
        HttpConnection.h
        LogicSystem.cpp
        LogicSystem.h)

# 设置find package策略
cmake_policy(SET CMP0167 NEW)

# 添加Boost库的依赖
find_package(Boost REQUIRED COMPONENTS system filesystem thread)
target_include_directories(GateServer PRIVATE ${Boost_INCLUDE_DIRS})
target_link_directories(GateServer PRIVATE
        Boost::system
        Boost::filesystem
        Boost::thread
        Boost::beast)

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
