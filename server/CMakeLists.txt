cmake_minimum_required(VERSION 3.22)
project(server)

set(CMAKE_CXX_STANDARD 14)

# Build 3rdParties.
set ( 3rdParties_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../3rdParties )
execute_process(COMMAND "./build.sh" WORKING_DIRECTORY ${3rdParties_PATH} RESULT_VARIABLE build_RESULT)
if(NOT ${build_RESULT} EQUAL "0")
	message( SEND_ERROR "Build returned with code " ${build_RESULT})
	message( FATAL_ERROR "Build of \"3rdParties\" was a failure." )
endif()

# 3rdParty directories.
set (curl_PATH ${3rdParties_PATH}/curl/output)

include_directories(includes)

add_executable(server
        includes/device.hpp
        includes/serv_include.hpp
        src/device.cpp
        src/thread_func.cpp
        server.cpp)

target_link_directories(server PUBLIC ${curl_PATH}/lib)
#target_link_directories(server PUBLIC ${curl_PATH}/lib)

target_link_libraries(server PUBLIC -lcurl)
