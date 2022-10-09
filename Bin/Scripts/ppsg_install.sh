#!/bin/sh

if test "$1"; then
    export RUN_KING_BIN_PATH=${x}
    export RUN_KING_CODE_PATH=`pwd`

    echo "export RUN_KING_BIN_PATH=${x}" >> /etc/profile
    echo "export RUN_KING_CODE_PATH=`pwd`" >> /etc/profile
    source /etc/profile
else
    export RUN_KING_BIN_PATH=/usr/local/games
    export RUN_KING_CODE_PATH=`pwd`

    echo "export RUN_KING_BIN_PATH=/usr/local/games" >> /etc/profile
    echo "export RUN_KING_CODE_PATH=`pwd`" >> /etc/profile
    source /etc/profile
fi

