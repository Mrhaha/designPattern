#pragma once

#include "logic_game_user.h"
#include "logic_game_cache_table.h"
#include "logic_game_bill.h"

#define PROCESSOR_LOG_DEBUG LOGIC_LOG_TRACE_DEBUG << "GROUP_ID:" << m_iGroupID << "|UIN:" << m_iUin << "|CMD:" << m_usCmd << "|CMD_NAME:" << m_strCmdName << "|"
#define PROCESSOR_LOG_ERROR LOGIC_LOG_TRACE_ERROR << "GROUP_ID:" << m_iGroupID << "|UIN:" << m_iUin << "|CMD:" << m_usCmd << "|CMD_NAME:" << m_strCmdName << "|"

#define SEND_ERRCODE_AND_RET(ERROR_CODE) \
{                                        \
    PROCESSOR_LOG_ERROR << "ERRCODE:" << ERROR_CODE << "|MSG:" << #ERROR_CODE << std::endl; \
    SendToClient(ERROR_CODE); \
    return (false);	\
}

#define SEND_ERRCODE_AND_RET1(ERROR_CODE, PARAM1) \
{                                        \
    PROCESSOR_LOG_ERROR << "ERRCODE:" << ERROR_CODE << "|MSG:" << #ERROR_CODE << "|PARAM1:" << PARAM1 << std::endl; \
    SendToClient(ERROR_CODE); \
    return (false);	\
}

#define SEND_ERRCODE_AND_RET2(ERROR_CODE, PARAM1, PARAM2) \
{                                        \
    PROCESSOR_LOG_ERROR << "ERRCODE:" << ERROR_CODE << "|MSG:" << #ERROR_CODE << "|PARAM1:" << PARAM1 << "|PARAM2:" << PARAM2 << std::endl; \
    SendToClient(ERROR_CODE); \
    return (false);	\
}

#define SEND_ERRCODE_AND_RET3(ERROR_CODE, PARAM1, PARAM2, PARAM3) \
{                                        \
    PROCESSOR_LOG_ERROR << "ERRCODE:" << ERROR_CODE << "|MSG:" << #ERROR_CODE << "|PARAM1:" << PARAM1 << "|PARAM2:" << PARAM2 << "|PARAM3:" << PARAM3 << std::endl; \
    SendToClient(ERROR_CODE); \
    return (false);	\
}

class CLogicProcessorBase
{    
public:
    CLogicProcessorBase(uint16_t usCmd, const std::string& strCmdName);

    virtual ~CLogicProcessorBase();

	void SetClientUser(int32_t iUin, int32_t iGroupID, int32_t iChannelID);

    void PrintSendMsg(const msgpack::unpacked& stResult);

    void PrintRecvMsg(const msgpack::unpacked& stResult);

	ILogicConnectorSender* GetConnectorSender();

	ILogicTCPConnection* GetCrossConnection();

public:
    template <class MSGPACK_T>
	bool DecodeMsgPack(const msgpack::unpacked& stFormatData, MSGPACK_T& stParam);

    template <class MSGPACK_T>
	void SendToClient(MSGPACK_T& stResultData, int iErrcode);

    void SendToClient(int iErrorCode);
    void SendSuccessCacheItemToClient();

	template <class MSGPACK_T>
	void NotifyToOtherClient(int iGroupID, int iUin, int iProtoCmd, MSGPACK_T& stNotifyData);

	template <class MSGPACK_T>
	void BroadcastToClient(int iProtoCmd, MSGPACK_T& stResultData);

	template <class MSGPACK_T>
	void SendToCrossServer(int8_t byCmdType, uint16_t usCmd, MSGPACK_T& stResultData, int32_t iDstUin = 0, int16_t nDstGroupID = 0);

    template <class MSGPACK_T>
    void SendToCrossReportData(uint16_t usCmd,const MSGPACK_T &params);

	bool DoUserProcess(const char *pBuffer, unsigned int uiLength);

	bool DoServerProcess(const CMsgHeadCross &stMsgHead, const char *pBuffer, unsigned int uiLength);

    bool DoServerProcess(const CMsgHeadQueue &stMsgHead, const char *pBuffer, unsigned int uiLength);

protected:    
    virtual bool DoUserRun(const msgpack::unpacked& stFormatData) = 0;

	virtual bool DoServerRun(const CMsgHeadCross &stMsgHead, const msgpack::unpacked &stFormatData) { return false; }

