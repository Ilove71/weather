cmake_minimum_required(VERSION 3.20)
project(test)

set(CMAKE_CXX_STANDARD 11)

find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

include_directories(${CURL_INCLUDE_DIRS})

add_executable(test src/main.cpp)
target_link_libraries(test ${CURL_LIBRARIES} nlohmann_json::nlohmann_json)