aux_source_directory(src SRC)

add_library(httplib STATIC ${SRC})

target_include_directories(httplib PUBLIC include)