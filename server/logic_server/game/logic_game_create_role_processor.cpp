#include "server_errcode.h"
#include "server_cmd_logic_define.h"
#include "server_friend_protocol.h"
#include "server_user_protocol.h"
#include "logic_link_common.h"
#include "logic_config.h"
#include "logic_game_item.h"
#include "logic_game_cache_common.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_game_create_role_processor.h"
#include "logic_record_server_nick_table.h"
#include "logic_game_item_factory.h"
#include "logic_game_day_refresh_system.h"
#include "logic_game_version_data_update.h"

REGISTER_CMD_FACTORY(MSG_LOGIC_CREATE_ROLE, CLogicCreateRoleProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_CHECK_ROLE_EXIST, CLogicCreateRoleProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_CHECK_NICK_CORRECT, CLogicCreateRoleProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_CHANGE_NICK, CLogicCreateRoleProcessor)

REGISTER_CMD_FACTORY(MSG_LOGIC_DEBUG_REFRESH_DATA, CLogicCreateRoleProcessor)

CLogicCreateRoleProcessor::CLogicCreateRoleProcessor(uint16_t usCmd, const std::string &strCmdName)
        : CLogicProcessorBase(usCmd, strCmdName)
{
}

bool 
CLogicCreateRoleProcessor::DoUserRun(const msgpack::unpacked& stFormatData)
{
    int iUin = m_iUin;
    int iGroupID = m_iGroupID;

    if ((int)m_usCmd == MSG_LOGIC_CREATE_ROLE)
    {
        CRequestCreateRole stParam;
        if(DecodeMsgPack(stFormatData, stParam) == false) return (false);

        auto iErrorCode = CLogicCacheCommon::CheckNickValid(stParam.m_strNick);
        if(iErrorCode != SERVER_ERRCODE::SUCCESS)
        {
            SEND_ERRCODE_AND_RET(iErrorCode)
        }

        // 固定女角色
        stParam.m_ucRoleGender = 0;

        int iCardID = CLogicConfigDefine::FEMALE_LEADING_HERO_ID;
        if(!CLogicConfig::GetHeroCardConfig().GetConfig(iCardID))
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << LEADING_ROLE_HERO_CARD_CONFIG_ERROR << "|MSG:" 
                                << GET_ERRMSG_BY_ERRCODE(LEADING_ROLE_HERO_CARD_CONFIG_ERROR) << std::endl;

            SEND_ERRCODE_AND_RET(LEADING_ROLE_HERO_CARD_CONFIG_ERROR)
        }

        std::string strEscapeNick = CLogicSQLTableRecordBase::GetMysqlConn()->escapeString(stParam.m_strNick);
        std::vector<std::string> stTransactionSQL;
        stTransactionSQL.push_back(CLogicSQLTableRecordBase::SQL_TRANSACTION_START);
		stTransactionSQL.push_back(GenServerNickTableSQL(strEscapeNick));
        stTransactionSQL.push_back(CLogicSQLTableRecordBase::SQL_TRANSACTION_END);

        try
        {
            for(std::vector<std::string>::const_iterator it = stTransactionSQL.begin(); it != stTransactionSQL.end(); it ++)
            {
                CLogicSQLTableRecordBase::GetMysqlConn()->execute(*it);
            }
        }
        catch(const taf::TC_Mysql_Exception& stEx)
        {
            char szTemp[128], szKey[128];
            const static std::string cstrNick = CRecordServerNickTable::s_strNick;
            int iErrCode = mysql_errno(CLogicSQLTableRecordBase::GetMysqlConn()->getMysql());
            if (iErrCode == 1062) //ER_DUP_ENTRY
            {
                std::string strErrString = mysql_error(CLogicSQLTableRecordBase::GetMysqlConn()->getMysql());
                sscanf(strErrString.c_str(), "Duplicate entry %s for key '%[a-zA-z_]'", szTemp, szKey);
                if (cstrNick.compare(szKey) == 0)
                {
                    using namespace SERVER_ERRCODE;
                    PROCESSOR_LOG_ERROR << "CREATE ROLE IN DATABASE ERROR|GROUP_ID:" << iGroupID << "|UIN:" << iUin 
                                        << "|DATABASE_ERROR_MSG:" << stEx.what() << "|ERRCODE:" << USER_ROLE_NICK_EXIST 
                                        << "|MSG:" << GET_ERRMSG_BY_ERRCODE(USER_ROLE_NICK_EXIST) << std::endl;

                    SendToClient(USER_ROLE_NICK_EXIST);
                }
                else
                {
                    using namespace SERVER_ERRCODE;
                    PROCESSOR_LOG_DEBUG << "USER HAS CREATE ROLE, BUT CALL CREATE ROLE AGAIN|MSG:" << strErrString << std::endl; 

                    SendToClient(SUCCESS);
                }
            }
            else
            {
                using namespace SERVER_ERRCODE;
                PROCESSOR_LOG_ERROR << "CREATE ROLE IN DATABASE ERROR|GROUP_ID:" << iGroupID << "|UIN:" << iUin 
                                    << "|DATABASE_ERROR_MSG:" << stEx.what() << "|ERRCODE:" << INTERN_ERROR 
                                    << "|MSG:" << GET_ERRMSG_BY_ERRCODE(INTERN_ERROR) << std::endl;

                SendToClient(INTERN_ERROR);
            }
            CLogicSQLTableRecordBase::GetMysqlConn()->execute("ROLLBACK;");
            return (false);
        }

        if(CreateRole(iCardID, strEscapeNick) == nullptr)
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "CreateRole ERROR|GROUP_ID:" << iGroupID << "|UIN:" << iUin
                                << "|ERRCODE:" << INTERN_ERROR << "|MSG:" << GET_ERRMSG_BY_ERRCODE(INTERN_ERROR) << std::endl;

            SEND_ERRCODE_AND_RET(INTERN_ERROR)
        }

        std::string strGender = 1 == stParam.m_ucRoleGender ? "male" : "female";
        PROCESSOR_LOG_DEBUG << "CREATE ROLE SUCCESS|NICK:" << stParam.m_strNick 
                            << "|LEADING ROLE GENDER:" << strGender 
                            << "|DEVICE_TYPE:" << stParam.m_iDeviceType << "|MAC:" << stParam.m_strMac 
                            << "|ID_FA:" << stParam.m_strIDFa 
                            << "|ACCOUNT:" << stParam.m_stStatInfo.m_strAccount 
                            << "|DEVICE_ID:" << stParam.m_stStatInfo.m_strDeviceID
                            << "|DEVICE_MODEL:" << stParam.m_stStatInfo.m_strDeviceModel 
                            << "|DEVICE_OS:" << stParam.m_stStatInfo.m_strDeviceOS
                            << "|DEVICE_RATIO:" << stParam.m_stStatInfo.m_strDeviceRatio 
                            << "|DEVICE_NET:" << stParam.m_stStatInfo.m_strDeviceNet 
                            << "|DEVICE_CARRIER:" << stParam.m_stStatInfo.m_strDeviceCarrier 
                            << "|PLATFORM_ID:" << stParam.m_stStatInfo.m_strPlatformID
                            << "|REMOTE_HOST:" << m_stUserConnectorData.m_strClientIP
							<< "|CHANNEL_ID:" << m_iChannelID
                            << std::endl;

		LOGIC_STATISTICS_INS->AddCreateRoleCount(m_iChannelID);

        CResponseCreateRole stRsp;
        stRsp.m_iGroupID = iGroupID;
        stRsp.m_iUin = iUin;
        stRsp.m_strNick = stParam.m_strNick;
        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

        return (true);
    }
    else if ((int)m_usCmd == MSG_LOGIC_CHECK_ROLE_EXIST)
    {
        CResponseCheckUserExist stRsp;

        CPackUserStatInfo stParam;
        const msgpack::object& stMsgObj = stFormatData.get();
        try
        {
            stMsgObj.convert(&stParam);

            m_stUserConnectorData.m_stStatInfo.m_strAccount = stParam.m_strAccount;
            m_stUserConnectorData.m_stStatInfo.m_strDeviceCarrier = stParam.m_strDeviceCarrier;
            m_stUserConnectorData.m_stStatInfo.m_strDeviceID = stParam.m_strDeviceID;
            m_stUserConnectorData.m_stStatInfo.m_strDeviceModel = stParam.m_strDeviceModel;
            m_stUserConnectorData.m_stStatInfo.m_strDeviceNet = stParam.m_strDeviceNet;
            m_stUserConnectorData.m_stStatInfo.m_strDeviceOS = stParam.m_strDeviceOS;
            m_stUserConnectorData.m_stStatInfo.m_strDeviceRatio = stParam.m_strDeviceRatio;
            m_stUserConnectorData.m_stStatInfo.m_strPlatformID = stParam.m_strPlatformID;
            m_stUserConnectorData.m_stStatInfo.m_strPlatformSign = stParam.m_strPlatformSign;
			m_stUserConnectorData.m_iResVersion = stParam.m_iResVer;
			m_stUserConnectorData.m_iChannelID = m_iChannelID;

            CLogicUser::GetInstance().SetUserInfo(m_iUin, m_stUserConnectorData);
        }
        catch(const std::bad_cast& stEx)
        {
        }

        m_pUserInfo = CLogicCacheCommon::LoadUserData(iGroupID, iUin);

        if (m_pUserInfo == nullptr)
        {
            stRsp.m_iCode = 0;
            PROCESSOR_LOG_DEBUG << "ROLE NOT EXIST RETURN:" << stRsp.m_iCode << std::endl;
        }
        else
        {
            stRsp.m_iCode = 1;
            PROCESSOR_LOG_DEBUG << "ROLE HAS EXIST RETURN:" << stRsp.m_iCode << std::endl;
        }

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

        return (true);
    }
    else if ((int)m_usCmd == MSG_LOGIC_CHECK_NICK_CORRECT)
    {
        CRequestCheckNickCorrect stParam;
        if(DecodeMsgPack(stFormatData, stParam) == false) return (false);

        auto iErrorCode = CLogicCacheCommon::CheckNickValid(stParam.m_strNick);
        if(iErrorCode != SERVER_ERRCODE::SUCCESS)
        {
            SEND_ERRCODE_AND_RET(iErrorCode)
        }

        std::string strNick = CLogicSQLTableRecordBase::GetMysqlConn()->escapeString(stParam.m_strNick);

        char szBuffer[1024];
        snprintf(szBuffer, sizeof(szBuffer), "SELECT * FROM server_nick_t WHERE nick=\"%s\";", strNick.c_str());

        taf::TC_Mysql::MysqlData stData;
        try {
            stData = CLogicSQLTableRecordBase::GetMysqlConn()->queryRecord(szBuffer);
        } catch(const taf::TC_Mysql_Exception& stEx) {}

        if (stData.size() > 0)
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << USER_ROLE_NICK_EXIST << "|MSG:" 
                                << GET_ERRMSG_BY_ERRCODE(USER_ROLE_NICK_EXIST)
                                << "|NICK:" << stParam.m_strNick << std::endl;

            SEND_ERRCODE_AND_RET(USER_ROLE_NICK_EXIST)
        }

        SendToClient(SERVER_ERRCODE::SUCCESS);
        return (true);
    }
	else if (m_usCmd == MSG_LOGIC_CHANGE_NICK)
    {
		CRequestChangeNick stReqBody;
		CResponseChangeNick stRspBody;
		if(DecodeMsgPack(stFormatData, stReqBody) == false) return (false);

        auto iErrorCode = CLogicCacheCommon::CheckNickValid(stReqBody.m_strNick);
        if(iErrorCode != SERVER_ERRCODE::SUCCESS)
        {
            SEND_ERRCODE_AND_RET(iErrorCode)
        }
		
		int32_t iCostYuanbao = m_pUserInfo->m_stUserInfoTable.GetChangeNickCount() ? CLogicConfig::GetGlobalConfig().m_iChangeNickYuanbao : 0;
		if (CLogicCacheCommon::GetItemNum(m_pUserInfo, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_YuanBaoID) < iCostYuanbao)
		{
			SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_RMB)
		}

		if (!CLogicCacheCommon::ChangeRoleNick(m_pUserInfo, stReqBody.m_strNick, CLogicSQLTableRecordBase::UPDATE))
		{
			PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode() << "|MSG:" << CLogicError::GetErrMsg() << std::endl;
			SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
		}

        CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_YuanBaoID, iCostYuanbao, m_usCmd);

        m_pUserInfo->m_stUserInfoTable.AddChangeNickCount(1);
		
		stRspBody.m_strNick = stReqBody.m_strNick;
        stRspBody.m_iChangeNickCount = m_pUserInfo->m_stUserInfoTable.GetChangeNickCount();
		SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
		return true;
    }
    else if (m_usCmd == MSG_LOGIC_DEBUG_REFRESH_DATA)
    {
        return DebugRefreshRoleData();
    }

    return (true);
}

