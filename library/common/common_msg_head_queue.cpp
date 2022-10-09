//
// Created by lxhbroser on 2019/4/24.
//

#include "string.h"
#include "common_msg_head_queue.h"
#include "common_base.h"
#include "common_codeengine.h"

CMsgHeadQueue::CMsgHeadQueue()
{
    m_iPackageLen = 0;
    m_usCmd = 0;
}

CMsgHeadQueue::~CMsgHeadQueue()
{
    //do nothing
}

int32_t CMsgHeadQueue::encode(char* pszOut, int32_t& iOutLength)
{
    if (NULL == pszOut)
    {
        return fail;
    }

    char* ptmp = pszOut;
    int32_t coded_length = 0;

    iOutLength = 0;

    coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iPackageLen);
    iOutLength += coded_length;

    coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_usCmd);
    iOutLength += coded_length;

    return success;
}

int32_t CMsgHeadQueue::decode(const char* pszIn, int32_t iInLength)
{
    if (NULL == pszIn || iInLength <= 0)
    {
        return fail;
    }

    char* ptmp = const_cast<char*>(pszIn);
    int32_t remained_length = iInLength;
    int32_t decoded_length = 0;

    decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)(&m_iPackageLen));
    remained_length -= decoded_length;

    decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)(&m_usCmd));
    remained_length -= decoded_length;

    if (remained_length < 0)
    {
        return fail;
    }

    return success;
}
