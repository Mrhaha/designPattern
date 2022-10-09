CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(LIB_DIR
        /usr/local/msgpack/lib/
        /usr/local/mysql/lib/
        )

ADD_SUBDIRECTORY(auth_server)
ADD_SUBDIRECTORY(connector)
ADD_SUBDIRECTORY(cross_server)
ADD_SUBDIRECTORY(logic_server)
ADD_SUBDIRECTORY(queue_server)
