add_library(time STATIC TimeStamp.cpp)
target_include_directories(time PUBLIC ${CMAKE_CURRENT_LIST_DIR})
