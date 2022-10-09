CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wno-unused-parameter -Woverloaded-virtual -Wpointer-arith -Wshadow -std=c++17 -pipe -fPIC -ggdb -gdwarf-2")

SET(LIB pthread rt dl -rdynamic -static-libstdc++ util common libmysqlclient.a)

SET(EXECUTABLE_OUTPUT_PATH ${TOOLS_OUTPUT_PATH})

SET(EXEC sql_alchemy)

AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} SRC)

INCLUDE_DIRECTORIES(${COMMON_INCLUDE_DIR})

LINK_DIRECTORIES(${LIB_DIR})

ADD_EXECUTABLE(${EXEC} ${SRC})

TARGET_LINK_LIBRARIES(${EXEC} ${LIB})
