#pragma once

#include "logicso_link.h"

class CLogicServiceApi : public ILogicServiceApi
{
public:
	template<typename T>
	friend class CCreateLogicServiceApi;

	CLogicServiceApi();

	void SetConnectorSender(ILogicConnectorSender* connector_sender);

	void SetCrossConnection(ILogicTCPConnection* cross_connection);

    void SetQueueConnection(ILogicTCPConnection* queue_connection);

	CLogicSoGameApi* GetLogicSoGameApi();

public:
    std::string_view GetServiceVersion() override ;

	ILogicConnectorSender* GetConnectorSender() override;

	ILogicTCPConnection* GetCrossConnection() override;

    ILogicTCPConnection* GetQueueConnection() override;

	taf::TC_DayLogger* GetLogTraceIns() override;

	taf::TC_DayLogger* GetLogSlowIns() override;

	ILogicStatistics* GetStatisticsIns() override;

	void LoadConfigSuccess() override;

    void RequestServiceAlert(const std::string &id, const map<string, string> &params) override;

	void ExecGameSQL(int iGroupID, int iUin, const std::string &strAPIName, const std::string &strColorRation,
                     const std::vector<std::string> &stSQL) override;

    void ExecStatSQL(int iGroupID, int iUin, const std::string &strAPIName, const std::string &strColorRation, const std::string & strSQL) override;

	std::string FilterTextEmoji(const std::string& strContent) override;

	std::string SkipText4ByteChars(const std::string& strContent) override;

	bool CheckNickHasDirtyWord(const std::string& strNick) override;

	bool FilterTalkContent(std::string& strContent) override;

	int CheckNickCorrect(const std::string& strNick, int& iDisplayCharWidth) override;

private:
    std::string version_;
	ILogicConnectorSender* connector_sender_;
	ILogicTCPConnection* cross_connection_;
    ILogicTCPConnection* queue_connection_;
	std::shared_ptr<CLogicSoGameApi> game_api_;
};

#define SERVICE_API_INS taf::TC_Singleton<CLogicServiceApi, CCreateLogicServiceApi>::getInstance()

template<typename T>
class CCreateLogicServiceApi
{
public:
	static CLogicServiceApi* create();

	static void destroy(CLogicServiceApi* api);
};

template <typename T>
CLogicServiceApi* CCreateLogicServiceApi<T>::create()
{
	auto* api = new CLogicServiceApi();

	api->game_api_.reset(CreateGameApiPtr(), DestroyGameApiPtr);

	api->game_api_->SetLogicServiceApi(api);

	return api;
}

template <typename T>
void CCreateLogicServiceApi<T>::destroy(CLogicServiceApi* api)
{
	delete api;
}

