CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(EXEC connector_svr)

SET(EXECUTABLE_OUTPUT_PATH ${CONN_OUTPUT_PATH})

SET(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wno-unused-parameter -Woverloaded-virtual -Wpointer-arith -Wshadow -std=c++17 -pipe -fPIC -ggdb -gdwarf-2")

SET(LIB -static-libstdc++ common session_key)

AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} SRC)

INCLUDE_DIRECTORIES(${COMMON_INCLUDE_DIR})

ADD_EXECUTABLE(${EXEC} ${SRC})

TARGET_LINK_LIBRARIES(${EXEC} ${LIB})
