#！/bin/sh

set -e

SHELL_PATH=$(cd `dirname $0`; pwd)

BUILD=$1

PLATFORM=$(echo $1 | tr '[a-z]' '[A-Z]')

CMAKE_PATH=${SHELL_PATH}/../cmake-build-${BUILD}

mkdir -p ${CMAKE_PATH}
cd ${CMAKE_PATH}


cmake ../ -DBUILD_PLATFORM=$PLATFORM
make -j3 && make install
