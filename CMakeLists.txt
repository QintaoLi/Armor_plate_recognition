cmake_minimum_required(VERSION 3.10)

project(装甲板识别)

aux_source_directory(src SRC_LIST)
add_executable(myapp ${SRC_LIST})

find_package(OpenCV REQUIRED)
target_link_directories(myapp PUBLIC
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(myapp PUBLIC
    ${OpenCV_LIBS}
)

