//
// Created by hzl on 2019/5/8.
//

#include <unistd.h>
#include <cstdint>
#include <cassert>
#include <iostream>
#include "shell.h"

#define ASSERT_NOT(a, b) assert((a) != (b))
#define ASSERT_IS(a, b) assert((a) == (b))

void shell::exec()
{
    int32_t parentToChild[2];
    int32_t childToParent[2];

    ASSERT_IS(0, pipe(parentToChild));
    ASSERT_IS(0, pipe(childToParent));

    pid_t pid;
    if ((pid = fork()) == -1)
    {
        std::cout << "fork fail" << std::endl;
        return;
    }
    else if (pid == 0)
    {//child
        ASSERT_NOT(-1, dup2(parentToChild[READ_FD], STDIN_FILENO));
        ASSERT_NOT(-1, dup2(childToParent[WRITE_FD], STDOUT_FILENO));
        ASSERT_NOT(-1, dup2(childToParent[WRITE_FD], STDERR_FILENO));
        ASSERT_IS(0, close(parentToChild [WRITE_FD]));
        ASSERT_IS(0, close(childToParent [READ_FD]));

        execlp("cd", "cd", "/data/asdfas", "2>&1", nullptr);
        std::cout << "This line should never be reached!!!" << std::endl;
        exit(-1);
    }
    else
    {//parent
        std::cout << "Child " << pid << " process running..." << std::endl;

        ASSERT_IS(0, close(parentToChild [READ_FD]));
        ASSERT_IS(0, close(childToParent [WRITE_FD]));

        std::string    dataReadFromChild;
        constexpr auto BUFFER_SIZE = 8096;
        char      buffer[BUFFER_SIZE + 1];
        ssize_t   readResult;

        while (true)
        {
            fd_set          readfds;
            struct timeval  timeout{};

            timeout.tv_sec  = 0;    /* Seconds */
            timeout.tv_usec = 1000; /* Microseconds */

            FD_ZERO(&readfds);
            FD_SET(childToParent[READ_FD], &readfds);

            switch (select (1 + childToParent[READ_FD], &readfds,  nullptr,  nullptr, & timeout))
            {
                case 0: /* Timeout expired */
                    break;

                case -1:
                    if ((errno == EINTR) || (errno == EAGAIN))
                    {
                        errno = 0;
                        break;
                    }
                    else
                    {
                        std::cout << "select fail" << std::endl;
                        exit(-1);
                    }

                case 1:  /* We have input */
                    readResult = read(childToParent[READ_FD], buffer, BUFFER_SIZE);
                    if (readResult > 0)
                    {
                        // However you want to handle it...
                        std::cout << std::string(buffer, readResult) ;
                        break;
                    }
                    return ;


                default:
                    std::cout << "How did we see input on more than one file descriptor?" << std::endl;
                    exit(-1);
            }
        }
    }


}
