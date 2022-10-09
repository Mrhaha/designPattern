#pragma once

#include <stdexcept>
using namespace std;

namespace taf
{

/**
* 异常类
*/
class TC_Exception : public exception
{
public:
    /**
     * 构造
     * @param buffer
     *
     * @return explicit
     */
	explicit TC_Exception(const string &buffer);

    /**
     * 构造
     * @param buffer
     * @param err, 错误码, 可用strerror获取错误信息
     */
	TC_Exception(const string &buffer, int err);

    /**
     * 析够
     */
    virtual ~TC_Exception() noexcept;

    /**
     * 错误信息
     *
     * @return const char*
     */
    virtual const char* what() const noexcept;

    /**
     * 
     * 
     * @return int
     */
    int getErrCode() { return _code; }

private:
    void getBacktrace();

private:
    string  _buffer;
    int     _code;

};

}
