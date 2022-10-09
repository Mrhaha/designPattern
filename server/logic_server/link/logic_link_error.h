#pragma once

#include <string>
#include "common_platform.h"

class CLogicError
{
public:
    static __FORCE_INLINE__ void SetErrCode(int iErrCode, const char* pErrMsg)
    {
        if(pErrMsg)
        {
            m_iErrCode = iErrCode;
            m_strErrMsg = pErrMsg;
        }
    }
    
    static __FORCE_INLINE__ int GetErrCode()
    {
        return (m_iErrCode);
    }

    static __FORCE_INLINE__ const std::string& GetErrMsg()
    {
        return (m_strErrMsg);
    }
    
    static __FORCE_INLINE__ void AppendErrMsg(const char* szErrMsg)
    {
        m_strErrMsg.append(szErrMsg);
    }

    static __FORCE_INLINE__ void AppendErrMsg(const std::string& strErrMsg)
    {
        m_strErrMsg += strErrMsg;
    }

private:
    static int          m_iErrCode;
    static std::string  m_strErrMsg;
};

#define GET_ERRMSG_BY_ERRCODE(ERRCODE) #ERRCODE
#define SET_ERR_INFO(ERRCODE) CLogicError::SetErrCode(ERRCODE, #ERRCODE)


