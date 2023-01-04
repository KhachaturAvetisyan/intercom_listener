cmake_minimum_required(VERSION 3.22)
project(client)

set(CMAKE_CXX_STANDARD 17)

add_executable(client
        client_include.hpp
        client.cpp)
