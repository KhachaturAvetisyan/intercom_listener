cmake_minimum_required(VERSION 3.22)
project(api_curl)

set(CMAKE_CXX_STANDARD 20)

set ( 3rdParties_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../3rdParties )

set (json_PATH ${3rdParties_PATH}/json/output)
set (curl_PATH ${3rdParties_PATH}/curl/output)

add_executable(api_curl
        api_curl.cpp)

target_include_directories(api_curl PUBLIC ${curl_PATH}/include  ${json_PATH}/include)
target_link_directories(api_curl PUBLIC ${curl_PATH}/lib)
target_link_libraries(api_curl PUBLIC -lcurl)
