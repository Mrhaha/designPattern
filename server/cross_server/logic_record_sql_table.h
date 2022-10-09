#pragma once

#include "tc_mysql.h"
#include "tc_common.h"

#include "msgpack.hpp"

#include <map>
#include <memory>
#include <string>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdio.h>

class CLogicSQLTableRecordBase
{
public:
    static const char* SQL_TRANSACTION_START;
    static const char* SQL_TRANSACTION_END;
    
public:
    CLogicSQLTableRecordBase() : m_iOperation(UNDEFINED) { }

    virtual ~CLogicSQLTableRecordBase() { }

    enum sql_operation_t
    {
        UNDEFINED = 0,
        INSERT = 1,
        UPDATE = 2,
        DELETE = 3,
    };

    struct uint64_type_elem_t
    {
        uint64_type_elem_t() :m_ulVal(0), m_cOperator('+') { }

        uint64_t    m_ulVal;
        char        m_cOperator;
    };

    virtual std::string MakeInsertSQL() = 0;
    virtual std::string MakeUpdateSQL() = 0;
    virtual std::string MakeDeleteSQL() = 0;
    
    virtual int GetUid() = 0;
    virtual int GetGroupID() = 0; 

    std::string MakeSQL();

    void SetOperation(int iOperation);

    inline void SetSQLComment(const std::string& strAPIName, const std::string& strColorRation)
    {
        m_strAPIName = strAPIName;
        m_strColorRation = strColorRation;
    }

    inline std::string Char2DBString(char cData)
    {
        char szBuffer[32];
        snprintf(szBuffer, sizeof(szBuffer), "%d", (int)cData);
        return szBuffer;
    }

    inline std::string Int2DBString(int iData)
    {
        char szBuffer[32];
        snprintf(szBuffer, sizeof(szBuffer), "%d", iData);
        return szBuffer;
    }

    inline std::string Uint642DBString(uint64_t ulData)
    {
        char szBuffer[32];
        snprintf(szBuffer, sizeof(szBuffer), "%" PRIu64, ulData);
        return szBuffer;
    }

    inline std::string String2DBString(const std::string& strData)
    {
        return ("'" + GetMysqlConn()->escapeString(strData) + "'");
    }

    inline std::string Var2DBString(const std::string& strData)
    {
        return ("0x" + taf::TC_Common::bin2str(strData.c_str(), strData.size()));
    }

    template<typename T> static std::pair<bool, std::string> FormatDBBin(const std::string& strBin, T& stData)
    {
        std::pair<bool, std::string> stRet;
        stRet.first = true;

        const char* pBuffer = strBin.c_str();
        size_t uiLength = strBin.size();
        msgpack::unpacked stResult;
        if(uiLength > 0)
        {
            try
            {
                msgpack::unpack(&stResult, pBuffer, uiLength);
            }
            catch(const std::runtime_error& stEx)
            {
                stRet.first = false;
                stRet.second = "NOT_MSGPACK_FORMAT|EXCEPTION_MSG:" + std::string(stEx.what());
                return (stRet);
            }

            const msgpack::object& stMsgObj = stResult.get();
            try
            {
                stMsgObj.convert(&stData);
            }
            catch(const std::bad_cast& stEx)
            {
                stRet.first = false;
                stRet.second = "STRUCTURE_IS_NOT_MATCH|EXCEPTION_MSG:" + std::string(stEx.what());
                return (stRet);
            }
        }

        return (stRet);
    }

    static inline void SetMysqlConn(taf::TC_Mysql* pMysqlHandle)
    {
        s_pMysqlHandle = std::shared_ptr<taf::TC_Mysql>(pMysqlHandle);
    }

    static inline void SetMysqlConn(std::shared_ptr<taf::TC_Mysql> pMysqlHandle)
    {
        s_pMysqlHandle = pMysqlHandle;
    }
    
    static inline taf::TC_Mysql* GetMysqlConn()
    {
        return (s_pMysqlHandle.get());
    }

protected:
    int m_iOperation;

    std::string m_strAPIName;
    std::string m_strColorRation;

    std::map<const char*, char>                 m_stCharData;
    std::map<const char*, int>                  m_stIntData;
    std::map<const char*, uint64_type_elem_t>   m_stUInt64Data;
    std::map<const char*, std::string>          m_stStringData;
    std::map<const char*, std::string>          m_stVarData;

    static std::shared_ptr<taf::TC_Mysql>       s_pMysqlHandle;
};
