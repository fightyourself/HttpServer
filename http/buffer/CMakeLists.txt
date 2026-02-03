add_library(buffer STATIC
    Buffer.cpp
)

target_include_directories(buffer PUBLIC ${CMAKE_CURRENT_LIST_DIR})
