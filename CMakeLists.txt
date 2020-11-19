cmake_minimum_required(VERSION 2.8)
project(robot_server)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_STANDARD "11")

include_directories("./include")

add_executable(server "./src/server_main.cpp" "./src/tcpserver.cpp")

find_package(Threads REQUIRED)

target_link_libraries(server Threads::Threads)
