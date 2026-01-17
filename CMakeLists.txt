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

# --- 步骤1: 收集源文件 ---
# 假设你的主程序入口在 src/main.cpp
# 如果没有单独的 main.cpp，而是分散在 src/ 各处，则整个 src/ 应该构成一个库
# 这里我们假设 src/ 包含了所有非测试的源代码，可能包括 main 或作为库的一部分

# 方法一：如果 src/ 包含 main.cpp，你想构建一个主可执行文件
# aux_source_directory(${PROJECT_SOURCE_DIR}/src REACTOR_SOURCES)
# add_executable(reacotr_app ${REACTOR_SOURCES}) # 可以为主程序起个不同的名字
# target_include_directories(reacotr_app PRIVATE ${PROJECT_SOURCE_DIR}/include)

# 方法二：如果你的主要逻辑在 src/ 作为一个库，然后 main.cpp 和 test.cpp 都链接这个库
# 这种方法更常见于大型项目，尤其是当有多个可执行文件（如 app 和 test）时
aux_source_directory(${PROJECT_SOURCE_DIR}/src ALL_SRC_FILES)

# --- 步骤2: 创建库 ---
# 将 src/ 中的所有源文件打包成一个静态库 (STATIC LIBRARY)
add_library(reactor_lib STATIC ${ALL_SRC_FILES})
# 为这个库设置包含目录
target_include_directories(reactor_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)
# 使用 PUBLIC 表示任何链接到 reactor_lib 的目标也会继承这个包含目录

# --- 步骤3: 创建主可执行文件 (如果需要) ---
# 如果你有 src/main.cpp
# find_file(MAIN_CPP_FILE main.cpp PATHS ${PROJECT_SOURCE_DIR}/src NO_DEFAULT_PATH)
# if(MAIN_CPP_FILE)
#     add_executable(reactor_app ${MAIN_CPP_FILE})
#     target_link_libraries(reactor_app reactor_lib) # 链接到 reactor_lib
# endif()

# --- 步骤4: 创建测试可执行文件 ---
aux_source_directory(${PROJECT_SOURCE_DIR}/test TEST_SOURCES)

if(TEST_SOURCES)
    set(TEST_TARGET_NAME test_runner)

    add_executable(${TEST_TARGET_NAME} ${TEST_SOURCES})
    # 关键：链接 reactor_lib，这样 test.cpp 可以使用 src/ 中的代码
    target_link_libraries(${TEST_TARGET_NAME} reactor_lib)

    # 可选：如果 test.cpp 也需要直接包含 src/ 内部的头文件
    # target_include_directories(${TEST_TARGET_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
endif()