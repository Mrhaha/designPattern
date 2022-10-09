#include <utility>

#include "logicso_game_api.h"
#include "logic_link_common.h"
#include "common_utf8handle.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_game_daily_task_system.h"
#include "logic_game_admin_processor.h"
#include "logic_game_global_frame.h"
#include "logic_game_global.h"
#include "logic_config.h"
#include "logic_game_rank_list.h"
#include "logic_game_data_recovery.h"
#include "logic_game_global.h"
#include "logic_manager_limit_boss.h"

CLogicSoGameApi* g_pstGameApi = nullptr;

void CLogicSoGameApi::SetLogicServiceApi(ILogicServiceApi* service_api)
{
	service_api_ = service_api;
}

ILogicServiceApi* CLogicSoGameApi::GetLogicServiceApi()
{
	assert(service_api_ != nullptr);
	return service_api_;
}

bool CLogicSoGameApi::InitGameApiSystem()
{
	CLogicCommon::FlushTime();

	LOGIC_PUBLISHER_INS_TYPE::getInstance();

	if (!CLogicGlobal::Initialize())
    {
        LOGIC_LOG_TRACE_ERROR << "LOGIC GAME GLOBAL DATA INIT FAIL" << std::endl;
        return false;
    }

	if (!LOGIC_GLOBAL_FRAME_INS->Initialize())
	{
		LOGIC_LOG_TRACE_ERROR << "LOGIC FRAME INIT FAIL" << std::endl;
		return false;
	}

    if(!CLogicManagerLimitBoss::Initialize())
    {
        LOGIC_LOG_TRACE_ERROR << "LOGIC LIMIT BOSS MANAGER INIT FAIL" << std::endl;
        return false;
    }

	return true;
}

void CLogicSoGameApi::StartGameApiSystem()
{
    //
}

void CLogicSoGameApi::StopGameApiSystem()
{
	CLogicGlobal::SendAllAsyncUserMail();

    //关闭服务器时，通知对应的网关把该服的客户端的连接全部断掉
    while (CLogicUser::GetInstance().GetUserNum() > 0)
    {
        GetLogicServiceApi()->GetConnectorSender()->NotifyCloseClient(
                CLogicUser::GetInstance().Begin()->first,
                CLogicUser::GetInstance().Begin()->second.m_usConnectionIndex,
                CLogicUser::GetInstance().Begin()->second.m_iClientIP,
                CLogicUser::GetInstance().Begin()->second.m_usClientPort);
    }

    CLogicDataRecovery::DumpRecoveryDataToFile();
}

int32_t CLogicSoGameApi::GetNowSec()
{
	return CLogicCommon::GetSec();
}

void CLogicSoGameApi::OnTimerTick()
{
	CLogicCommon::FlushTime();

    CLogicDataRecovery::CheckDumpDataTime();

	CLogicGlobal::Refresh();

	LOGIC_GLOBAL_FRAME_INS->Refresh();

    CLogicManagerLimitBoss::Refresh();
}

void CLogicSoGameApi::OnTimer15Sec()
{
    CLogicUser::GetInstance().UpdateUserOnlineTime();
}

bool CLogicSoGameApi::NeedPrintMessageInfo()
{
	return CLogicConfig::GetSystemConfig().IsPrintPackInfo();
}

bool CLogicSoGameApi::SetMysqlTableRecordConn(std::shared_ptr<taf::TC_Mysql> pMysqlHandle)
{
    CLogicSQLTableRecordBase::SetMysqlConn(std::move(pMysqlHandle));
    return true;
}

bool CLogicSoGameApi::ProcessUserMessage(int32_t iUin, int32_t iGroupID, int32_t iChannelID, int32_t iCmd, const char *pszBody, int32_t iBodyLength)
{
	processor_base_ptr_type stProcessor = LOGIC_CMD_CONTAINER.GetProcessor(iCmd);
	stProcessor->SetClientUser(iUin, iGroupID, iChannelID);
	return stProcessor->DoUserProcess(pszBody, static_cast<unsigned int>(iBodyLength));
}

