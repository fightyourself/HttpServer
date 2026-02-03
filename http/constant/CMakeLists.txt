add_library(constant STATIC constants.cpp)

target_include_directories(constant PUBLIC ${CMAKE_CURRENT_LIST_DIR})
