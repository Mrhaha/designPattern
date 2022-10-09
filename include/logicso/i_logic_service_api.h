#pragma once
#include "tc_logger.h"
#include "i_logic_statistics.h"
#include "i_logic_connector_sender.h"
#include "i_logic_tcpconnection.h"

class ILogicServiceApi
{
public:
    virtual ~ILogicServiceApi() = default;

    virtual std::string_view GetServiceVersion() = 0;

	virtual ILogicConnectorSender* GetConnectorSender() = 0;

	virtual ILogicTCPConnection* GetCrossConnection() = 0;

    virtual ILogicTCPConnection* GetQueueConnection() = 0;

	virtual taf::TC_DayLogger* GetLogTraceIns() = 0;

	virtual taf::TC_DayLogger* GetLogSlowIns() = 0;

	virtual ILogicStatistics* GetStatisticsIns() = 0;

	virtual void LoadConfigSuccess() = 0;

	virtual void RequestServiceAlert(const std::string &id, const map<string, string> &params) = 0;

	virtual void ExecGameSQL(int iGroupID, int iUin, const std::string &strAPIName, const std::string &strColorRation,
                             const std::vector<std::string> &stSQL) = 0;

	virtual void ExecStatSQL(int iGroupID, int iUin, const std::string &strAPIName, const std::string &strColorRation, const std::string & strSQL) = 0;

	virtual std::string FilterTextEmoji(const std::string& strContent) = 0;

	virtual std::string SkipText4ByteChars(const std::string& strContent) = 0;

	virtual bool CheckNickHasDirtyWord(const std::string& strNick) = 0;

	virtual bool FilterTalkContent(std::string& strContent) = 0;

	virtual int CheckNickCorrect(const std::string& strNick, int& iDisplayCharWidth) = 0;
};

