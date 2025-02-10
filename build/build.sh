#!/bin/bash

BUILD_PATH=$(cd `dirname $0`; pwd)
echo "BUILD_PATH:${BUILD_PATH}"

export ROOT_PATH=$(cd `dirname ${BUILD_PATH}`; pwd)
echo "ROOT_PATH:${ROOT_PATH}"

BIN_PATH=${ROOT_PATH}/bin
rm -rf ${BIN_PATH}/*

cd ${BUILD_PATH}
make clean
cmake ..
make
make install