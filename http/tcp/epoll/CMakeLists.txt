add_library(epoll STATIC
    Channel.cpp
    Epoll.cpp
)
target_include_directories(epoll PUBLIC ${CMAKE_CURRENT_LIST_DIR} ../../)
target_link_libraries(epoll PRIVATE constant)
