#!/usr/bin/env bash

# Copyright (c) 2015 by Kota Iguchi. All Rights Reserved.
# Licensed under the terms of the Apache Public License.
# Please see the LICENSE included with this distribution for details.

set -e

declare -rx VERBOSE=1
declare -r Daisy_ENABLE_TESTS="ON"
declare -r PLATFORM="macosx"
declare -r ARCHTECTURE="i386"
#declare -r PLATFORM="iphoneos"
#declare -r ARCHTECTURE="i386"
#declare -r PLATFORM="iphonesimulator"
#declare -r ARCHTECTURE="arm64"

cmd+="cmake -DARCH=${ARCHTECTURE} -DPLATFORM=${PLATFORM}"

declare -r CMAKE_BUILD_TYPE=Debug
declare -r BUILD_DIR=build.$(echo ${CMAKE_BUILD_TYPE} | tr '[:upper:]' '[:lower:]')
cmd+=" -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DDaisy_ENABLE_TESTS=${Daisy_ENABLE_TESTS}"
cmd+=" ../"
cmd+=" && make -j 4"

function echo_and_eval {
    local -r cmd="${1:?}"
    echo "${cmd}" && eval "${cmd}"
}

echo_and_eval "rm -rf \"${BUILD_DIR}\""
echo_and_eval "mkdir -p \"${BUILD_DIR}\""
echo_and_eval "pushd \"${BUILD_DIR}\""
echo_and_eval "${cmd}"

if [[ "${PLATFORM}" == "macosx" && "${Daisy_ENABLE_TESTS}" == "ON" ]]; then
    echo_and_eval "ctest -VV --output-on-failure"
fi

echo_and_eval "popd"
