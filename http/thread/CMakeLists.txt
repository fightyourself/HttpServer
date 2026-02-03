add_library(thread STATIC
    ThreadPool.cpp
)
target_include_directories(thread PUBLIC ${CMAKE_CURRENT_LIST_DIR})
