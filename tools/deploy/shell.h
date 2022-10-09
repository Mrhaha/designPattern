//
// Created by hzl on 2019/5/8.
//

#pragma once


class shell
{
public:
    enum PIPE_FILE_DESCRIPTERS
    {
        READ_FD  = 0,
        WRITE_FD = 1
    };

    static void exec();
};


