cmake_minimum_required(VERSION 2.8)
project(robot_server)

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_STANDARD "11")

include_directories("./include")

add_executable(tcp_server "./src/tcpserver_main.cpp" "./src/tcpserver.cpp")
add_executable(udp_serve "./src/udpserver_main.cpp" "./src/udpserver.cpp")

find_package(Threads REQUIRED)

target_link_libraries(tcp_server Threads::Threads)
