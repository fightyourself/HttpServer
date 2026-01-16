cmake_minimum_required(VERSION 3.22)

project(http_server CXX)

include_directories(include)
aux_source_directory(./src SRC)


add_executable(server ${SRC})
