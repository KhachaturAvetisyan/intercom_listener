cmake_minimum_required(VERSION 3.22)
project(intercom_listener)

# Build 3rdParties.
set ( 3rdParties_PATH ${CMAKE_CURRENT_SOURCE_DIR}/3rdParties )

execute_process(COMMAND "./build.sh" WORKING_DIRECTORY ${3rdParties_PATH} RESULT_VARIABLE build_RESULT)
if(NOT ${build_RESULT} EQUAL "0")
    message( SEND_ERROR "Build returned with code : " ${build_RESULT})
    message( FATAL_ERROR "Build of \"3rdParties\" was a failure." )
endif()

add_subdirectory(server)
add_subdirectory(client)
add_subdirectory(api_server)
add_subdirectory(api_curl)