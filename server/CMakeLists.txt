cmake_minimum_required(VERSION 3.22)
project(server)

set(CMAKE_CXX_STANDARD 17)

# Build 3rdParties.
set ( 3rdParties_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../3rdParties )
execute_process(COMMAND "./build.sh" WORKING_DIRECTORY ${3rdParties_PATH} RESULT_VARIABLE build_RESULT)
if(NOT ${build_RESULT} EQUAL "0")
	message( SEND_ERROR "Build returned with code " ${build_RESULT})
	message( FATAL_ERROR "Build of \"3rdParties\" was a failure." )
endif()

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
        
# set_property(TARGET server PROPERTY CXX_STANDARD 20)

target_include_directories(server PUBLIC includes ${curl_PATH}/include ${pistache_PATH}/include  ${json_PATH}/include)
target_link_directories(server PUBLIC ${curl_PATH}/lib ${pistache_PATH}/lib)
target_link_libraries(server PUBLIC -lcurl -lpistache)
