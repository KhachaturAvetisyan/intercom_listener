#!/bin/bash

function build_curl()
{
    pushd curl/
    mkdir -p build/ output/
    cd build/
    cmake -DCMAKE_INSTALL_PREFIX=`realpath ../output/` -DBUILD_TESTING=0 ..
    make -j`nproc` 
    make install 
    popd
}

function build_pistache()
{
    pushd pistache/
    mkdir -p build/ output/include/ output/lib/
    cd build/
    cmake ..
    make -j`nproc`
    cp -r ../include/* ../output/include/
    cp -r src/lib* ../output/lib/
    popd

}

function build_json()
{
    pushd json/
    mkdir -p build/ output/include 
    cd build/
    cmake -DJSON_BuildTests=0 -DCMAKE_INSTALL_PREFIX=`realpath ../output/` ..
    make -j`nproc`
    cp -r ../include/* ../output/include/
    popd
}

build_curl;
build_pistache;
build_json;
