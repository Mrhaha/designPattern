CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(EXEC cross_svr)

SET(EXECUTABLE_OUTPUT_PATH ${CROSS_OUTPUT_PATH})

SET(LIB pthread rt dl -rdynamic -static-libstdc++ common_mt zookeeper util libmysqlclient.a curl)

AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} SRC)

CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/AppPlatform.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/AppPlatform.cpp" @ONLY)
LIST(APPEND SRC "${CMAKE_CURRENT_BINARY_DIR}/AppPlatform.cpp")

SET(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wno-unused-parameter -Woverloaded-virtual -Wpointer-arith -Wshadow -std=c++17 -pipe -fPIC -g -ggdb -gdwarf-2 -D__MULTI_THREAD__")
add_definitions(-DCROSS_BUILD_PLATFORM=${FIND_PLATFORM})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${COMMON_INCLUDE_DIR})

LINK_DIRECTORIES(${LIB_DIR})


ADD_EXECUTABLE(${EXEC} ${SRC})

TARGET_LINK_LIBRARIES(${EXEC} ${LIB})