bool CLogicSoGameApi::ProcessServerMessage(const CMsgHeadCross &stMsgHead, const char *pszBody, int32_t iBodyLength)
{
	processor_base_ptr_type stProcessor = LOGIC_CMD_CONTAINER.GetProcessor(stMsgHead.m_usCmd);
	return stProcessor->DoServerProcess(stMsgHead, pszBody, static_cast<unsigned int>(iBodyLength));
}

bool CLogicSoGameApi::ProcessServerMessage(const CMsgHeadQueue &stMsgHead, const char *pszBody, int32_t iBodyLength)
{
    processor_base_ptr_type stProcessor = LOGIC_CMD_CONTAINER.GetProcessor(stMsgHead.m_usCmd);
    return stProcessor->DoServerProcess(stMsgHead, pszBody, static_cast<unsigned int>(iBodyLength));
}

bool CLogicSoGameApi::ProcessAdminMessage(int32_t iUin, int16_t nGroupID, uint16_t usCmd, stLogicSoAdminClient &stClient)
{
	return CLogicAdminProcessor::ProcessMessage(iUin, nGroupID, usCmd, stClient);
}

std::pair<bool, std::string> CLogicSoGameApi::LoadGameConfig(std::function<const std::string&(const std::string&)> GetXmlContentFunc)
{
	auto pOldConfig = CLogicConfig::GetConfig();
	auto * pConfig = new CLogicConfig;
	CLogicConfig::SetConfig(pConfig);

	CLogicConfigParserBase* pParser;

	std::pair<bool, std::string> stRet(true, "OK");
	for (size_t i = 0; i < CLogicConfig::s_iConfigSize; ++i)
	{
		const auto& stConfigLoader = CLogicConfig::GetConfigLoader()[i];
		pParser = stConfigLoader.m_pParserGetFunc(pConfig);

		pParser->SetGetXMLContentCallback(GetXmlContentFunc);

		stRet = pParser->Load(stConfigLoader.m_pFileName, GetXmlContentFunc(stConfigLoader.m_pFileName));
		if (!stRet.first)
		{
			LOGIC_LOG_TRACE_ERROR << "PARSE CONFIG " << stConfigLoader.m_pFileName <<" ERROR|MSG:" << stRet.second;

            GetLogicServiceApi()->RequestServiceAlert("parse_config_fail", {
                    { "filename", stConfigLoader.m_pFileName},
                    { "error", stRet.second}
            });

            CLogicConfig::SetConfig(pOldConfig);
            delete pConfig;

			return stRet;
		}
	}

    delete pOldConfig;

	GetLogicServiceApi()->LoadConfigSuccess();

    CLogicCommon::SetDebugTimeOffset(CLogicConfig::GetSystemConfig().m_stServerConfig.m_iDebugTimeOffset);

	return stRet;
}

std::vector<std::string> CLogicSoGameApi::GetConfigFileNames()
{
	std::vector<std::string> names;
	for (size_t i = 0; i < CLogicConfig::s_iConfigSize; ++i)
	{
		names.emplace_back(CLogicConfig::GetConfigLoader()[i].m_pFileName);
	}
	return names;
}

const TLogicSoServerConfig & CLogicSoGameApi::GetGameServerConfig()
{
	return CLogicConfig::GetSystemConfig().m_stServerConfig;
}

const TLogicSoRouterConfig & CLogicSoGameApi::GetGameRouterConfig()
{
	return CLogicConfig::GetSystemConfig().m_stRouterConfig;
}

const TLogicSoMysqlConfig & CLogicSoGameApi::GetGameMysqlConfig()
{
	return CLogicConfig::GetSystemConfig().m_stMysqlConfig;
}

