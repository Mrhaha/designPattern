#include "tc_logger.h"
#include "tc_functor.h"
#include <iostream>
#include <string.h>

namespace taf
{
int TC_LoggerRoll::_iDyeingThreadId = 0;

void TC_LoggerRoll::setupThread(TC_LoggerThreadGroup *pThreadGroup)
{
    assert(pThreadGroup != NULL);

    unSetupThread();

    TC_LockT<TC_ThreadMutex> lock(_mutex);

    _pThreadGroup = pThreadGroup;

    TC_LoggerRollPtr self = this;

    _pThreadGroup->registerLogger(self);
}

void TC_LoggerRoll::unSetupThread()
{
    TC_LockT<TC_ThreadMutex> lock(_mutex);

    if(_pThreadGroup != NULL)
    {
        _pThreadGroup->flush();

        TC_LoggerRollPtr self = this;

        _pThreadGroup->unRegisterLogger(self);

        _pThreadGroup = NULL;
    }

    flush();
}

void TC_LoggerRoll::write(const pair<int, string> &buffer)
{
    if(_pThreadGroup)
    {
        _buffer.push_back(make_pair(_iDyeingThreadId, buffer.second));
    }
    else
    {
        //同步记录日志
        deque<pair<int, string> > ds;
        ds.push_back(make_pair(_iDyeingThreadId, buffer.second));
        roll(ds);
    }
}

void TC_LoggerRoll::flush()
{
    TC_ThreadQueue<pair<int, string> >::queue_type qt;
    _buffer.swap(qt);

    if(!qt.empty())
    {
        roll(qt);
    }
}

//////////////////////////////////////////////////////////////////
//
TC_LoggerThreadGroup::TC_LoggerThreadGroup() : _bTerminate(false)
{
}

TC_LoggerThreadGroup::~TC_LoggerThreadGroup()
{
    flush();

    _bTerminate = true;

    {
        Lock lock(*this);
        notifyAll();
    }

    _tpool.stop();
    _tpool.waitForAllDone();
}

void TC_LoggerThreadGroup::start(size_t iThreadNum)
{
    _tpool.init(iThreadNum);
    _tpool.start();

    TC_Functor<void> cmd(this, &TC_LoggerThreadGroup::run);
    TC_Functor<void>::wrapper_type wrapper(cmd);
    for(size_t i = 0; i < _tpool.getThreadNum(); i++)
    {
        _tpool.exec(wrapper);
    }
}

void TC_LoggerThreadGroup::registerLogger(TC_LoggerRollPtr &l)
{
    Lock lock(*this);

    _logger.insert(l);
}

void TC_LoggerThreadGroup::unRegisterLogger(TC_LoggerRollPtr &l)
{
    Lock lock(*this);

    _logger.erase(l);
}

void TC_LoggerThreadGroup::flush()
{
    logger_set logger;

    {
        Lock lock(*this);
        logger = _logger;
    }

    logger_set::iterator it = logger.begin();
    while(it != logger.end())
    {
        try
        {
            it->get()->flush();
        }
        catch(...)
        {
        }
        ++it;
    }
}

void TC_LoggerThreadGroup::run()
{
    while (!_bTerminate)
    {
        //100ms
        {
            Lock lock(*this);
            timedWait(100);
        }

        flush();
    }
}

//////////////////////////////////////////////////////////////////////////////////

LoggerBuffer::LoggerBuffer() : _buffer(NULL), _buffer_len(0)
{
}

LoggerBuffer::LoggerBuffer(TC_LoggerRollPtr roll, size_t buffer_len) : _roll(roll), _buffer(NULL), _buffer_len(buffer_len)
{
    //设置get buffer, 无效, 不适用
    setg(NULL, NULL, NULL);
    
    //设置put buffer
    if(_roll)
    {
        //分配空间
        _buffer = new char[_buffer_len]; 
        setp(_buffer, _buffer + _buffer_len);            
    }
    else
    {
        setp(NULL, NULL);
        _buffer_len = 0;
    }
}

LoggerBuffer::~LoggerBuffer()
{
	sync();
    if(_buffer)
    {
        delete[] _buffer;
    }
}

streamsize LoggerBuffer::xsputn(const char_type* s, streamsize n)
{
    if(!_roll)
    {
        return n;
    }

    return std::basic_streambuf<char>::xsputn(s, n);
}

void LoggerBuffer::reserve(std::streamsize n)
{
	if(n <= _buffer_len)
    {
		return;
    }

    //不超过最大大小
    if(n > MAX_BUFFER_LENGTH)
    {
        n = MAX_BUFFER_LENGTH;
    }

	int len = pptr() - pbase();
	char_type * p = new char_type[n];
	memcpy(p, _buffer, len);
	delete[] _buffer;
	_buffer     = p;
	_buffer_len = n;

	setp(_buffer, _buffer + _buffer_len);

	pbump(len);

    return;
}

std::basic_streambuf<char>::int_type LoggerBuffer::overflow(std::basic_streambuf<char>::int_type c)
{
    if(!_roll)
    {
        return 0;
    }

    if(_buffer_len >= MAX_BUFFER_LENGTH)
    {
        sync();
    }
    else
    {
        reserve(_buffer_len * 2);
    }

	if(std::char_traits<char_type>::eq_int_type(c,std::char_traits<char_type>::eof()) )
    {
		return std::char_traits<char_type>::not_eof(c);
	}
    else
    {
		return sputc(c);
	}
    return 0;
}

int LoggerBuffer::sync()
{
    //有数据
	if(pptr() > pbase())
    {
		std::streamsize len = pptr() - pbase();

        if(_roll)
        {
            //具体的写逻辑
            _roll->write(make_pair(0, string(pbase(), len)));
        }

        //重新设置put缓冲区, pptr()重置到pbase()处
		setp(pbase(), epptr());
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////
// 

}


