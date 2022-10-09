#pragma once

#include <functional>
#include "tc_mysql.h"
#include "i_logic_service_api.h"

class CLogicSoGameApi
{
public:
	void SetLogicServiceApi(ILogicServiceApi* service_api);

	ILogicServiceApi* GetLogicServiceApi();

public:
	bool InitGameApiSystem();

	void StartGameApiSystem();

	void StopGameApiSystem();

public:
    int32_t GetNowSec();

	void OnTimerTick();

	void OnTimer15Sec();

public:
    bool NeedPrintMessageInfo();

	bool SetMysqlTableRecordConn(std::shared_ptr<taf::TC_Mysql> pMysqlHandle);

    bool ProcessUserMessage(int32_t iUin, int32_t iGroupID, int32_t iChannelID, int32_t iCmd, const char *pszBody, int32_t iBodyLength);

	bool ProcessServerMessage(const CMsgHeadCross &stMsgHead, const char *pszBody, int32_t iBodyLength);

    bool ProcessServerMessage(const CMsgHeadQueue &stMsgHead, const char *pszBody, int32_t iBodyLength);

	bool ProcessAdminMessage(int32_t iUin, int16_t nGroupID, uint16_t usCmd, stLogicSoAdminClient& stClient);

public:
	std::pair<bool, std::string> LoadGameConfig(std::function<const std::string&(const std::string&)> GetXmlContentFunc);

	std::vector<std::string> GetConfigFileNames();

	const TLogicSoServerConfig & GetGameServerConfig();

	const TLogicSoRouterConfig & GetGameRouterConfig();

	const TLogicSoMysqlConfig & GetGameMysqlConfig();

	std::vector<std::vector<uint32_t>> GetUtf8DirtyWordNameList();

    std::vector<std::vector<uint32_t>> GetUtf8DirtyWordChatList();


    void NotifyQueueSvrUserListOnline();

public:
    stLogicSoSenderUser GetClientUser(int iGroupID, int iUin);

	stLogicSoClientInfo GetClientInfo(int32_t iUin);

    void DelClientUser(int32_t iUin);

    bool AddClientUser(int iGroupID, int iUin, stLogicSoClientInfo& stUserInfo);

    std::map<int32_t, int32_t> GetOnlineChannelCount();

public:
    int32_t ParseRecoveryDataFile(const std::string& filename);

private:
	ILogicServiceApi* service_api_ = nullptr;
};

