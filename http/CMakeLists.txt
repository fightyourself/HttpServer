

add_subdirectory(constant)
add_subdirectory(time)
add_subdirectory(tcp)
add_subdirectory(buffer)
add_subdirectory(thread)

add_library(httplib STATIC 
    Acceptor.cpp
    Connection.cpp
    EventLoop.cpp
    HttpParser.cpp
    HttpRequest.cpp
    HttpResponse.cpp
    HttpServer.cpp
    Router.cpp
    TcpServer.cpp
    Trie.cpp
)

target_include_directories(httplib PUBLIC include)
target_link_libraries(httplib PRIVATE constant)
target_link_libraries(httplib PUBLIC time epoll socket buffer thread)