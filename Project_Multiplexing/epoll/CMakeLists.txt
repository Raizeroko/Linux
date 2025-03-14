# 设置最低CMake版本要求
cmake_minimum_required(VERSION 3.10)

# 定义项目名称
project(EpollServer)

# 指定源文件
set(SOURCES Main.cc)

# 添加可执行文件
add_executable(server ${SOURCES})
