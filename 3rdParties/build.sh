#!/bin/bash

function build_curl()
{
    pushd curl/
    mkdir build/ output/
    cd build/
    cmake -DCMAKE_INSTALL_PREFIX=`realpath ../output/` -DBUILD_TESTING=0 .. 
    make -j`nproc` 
    make install 
    popd
}

function build_pistache()
{
    pwd
    pushd pistache/
    mkdir -p build/ output/include/ output/lib/
    cd build/
    cmake ..
    make -j`nproc`
    cp -r include/* ../output/include/
    cp src/lib* ../output/lib/
    popd

}

build_curl;
build_pistache;
