CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(INCLUDE_DIR  
    ../../include/common
    /usr/include/
    /usr/local/include/ 
    /usr/local/msgpack/include/
)

INCLUDE_DIRECTORIES(${INCLUDE_DIR})

AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} SRC)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wno-unused-parameter -Woverloaded-virtual -Wpointer-arith -Wshadow -std=c++17 -pipe -fPIC -ggdb -gdwarf-2")

IF (${MULTI_THREAD} STREQUAL "ON")
    ADD_DEFINITIONS(-D__MULTI_THREAD__)
    ADD_LIBRARY(common_mt ${SRC})
ELSE() 
    ADD_LIBRARY(common ${SRC})
ENDIF(${MULTI_THREAD} STREQUAL "ON")

