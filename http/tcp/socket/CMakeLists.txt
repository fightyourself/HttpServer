add_library(socket STATIC
    Socket.cpp
    InetAddress.cpp
)

target_include_directories(socket PUBLIC ${CMAKE_CURRENT_LIST_DIR})
