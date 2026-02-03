# CMakeLists.txt

# 设置 CMake 最低版本要求
cmake_minimum_required(VERSION 3.10)

# 设置项目名称
project(reactor) # 你的项目名是 reactor

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- 关键设置：设置 Debug 模式 ---
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

set(CMAKE_EXPORT_COMPILE_COMMANDS True)

add_subdirectory(client)
add_subdirectory(http)
add_subdirectory(server)
add_subdirectory(test)