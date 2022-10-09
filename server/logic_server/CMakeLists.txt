CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wno-unused-parameter -Woverloaded-virtual -Wpointer-arith -Wshadow -std=c++17 -pipe -fPIC -ggdb -gdwarf-2 -ftemplate-depth=2048")
add_definitions(-DLOGIC_BUILD_PLATFORM=${FIND_PLATFORM})

ADD_SUBDIRECTORY(config)
ADD_SUBDIRECTORY(record)
ADD_SUBDIRECTORY(link)
ADD_SUBDIRECTORY(game)
ADD_SUBDIRECTORY(service)


