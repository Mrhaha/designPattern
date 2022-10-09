CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(INCLUDE_DIR
    ${LOGIC_SOURCE_DIR}/service
)

SET(LIB pthread rt dl
        -rdynamic
        -static-libstdc++
        "-Wl,--whole-archive" #之后的库使用--whole-archive选项
        logic_game
        logic_config
        logic_record
        logic_link
        "-Wl,--no-whole-archive" #之后的库不使用--whole-archive选项
        common
        zookeeper
        util
        libmysqlclient.a
)

SET(EXECUTABLE_OUTPUT_PATH ${LOGIC_OUTPUT_PATH})

SET(EXEC logic_svr)

INCLUDE_DIRECTORIES(${INCLUDE_DIR} ${COMMON_INCLUDE_DIR})

AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} SRC)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/logic_service_platform.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/logicserver_platform.cpp" @ONLY)
LIST(APPEND SRC "${CMAKE_CURRENT_BINARY_DIR}/logicserver_platform.cpp")

LINK_DIRECTORIES(${LIB_DIR})

ADD_EXECUTABLE(${EXEC} ${SRC})

TARGET_LINK_LIBRARIES(${EXEC} ${LIB})
