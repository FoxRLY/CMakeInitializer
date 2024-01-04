#!/bin/bash
if [[ "${1,,}" == "release" ]] || [[ "${1,,}" == "debug" ]]
then
    cmake -B ./build/${1,,} -DCMAKE_BUILD_TYPE=${1^^} -G Ninja && ninja -C ./build/${1,,} && ./build/${1,,}/cmake-init "${@:2}"
else
    echo "only release or debug"
fi;