    virtual bool DoServerRun(const CMsgHeadQueue &stMsgHead, const msgpack::unpacked &stFormatData) { return false; }

protected:
	uint16_t	m_usCmd;
	std::string m_strCmdName;

	int32_t		m_iUin;
	int32_t		m_iGroupID;
	int32_t     m_iChannelID;
    stLogicSoClientInfo m_stUserConnectorData;
    user_data_table_ptr_type m_pUserInfo;
};

template <class MSGPACK_T>
bool CLogicProcessorBase::DecodeMsgPack(const msgpack::unpacked& stFormatData, MSGPACK_T& stParam)
{
	const msgpack::object& stMsgObj = stFormatData.get();
    PROCESSOR_LOG_DEBUG << "|PROTO_DATA:" << stMsgObj << std::endl;

	try
	{
		stMsgObj.convert(&stParam);
	}
	catch (const std::bad_cast& stEx)
	{
		PROCESSOR_LOG_ERROR << "PROTO IS NOT MATCH|MSG:" << stEx.what() << "|PROTO_DATA:" << stMsgObj << std::endl;

		GetConnectorSender()->NotifyCloseClient(m_iUin,
		                                        m_stUserConnectorData.m_usConnectionIndex,
		                                        m_stUserConnectorData.m_iClientIP,
		                                        m_stUserConnectorData.m_usClientPort);

		return (false);
	}

	return (true);
}

template <class MSGPACK_T>
void CLogicProcessorBase::SendToClient(MSGPACK_T& stResultData, int iErrcode)
{
	GetConnectorSender()->NotifyClientUser(m_iGroupID, m_iUin, m_usCmd, stResultData, iErrcode);
}

template <class MSGPACK_T>
void CLogicProcessorBase::NotifyToOtherClient(int iGroupID, int iUin, int iProtoCmd, MSGPACK_T& stNotifyData)
{
    GetConnectorSender()->NotifyClientUser(iGroupID, iUin, iProtoCmd, stNotifyData);
}

template <class MSGPACK_T>
void CLogicProcessorBase::BroadcastToClient(int iProtoCmd, MSGPACK_T& stResultData)
{
	const auto& stInstance = CLogicUser::GetInstance();
	for (auto stIter = stInstance.Begin(); stIter != stInstance.End(); ++stIter)
	{
		NotifyToOtherClient(stIter->second.m_iGroupID, stIter->first, iProtoCmd, stResultData);
	}
}

template<class MSGPACK_T>
void CLogicProcessorBase::SendToCrossServer(int8_t byCmdType, uint16_t usCmd, MSGPACK_T & stResultData, int32_t iDstUin, int16_t nDstGroupID)
{
	CMsgHeadCross stMsgHead;
	stMsgHead.m_byCmdType = byCmdType;
	stMsgHead.m_usCmd = usCmd;
	stMsgHead.m_iUin = iDstUin > 0 ? iDstUin : m_iUin;
	stMsgHead.m_nGroupID = nDstGroupID > 0 ? nDstGroupID : static_cast<int16_t>(m_iGroupID);

	GetCrossConnection()->SendToServer(stMsgHead, stResultData);
}

template <class MSGPACK_T>
void CLogicProcessorBase::SendToCrossReportData(uint16_t usCmd,const MSGPACK_T &params)
{
    CMsgHeadCross stMsgHead;
    stMsgHead.m_byCmdType = enmCmdType_request;
    stMsgHead.m_usCmd = usCmd;
    stMsgHead.m_iUin = m_iUin;
    stMsgHead.m_nGroupID = m_iGroupID;
    GetCrossConnection()->SendToServer(stMsgHead, params);
}

template <class T>
class CDefaultProcessorCreator
{
public:
    static T* Create(uint16_t usCmd, const std::string& strCmdName)
    {
        return (new T(usCmd, strCmdName));
    }
    
    static void Destroy(CLogicProcessorBase *pProcessor)
    {
        delete pProcessor;
    }
};

typedef std::shared_ptr<CLogicProcessorBase> processor_base_ptr_type;

template <class T, class CreatorT = CDefaultProcessorCreator<T> >
class CLogicProcessorFactory
{
public:
    static processor_base_ptr_type Create(uint16_t usCmd, const std::string& strCmdName)
    {
        TAF_STATIC_CHECK(SUPERSUBCLASS(CLogicProcessorBase, T), processor_factory_type_can_not_convert);
        
        return (processor_base_ptr_type(CreatorT::Create(usCmd, strCmdName), CreatorT::Destroy));
    };
};
