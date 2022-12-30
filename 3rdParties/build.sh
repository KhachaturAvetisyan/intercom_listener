#!/bin/bash

function build_curl()
{
    cd curl/
    mkdir build/ output/
    pushd build/
    cmake -DCMAKE_INSTALL_PREFIX=`realpath ../output/` -DBUILD_TESTING=0 .. || echo PIZDEEEEC
    make -j`nproc` || echo PIZDEEEEEEEEEEEEEEEEEEEEEEEEEEEEC
    make install || echo JA PIERDOLE KURWAAAAAAAAAAAAAA
    popd
}

build_curl;
