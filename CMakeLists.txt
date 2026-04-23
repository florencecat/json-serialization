cmake_minimum_required(VERSION 3.20)
project(json CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(json_lib STATIC
    src/json/json.cpp
)
target_include_directories(json_lib PUBLIC
    include
)

add_executable(json_example
    src/main.cpp
)
target_link_libraries(json_example PRIVATE
    json_lib
)