std::string CLogicCreateRoleProcessor::GenServerNickTableSQL(const std::string& strNick) const
{
	CRecordServerNickTable stServerNickTableRecord;
	stServerNickTableRecord.SetOperation(CLogicSQLTableRecordBase::INSERT);
	stServerNickTableRecord.SetPrimaryKey(m_iUin, m_iGroupID);
	stServerNickTableRecord.SetNick(strNick);

	return (stServerNickTableRecord.MakeInsertSQL());
}

user_data_table_ptr_type CLogicCreateRoleProcessor::CreateRole(int iCardID, const std::string& strNick) const
{
    user_data_table_ptr_type pUserData = new user_data_table_t();
    pUserData->m_iUin     = m_iUin;
    pUserData->m_iGroupID = m_iGroupID;
    pUserData->m_strNick  = strNick;
    pUserData->m_iCommand = m_usCmd;
    pUserData->m_iChannelID = m_iChannelID;

    TUserInfoTableValueType stRoleInfo;
    stRoleInfo.m_iUid = m_iUin;
    stRoleInfo.m_iGroupID = m_iGroupID;
    stRoleInfo.m_strNick = strNick;
    stRoleInfo.m_iCreateTime = CLogicCommon::GetSec();
    stRoleInfo.m_iPhyPower = CLogicConfig::GetGlobalConfig().m_iMaxEnergy;
    stRoleInfo.m_iLastAutoAddTime = CLogicCommon::GetSec();
    stRoleInfo.m_iNpcMailServer = CLogicConfig::GetSystemConfig().GetOpenSvrDay() - 1;
    stRoleInfo.m_iDataUpdateVersion = CLogicVersionDataUpdateSystem::EV_CurVersion;
    stRoleInfo.m_iAvatarID = CLogicConfig::GetAvatarConfig().m_iDefaultIconID;
    stRoleInfo.m_iAvatarBorder = CLogicConfig::GetAvatarConfig().m_iDefaultBorderID;
    pUserData->m_stUserInfoTable.Initialize(stRoleInfo);

    TUserDailyTableValueType stDailyValue;
    stDailyValue.m_iUid = m_iUin;
    stDailyValue.m_iGroupID = m_iGroupID;
    pUserData->m_stUserDailyTable.Initialize(stDailyValue);

    TUserTeamInfoTableValueType stTeamValue;
    stTeamValue.m_iUid = m_iUin;
    stTeamValue.m_iGroupID = m_iGroupID;
    stTeamValue.m_stTeam1.push_back(iCardID);
    stTeamValue.m_stTeam7.push_back(iCardID);
    pUserData->m_stUserTeamInfoTable.Initialize(stTeamValue);

    TUserIbusyInfoTableValueType stIbusyInfo;
    stIbusyInfo.m_iUid = m_iUin;
    stIbusyInfo.m_iGroupID = m_iGroupID;
    pUserData->m_stIBusyData.Initialize(stIbusyInfo);

    TUserPayDiamondTableValueType stPayDiamondValue;
    stPayDiamondValue.m_iUid = m_iUin;
    stPayDiamondValue.m_iGroupID = m_iGroupID;
    pUserData->m_stUserPayDiamondTable.Initialize(stPayDiamondValue);

    user_info_key_t stKey;
    stKey.m_stRoleIDDetail.m_iGroupID = m_iGroupID;
    stKey.m_stRoleIDDetail.m_iUin     = m_iUin;
    auto [iter, ret] = LOGIC_USER_DATA_HASHMAP_INS->insert(std::make_pair(stKey.m_ulRoleID, pUserData));
    if(!ret)
    {
        LOGIC_SERVICE_API_INS->RequestServiceAlert("create_role_error", {
                                                                            { "uin", std::to_string(m_iUin) },
                                                                            { "group_id", std::to_string(m_iGroupID) }
                                                                        });

        delete pUserData;

        return nullptr;
    }

    LOGIC_LOG_SLOW_ERROR << "INSERT INTO USER PTR TO CACHE|UIN:" << m_iUin << "|GROUP_ID:" << m_iGroupID << "|CACHE TOTAL USER:" << LOGIC_USER_DATA_HASHMAP_INS->size() << std::endl;

    // 添加道具
    CLogicCacheCommon::AddGameItem(pUserData, CLogicConfigDefine::HERO_CARD, iCardID, 1, m_usCmd);

    //添加初始化道具
    auto createInitConfig = CLogicConfig::GetAwardConfig();
    CLogicCacheCommon::AddGameItem(pUserData,createInitConfig.m_stInitReward,m_usCmd);

    return pUserData;
}

bool CLogicCreateRoleProcessor::DebugRefreshRoleData()
{
    // 只能在 local 模式才能使用
#if LOGIC_BUILD_PLATFORM == BUILD_PLATFORM_LOCAL

    m_pUserInfo->m_stUserDailyTable.SetLastRefreshTime(1);

    for(auto iterLevel = m_pUserInfo->m_stLevelInfoTableMap.Begin(); iterLevel != m_pUserInfo->m_stLevelInfoTableMap.End(); ++iterLevel)
    {
        iterLevel->second->SetSharedTimes(0);
        iterLevel->second->SetPlayTimes(0);
    }

#endif

    SendToClient(SERVER_ERRCODE::SUCCESS);
    return true;
}
