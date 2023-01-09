cmake_minimum_required(VERSION 3.22)
project(server)

set(CMAKE_CXX_STANDARD 17)
set ( 3rdParties_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../3rdParties )


# 3rdParty directories.
set (curl_PATH ${3rdParties_PATH}/curl/output)
set (pistache_PATH ${3rdParties_PATH}/pistache/output)
set (json_PATH ${3rdParties_PATH}/json/output)

add_executable(server
        includes/device.hpp
        includes/serv_include.hpp
        src/device.cpp
        src/thread_func.cpp
        server.cpp)


target_include_directories(server PUBLIC includes ${curl_PATH}/include ${pistache_PATH}/include  ${json_PATH}/include)
target_link_directories(server PUBLIC ${curl_PATH}/lib ${pistache_PATH}/lib)
target_link_libraries(server PUBLIC -lcurl -lpistache)
