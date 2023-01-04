cmake_minimum_required(VERSION 3.22)
project(api_server)

set(CMAKE_CXX_STANDARD 17)

set ( 3rdParties_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../3rdParties )
set (pistache_PATH ${3rdParties_PATH}/pistache/output)
set (json_PATH ${3rdParties_PATH}/json/output)

add_executable(api_server
        api.cpp)

target_include_directories(api_server PUBLIC ${pistache_PATH}/include  ${json_PATH}/include)
target_link_directories(api_server PUBLIC ${pistache_PATH}/lib)
target_link_libraries(api_server PUBLIC -lpistache)