std::vector<std::vector<uint32_t>> CLogicSoGameApi::GetUtf8DirtyWordNameList()
{
	std::vector<uint32_t> stUtf8DirtyWord;
	std::vector<std::vector<uint32_t>> stUtf8DirtyWordList;
	for (const auto& strDirtyWord : CLogicConfig::GetDirtyWordConfig().m_stDirtyWordList)
	{
		stUtf8DirtyWord.clear();
		CUTF8Handle::Decode(strDirtyWord, stUtf8DirtyWord);
		stUtf8DirtyWordList.push_back(stUtf8DirtyWord);
	}
	for (const auto& strDirtyWord : CLogicConfig::GetZooLogicGameConfig().m_strDirtyWordVec)
	{
		stUtf8DirtyWord.clear();
		CUTF8Handle::Decode(strDirtyWord, stUtf8DirtyWord);
		stUtf8DirtyWordList.push_back(stUtf8DirtyWord);
	}
	return stUtf8DirtyWordList;
}

std::vector<std::vector<uint32_t>> CLogicSoGameApi::GetUtf8DirtyWordChatList()
{
    std::vector<uint32_t> stUtf8DirtyWord;
    std::vector<std::vector<uint32_t>> stUtf8DirtyWordList;
    for (const auto& strDirtyWord : CLogicConfig::GetDirtyWordChatConfig().m_stDirtyWordList)
    {
        stUtf8DirtyWord.clear();
        CUTF8Handle::Decode(strDirtyWord, stUtf8DirtyWord);
        stUtf8DirtyWordList.push_back(stUtf8DirtyWord);
    }
    return stUtf8DirtyWordList;
}

void CLogicSoGameApi::NotifyQueueSvrUserListOnline()
{
    CLogicGlobal::NotifyQueueSvrUserListOnline();
}

stLogicSoSenderUser CLogicSoGameApi::GetClientUser(int iGroupID, int iUin)
{
	stLogicSoSenderUser stClientInfo = {};
	const auto stRet = CLogicUser::GetInstance().GetUser(iUin);
	if (stRet.second)
	{
		stClientInfo.m_iConnectionIndex = stRet.first->second.m_usConnectionIndex;

		const auto pUserInfo = CLogicCacheCommon::LoadUserData(iGroupID, iUin);
		if (pUserInfo)
		{
			stClientInfo.m_iUserGold = pUserInfo->m_stUserInfoTable.GetGold();
			stClientInfo.m_iUserRmb = pUserInfo->m_stUserInfoTable.GetRmb();
			stClientInfo.m_iUserPhyPower = pUserInfo->m_stUserInfoTable.GetPhyPower();
			stClientInfo.m_iUserLastAutoAddTime = pUserInfo->m_stUserInfoTable.GetLastAutoAddTime();
		}
	}

	return stClientInfo;
}

void CLogicSoGameApi::DelClientUser(int32_t iUin)
{
	CLogicUser::GetInstance().DelUser(iUin);
}

stLogicSoClientInfo CLogicSoGameApi::GetClientInfo(int32_t iUin)
{
	return CLogicUser::GetInstance().GetClientInfo(iUin);
}

bool CLogicSoGameApi::AddClientUser(int iGroupID, int iUin, stLogicSoClientInfo& stUserInfo)
{
	return CLogicUser::GetInstance().AddUser(iGroupID, iUin, stUserInfo).second;
}

std::map<int32_t, int32_t> CLogicSoGameApi::GetOnlineChannelCount()
{
    std::map<int32_t, int32_t> online;
    for (auto iterUser = CLogicUser::GetInstance().Begin(); iterUser != CLogicUser::GetInstance().End(); ++iterUser)
    {
        ++online[iterUser->second.m_iChannelID];
    }
    return online;
}

int32_t CLogicSoGameApi::ParseRecoveryDataFile(const std::string &filename)
{
	return CLogicDataRecovery::ParseRecoveryDataFile(filename);
}
