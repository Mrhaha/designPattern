CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(EXEC queue_svr)

SET(EXECUTABLE_OUTPUT_PATH ${QUEUE_OUTPUT_PATH})

SET(LIB pthread rt dl -rdynamic -static-libstdc++ common_mt zookeeper util curl session_key)

AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} SRC)

CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/QueueAppPlatform.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/QueueAppPlatform.cpp" @ONLY)
LIST(APPEND SRC "${CMAKE_CURRENT_BINARY_DIR}/QueueAppPlatform.cpp")

SET(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wno-unused-parameter -Woverloaded-virtual -Wpointer-arith -Wshadow -std=c++17 -pipe -fPIC -g -ggdb -gdwarf-2 -D__MULTI_THREAD__")
add_definitions(-DQUEUE_BUILD_PLATFORM=${FIND_PLATFORM})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${COMMON_INCLUDE_DIR})

LINK_DIRECTORIES(${LIB_DIR})

ADD_EXECUTABLE(${EXEC} ${SRC})

TARGET_LINK_LIBRARIES(${EXEC} ${LIB})


