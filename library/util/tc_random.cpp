#include "tc_random.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>

namespace taf
{

static pthread_mutex_t stMutex = PTHREAD_MUTEX_INITIALIZER;
static int fd = -1;

void generateRandom(char* buffer, int size)
{
    ::pthread_mutex_lock(&stMutex);
    if(fd == -1)
    {
        fd = open("/dev/urandom", O_RDONLY);
        if(fd == -1)
        {
            ::pthread_mutex_unlock(&stMutex);
            assert(0);
        }
    }

    int reads = 0;
    size_t index = 0;    
    while(reads <= 20 && index != static_cast<size_t>(size))
    {
        ssize_t bytesRead = ::read(fd, buffer + index, static_cast<size_t>(size) - index);
        
        if(bytesRead == -1 && errno != EINTR)
        {
            ::pthread_mutex_unlock(&stMutex);
            assert(0);
        }
        else
        {
            index += bytesRead;
            reads++;
        }
    }
        
    if(index != static_cast<size_t>(size))
    {
        ::pthread_mutex_unlock(&stMutex);
        assert(0);
    }

    ::pthread_mutex_unlock(&stMutex);
}

unsigned int random(int limit)
{
    unsigned int r;

    generateRandom(reinterpret_cast<char*>(&r), static_cast<unsigned int>(sizeof(unsigned int)));

    if(limit > 0)
    {
        r = r % limit;
    }
    
    return r;
}

}
