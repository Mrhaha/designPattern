//
// Created by hzl on 2019/4/30.
//

#pragma once
#include "common_base.h"

template <class MessageHead>
int32_t encode_message_to_buffer(char *pszOut, int32_t &iOutLength, MessageHead &rstMsgHead)
{
    if (iOutLength <= 0 || nullptr == pszOut)
    {
        return fail;
    }

    char* pszTmp = pszOut;
    rstMsgHead.m_iPackageLen = MessageHead::SIZE;
    int32_t iCodedLength = MessageHead::SIZE;
    if (success != rstMsgHead.encode(pszTmp, iCodedLength))
    {
        return fail;
    }

    iOutLength = rstMsgHead.m_iPackageLen;
    return success;
}

template <class MessageHead, class MSGPACK_T>
int32_t encode_message_to_buffer(char *pszOut, int32_t &iOutLength, MessageHead &rstMsgHead, MSGPACK_T &stParam)
{
    if (iOutLength <= 0 || nullptr == pszOut)
    {
        return fail;
    }

    char* pszTmp = pszOut;

    msgpack::sbuffer stBuffer;
    msgpack::pack(stBuffer, stParam);
    memcpy(pszOut + MessageHead::SIZE, stBuffer.data(), stBuffer.size());

    pszTmp = pszOut;
    rstMsgHead.m_iPackageLen = MessageHead::SIZE + stBuffer.size();
    int32_t iCodedLength = MessageHead::SIZE;
    if (success != rstMsgHead.encode(pszTmp, iCodedLength))
    {
        return fail;
    }

    iOutLength = rstMsgHead.m_iPackageLen;
    return success;
}

template <class MessageHead, class MessageBody>
int32_t encode_message_to_buffer(char *pszOut, int32_t &iOutLength, MessageHead &rstMsgHead, MessageBody *pstMsgBody)
{
    if (iOutLength <= 0 || nullptr == pszOut)
    {
        return fail;
    }

    char* pszTmp = pszOut;
    int32_t iCodedLength = 0;

    if (nullptr != pstMsgBody)
    {
        pszTmp = pszOut + MessageHead::SIZE;
        iCodedLength = iOutLength - MessageHead::SIZE;

        if (success != pstMsgBody->encode(pszTmp, iCodedLength))
        {
            return fail;
        }
    }

    pszTmp = pszOut;
    rstMsgHead.m_iPackageLen = MessageHead::SIZE + iCodedLength;
    iCodedLength = MessageHead::SIZE;
    if (success != rstMsgHead.encode(pszTmp, iCodedLength))
    {
        return fail;
    }

    iOutLength = rstMsgHead.m_iPackageLen;
    return success;
}

template <class MessageHead>
int32_t encode_message_to_buffer(char *pszOut, int32_t &iOutLength, MessageHead &rstMsgHead, const char *pstBodyBuff,
                                 size_t uiBuffSize)
{
    if (iOutLength <= 0 || nullptr == pszOut)
    {
        return fail;
    }

    char* pszTmp = pszOut;
    int32_t iCodedLength = 0;

    if (nullptr != pstBodyBuff && uiBuffSize > 0)
    {
        pszTmp = pszOut + MessageHead::SIZE;
        iCodedLength = uiBuffSize;
        memcpy(pszTmp, pstBodyBuff, uiBuffSize);
    }

    pszTmp = pszOut;
    rstMsgHead.m_iPackageLen = MessageHead::SIZE + iCodedLength;
    iCodedLength = MessageHead::SIZE;
    if (success != rstMsgHead.encode(pszTmp, iCodedLength))
    {
        return fail;
    }

    iOutLength = rstMsgHead.m_iPackageLen;
    return success;
}

template <class MessageHead>
int32_t decode_buffer_to_message(const char *pszInCode, int32_t iInCodeLength, MessageHead &rstMsgHead,
                                 char **ppszMsgBody, int32_t &iBodyLength)
{
    if (nullptr == pszInCode || iInCodeLength < static_cast<int32_t>(MessageHead::SIZE) || nullptr == ppszMsgBody)
    {
        return fail;
    }

    char* pbyTemp = const_cast<char*>(pszInCode);
    const int32_t iTempSize = iInCodeLength;

    if (success != rstMsgHead.decode(pbyTemp, iTempSize))
    {
        return fail;
    }

    pbyTemp += MessageHead::SIZE;	//移到message body处
    iBodyLength = iTempSize - MessageHead::SIZE;

    if (iBodyLength > 0) //可以没有协议体
    {
        *ppszMsgBody = pbyTemp;
    }

    return success;
}
