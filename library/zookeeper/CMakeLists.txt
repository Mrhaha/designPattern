CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

INCLUDE_DIRECTORIES("../../include/zookeeper")
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} SRC)

SET(CMAKE_C_FLAGS "-Wall -Wextra -Werror -Wno-unused-parameter -Woverloaded-virtual -Wpointer-arith -pipe -fPIC -ggdb -gdwarf-2 -Wno-deprecated -Wno-sign-compare")

ADD_LIBRARY(zookeeper ${SRC})
