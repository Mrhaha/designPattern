cmake_minimum_required(VERSION 3.16)
project(designPatterns)

set(CMAKE_CXX_STANDARD 14)

ADD_EXECUTABLE(sockServer server/server.cpp)
ADD_EXECUTABLE(sockClient client/client.cpp)