#include "server_errcode.h"
#include "server_cmd_logic_define.h"
#include "logic_link_common.h"
#include "logic_link_error.h"
#include "server_fight_protocol.h"
#include "server_task_protocol.h"
#include "server_card_protocol.h"
#include "logic_game_rank_list.h"
#include "logic_game_global.h"
#include "logic_game_rank_processor.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_config.h"
#include "logic_game_attribute.h"
#include "logic_game_cache_common.h"
#include "logic_game_publisher.h"
#include "logic_game_common_publisher.h"

REGISTER_CMD_FACTORY(MSG_LOGIC_UPDATE_FIGHT_POWER, CLogicRankProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_FIGHT_POWER_SELF_RANK, CLogicRankProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_FIGHT_POWER_RANK_LIST, CLogicRankProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_PVP_RANK_LIST, CLogicRankProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_PVP_LAST_DAY_RANK_LIST, CLogicRankProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_FIGHT_POWER_ACTIVE_STAGE_BONUS, CLogicRankProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_CLIMB_TOWER_RANK_LIST, CLogicRankProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_CLIMB_TOWER_SELF_RANK, CLogicRankProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_MILITARK_LEVEL_RANK, CLogicRankProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_CHARM_RANK_LIST, CLogicRankProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_MATCH_PVP_RANK_LIST, CLogicRankProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_IBUSY_GET_SCORE_RANK_LIST, CLogicRankProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_IBUSY_GET_SCORE_SELF_RANK, CLogicRankProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_FIGHT_REPORT, CLogicRankProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_SELF_SIMPLE_RANK_INFO, CLogicRankProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_TOUHOU_OPEN_COMMON_RANK, CLogicRankProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_TOUHOU_GET_COMMON_RANK_SELF_INFO, CLogicRankProcessor)

CLogicRankProcessor::CLogicRankProcessor(uint16_t usCmd, const std::string &strCmdName)
        : CLogicProcessorBase(usCmd, strCmdName)
{
}


CPackRankBaseCardInfo
CLogicRankProcessor::GetCardInfo(user_data_table_ptr_type pUserInfo, int iCardID)
{
    CPackRankBaseCardInfo stCardInfo;
    auto stCardIter = pUserInfo->m_stHeroCardTableMap.Find(iCardID);
    if(stCardIter != pUserInfo->m_stHeroCardTableMap.End())
    {
        stCardInfo.m_iCardID = iCardID;
        stCardInfo.m_iColor = stCardIter->second->GetCardColor();
        stCardInfo.m_iStar = stCardIter->second->GetCardStar();
        stCardInfo.m_iLevel = stCardIter->second->GetCardLevel();
    }
    
    return (stCardInfo);
}

std::vector<CPackRankBaseCardInfo> CLogicRankProcessor::GetTeamCardInfo(user_data_table_ptr_type pUserInfo, int32_t iType)
{
    std::vector<CPackRankBaseCardInfo> stTeamCardInfo;
    auto stTeam = CLogicCacheCommon::GetTeam(pUserInfo, iType);
    for(auto cardID : stTeam)
    {
        if(cardID > 0)
        {
            auto stCardInfoPack = GetCardInfo(pUserInfo, cardID);
            stTeamCardInfo.push_back(stCardInfoPack);
        }
    }
    
    return (stTeamCardInfo);
}

CPackGetPvpRankListData
CLogicRankProcessor::GetPVPPlayerInfo(int iGroupID, int iUin)
{
    CPackGetPvpRankListData stPlayerInfo;
	stPlayerInfo.m_iUid = iUin;
	stPlayerInfo.m_iGroupID = iGroupID;

    if(iGroupID > 0)
    {
        auto pUserInfo = CLogicCacheCommon::LoadUserData(iGroupID, iUin);
        if (pUserInfo)
        {
            stPlayerInfo.m_strNick = pUserInfo->m_strNick;
            stPlayerInfo.m_iLevel = pUserInfo->m_stUserInfoTable.GetLevel();
            stPlayerInfo.m_iLeaderCardID = CLogicCacheCommon::GetMainRoleCardID(pUserInfo);
            stPlayerInfo.m_stCardInfoList = GetTeamCardInfo(pUserInfo, CLogicConfigDefine::ETT_PvpTeam);
            stPlayerInfo.m_iAvatarIcon = pUserInfo->m_stUserInfoTable.GetAvatarID();
            stPlayerInfo.m_iAvatarBorder = pUserInfo->m_stUserInfoTable.GetAvatarBorder();
            if(stPlayerInfo.m_iAvatarIcon == 0)
                stPlayerInfo.m_iAvatarIcon = CLogicConfig::GetAvatarConfig().m_iDefaultIconID;
            if(stPlayerInfo.m_iAvatarBorder == 0)
                stPlayerInfo.m_iAvatarBorder = CLogicConfig::GetAvatarConfig().m_iDefaultBorderID;
        }
    }
    else
    {
        user_info_key_t stKey;
        stKey.m_stRoleIDDetail.m_iGroupID = iGroupID;
        stKey.m_stRoleIDDetail.m_iUin = iUin;
        
        auto stIter = CLogicConfig::GetPVPRobotConfig().m_stRobotList.find(stKey.m_ulRoleID);
        if(stIter != CLogicConfig::GetPVPRobotConfig().m_stRobotList.end())
        {
            stPlayerInfo.m_strNick = stIter->second.m_strNick;
            stPlayerInfo.m_iLevel = stIter->second.m_iLevel;
            stPlayerInfo.m_iTotalCombat = stIter->second.m_iTotalCombat;
            stPlayerInfo.m_iLeaderCardID = 10;
            stPlayerInfo.m_iAvatarIcon = stIter->second.m_iAvatarID == 0 ?
                CLogicConfig::GetAvatarConfig().m_iDefaultIconID : stIter->second.m_iAvatarID;
            stPlayerInfo.m_iAvatarBorder = stIter->second.m_iAvatarBorder == 0 ?
                CLogicConfig::GetAvatarConfig().m_iDefaultBorderID : stIter->second.m_iAvatarBorder;

            CPackRankBaseCardInfo stCardInfo;
            for(auto stCardIT = stIter->second.m_stCardInfoList.begin();
                     stCardIT != stIter->second.m_stCardInfoList.end();
                     stCardIT ++)
            {
                stCardInfo.m_iCardID = stCardIT->m_iCardID;
                // stCardInfo.m_iColor = stCardIT->m_iColor;
                // stCardInfo.m_iStar = stCardIT->m_iStar;
                // stCardInfo.m_iLevel = stCardIT->m_iLevel;
                
                stPlayerInfo.m_stCardInfoList.push_back(stCardInfo);
            }
        }
    }
    
    return (stPlayerInfo);
}

CPackClimbTowerRankListData
CLogicRankProcessor::GetClimbTowerPlayerInfo(int iGroupID, int iUin, int iScore)
{
	CPackClimbTowerRankListData stPlayerInfo;
	stPlayerInfo.m_iUid = iUin;
	stPlayerInfo.m_iGroupID = iGroupID;

	if (iGroupID > 0)
	{
		auto pUserInfo = CLogicCacheCommon::LoadUserData(iGroupID, iUin);
		if (pUserInfo)
		{
			stPlayerInfo.m_strNick = pUserInfo->m_strNick;
			stPlayerInfo.m_iLevel = pUserInfo->m_stUserInfoTable.GetLevel();
			stPlayerInfo.m_iTotalCombat = pUserInfo->m_stUserInfoTable.GetRoleCombat();
			stPlayerInfo.m_iLeaderCardID = CLogicCacheCommon::GetMainRoleCardID(pUserInfo);
			stPlayerInfo.m_stCardInfoList = GetTeamCardInfo(pUserInfo, CLogicConfigDefine::ETT_MainTeam);
			stPlayerInfo.m_iAvatarIcon = pUserInfo->m_stUserInfoTable.GetAvatarID();
			stPlayerInfo.m_iAvatarBorder = pUserInfo->m_stUserInfoTable.GetAvatarBorder();

			if (stPlayerInfo.m_iAvatarIcon == 0) stPlayerInfo.m_iAvatarIcon = CLogicConfig::GetAvatarConfig().m_iDefaultIconID;
			if (stPlayerInfo.m_iAvatarBorder == 0) stPlayerInfo.m_iAvatarBorder = CLogicConfig::GetAvatarConfig().m_iDefaultBorderID;
		}
	}
	else
	{
		user_info_key_t stKey;
		stKey.m_stRoleIDDetail.m_iGroupID = iGroupID;
		stKey.m_stRoleIDDetail.m_iUin = iUin;

		auto stIter = CLogicConfig::GetPVPRobotConfig().m_stRobotList.find(stKey.m_ulRoleID);
		if (stIter != CLogicConfig::GetPVPRobotConfig().m_stRobotList.end())
		{
			stPlayerInfo.m_strNick = stIter->second.m_strNick;
			stPlayerInfo.m_iLevel = stIter->second.m_iLevel;
			stPlayerInfo.m_iTotalCombat = stIter->second.m_iTotalCombat;

			for (auto stCardIT = stIter->second.m_stCardInfoList.begin();
				stCardIT != stIter->second.m_stCardInfoList.end();
				stCardIT++)
			{
				CPackRankBaseCardInfo stCardInfo;

				stCardInfo.m_iCardID = stCardIT->m_iCardID;
				// stCardInfo.m_iLevel = stCardIT->m_iLevel;
				// stCardInfo.m_iColor = stCardIT->m_iColor;
				// stCardInfo.m_iStar = stCardIT->m_iStar;

				stPlayerInfo.m_stCardInfoList.push_back(stCardInfo);
			}
		}

		stPlayerInfo.m_iAvatarIcon = CLogicConfig::GetAvatarConfig().m_iDefaultIconID;
		stPlayerInfo.m_iAvatarBorder = CLogicConfig::GetAvatarConfig().m_iDefaultBorderID;
	}

	stPlayerInfo.m_iTeamScore = iScore;

	return (stPlayerInfo);
}

CPackFightPowerRankListData
CLogicRankProcessor::GetFightPowerPlayerInfo(int iGroupID, int iUin)
{
    CPackFightPowerRankListData stPlayerInfo;
	stPlayerInfo.m_iUid = iUin;
	stPlayerInfo.m_iGroupID = iGroupID;

    auto pUserInfo = CLogicCacheCommon::LoadUserData(iGroupID, iUin);
    if(pUserInfo)
    {
        stPlayerInfo.m_strNick = pUserInfo->m_strNick;
        stPlayerInfo.m_iLevel = pUserInfo->m_stUserInfoTable.GetLevel();
        stPlayerInfo.m_iTotalCombat = pUserInfo->m_stUserInfoTable.GetRoleCombat();
        stPlayerInfo.m_iLeaderCardID = CLogicCacheCommon::GetMainRoleCardID(pUserInfo);
        stPlayerInfo.m_stCardInfoList = GetTeamCardInfo(pUserInfo, CLogicConfigDefine::ETT_MainTeam);
        stPlayerInfo.m_iAvatarIcon = pUserInfo->m_stUserInfoTable.GetAvatarID();
        stPlayerInfo.m_iAvatarBorder = pUserInfo->m_stUserInfoTable.GetAvatarBorder();

        if(stPlayerInfo.m_iAvatarIcon == 0) stPlayerInfo.m_iAvatarIcon = CLogicConfig::GetAvatarConfig().m_iDefaultIconID;
        if(stPlayerInfo.m_iAvatarBorder == 0) stPlayerInfo.m_iAvatarBorder = CLogicConfig::GetAvatarConfig().m_iDefaultBorderID;

		stPlayerInfo.m_iEquipHonorID = CLogicCacheCommon::GetEquipHonorID(pUserInfo);
    }

    return (stPlayerInfo);
}

bool 
CLogicRankProcessor::DoUserRun(const msgpack::unpacked& stFormatData)
{
    if((int)m_usCmd == MSG_LOGIC_UPDATE_FIGHT_POWER)
    {
        CRequestUpdateFightPower stParam;

        if(!DecodeMsgPack(stFormatData, stParam)) return (false);

        auto iPveFightPower = CLogicAttribute::CalcClientFightPower(m_pUserInfo, stParam.m_iFightPower1, stParam.m_iFightPower2);
        auto iEstimatePower = CLogicAttribute::CalcUserTeamFightPower(m_pUserInfo, CLogicConfigDefine::ETT_MainTeam);
        if (iEstimatePower < iPveFightPower)
        {
            PROCESSOR_LOG_ERROR << "FIGHT_POWER_CALC_ERROR|PVE_FIGHT_POWER:" << iPveFightPower << "|ESTIMATE_POWER:" << iEstimatePower << std::endl;
            iPveFightPower = iEstimatePower;
        }
        if (iPveFightPower > m_pUserInfo->m_stUserInfoTable.GetRoleCombat())
        {
            m_pUserInfo->m_stUserInfoTable.SetRoleCombat(iPveFightPower);

            int iRank = CLogicGlobal::UpdateOrInsertPowerRank(m_iUin, m_iGroupID, iPveFightPower);

            int iHighestRank = m_pUserInfo->m_stUserInfoTable.GetFightPowerHighestRank();
            if (!iHighestRank || (iHighestRank > iRank))
            {
                m_pUserInfo->m_stUserInfoTable.SetFightPowerHighestRank(iRank);
            }

            LOGIC_PUBLISHER_INS->PublishGameCombatInfo(m_pUserInfo);
        }

        // 记录最大战力
        if(m_pUserInfo->m_stUserInfoTable.GetRoleCombat() > m_pUserInfo->m_stUserInfoTable.GetRoleCombatMax())
        {
            CLogicEventData stEventData;
            stEventData.m_eType = CLogicConfigDefine::ECET_RoleCombatUp;
            stEventData.m_iPara = m_pUserInfo->m_stUserInfoTable.GetRoleCombatMax();
            stEventData.m_iParaEx = m_pUserInfo->m_stUserInfoTable.GetRoleCombat();
            stEventData.m_iCmd = m_usCmd;

            m_pUserInfo->m_stUserInfoTable.SetRoleCombatMax(m_pUserInfo->m_stUserInfoTable.GetRoleCombat());

            // 通用事件
            LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);

        }

        SendToClient(SERVER_ERRCODE::SUCCESS);

        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS" << std::endl;

        return (true);
    }
    else if((int)m_usCmd == MSG_LOGIC_GET_FIGHT_POWER_SELF_RANK)
    {
        CResponseGetFightPowerSelfRank stRsp;

        user_info_key_t stKey;

        stKey.m_stRoleIDDetail.m_iUin = m_iUin;
        stKey.m_stRoleIDDetail.m_iGroupID = m_iGroupID;

        stRsp.m_iRankNum = LOGIC_FIGHT_POWER_RANK_INS.GetRank(stKey.m_ulRoleID);

        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|RANK_NUM:" << stRsp.m_iRankNum << std::endl;

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    }
    else if((int)m_usCmd == MSG_LOGIC_GET_FIGHT_POWER_RANK_LIST)
    {
        CResponseGetFightPowerRankList stRsp;
        CRequestGetFightPowerRankList stParam;

        if(DecodeMsgPack(stFormatData, stParam) == false) return (false);

        int iMaxRankAmount = stParam.m_iRankAmount;
        if (iMaxRankAmount > CLogicConfigDefine::MAX_FIGHT_POWER_RANK_AMOUNT) 
            iMaxRankAmount = CLogicConfigDefine::MAX_FIGHT_POWER_RANK_AMOUNT;

        auto stItBeg = LOGIC_FIGHT_POWER_RANK_INS.begin();
        auto stItEnd = LOGIC_FIGHT_POWER_RANK_INS.end();

        int iRankAmount = 0;
        user_info_key_t stKey;
        CPackFightPowerRankListData stRankDataElem;
        for ( ; (iRankAmount < iMaxRankAmount) && (stItBeg != stItEnd); ++stItBeg, ++iRankAmount)
        {
            stKey.m_ulRoleID = stItBeg->first;
            
            stRankDataElem = GetFightPowerPlayerInfo(
                    stKey.m_stRoleIDDetail.m_iGroupID,
                    stKey.m_stRoleIDDetail.m_iUin);

            stRsp.m_stRankList.push_back(stRankDataElem);
        }

		stKey.m_stRoleIDDetail.m_iGroupID = m_pUserInfo->m_iGroupID;
		stKey.m_stRoleIDDetail.m_iUin = m_pUserInfo->m_iUin;
		stRsp.m_iSelfRankNum = LOGIC_FIGHT_POWER_RANK_INS.GetRank(stKey.m_ulRoleID);
		stRsp.m_iSelfHighestRank = m_pUserInfo->m_stUserInfoTable.GetFightPowerHighestRank();

        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|RANK_AMOUNT:" << iRankAmount << std::endl;

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    } 
    else if((int)m_usCmd == MSG_LOGIC_GET_PVP_RANK_LIST)
    {
        CResponseGetPvpRankList stRsp;
        
        user_info_key_t stKey;
        int iUserCount = CLogicConfigDefine::MAX_PVP_RANK_AMOUNT;

        auto stIterBeg = LOGIC_PVP_RANK_INS.begin();
        auto stIterEnd = LOGIC_PVP_RANK_INS.end();
        for (int i = 0; i < iUserCount && stIterBeg != stIterEnd; ++i, ++stIterBeg)
        {
            stKey.m_ulRoleID = stIterBeg->first;
            stRsp.m_stRankList.push_back(GetPVPPlayerInfo(
                        stKey.m_stRoleIDDetail.m_iGroupID,
                        stKey.m_stRoleIDDetail.m_iUin));
        }

        stKey.m_stRoleIDDetail.m_iGroupID = m_iGroupID;
        stKey.m_stRoleIDDetail.m_iUin = m_iUin;
        stRsp.m_iCurrentSelfRank = LOGIC_PVP_RANK_INS.GetRank(stKey.m_ulRoleID);
        stRsp.m_iMaxSelfRank = m_pUserInfo->m_stUserInfoTable.GetPvpHighestRank();
        
        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS" << std::endl;

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

        return (true);
    }
    else if((int)m_usCmd == MSG_LOGIC_GET_PVP_LAST_DAY_RANK_LIST)
    {
        CResponseGetPvpLastDayRankList stRsp;

        auto pstGlobalCache = CLogicGlobal::GetGlobalCache();

        for (auto stIter = pstGlobalCache->m_stLastDayPvpRank.Begin();
                  stIter != pstGlobalCache->m_stLastDayPvpRank.End();
                  stIter ++)
        {
            stRsp.m_stRankList.push_back(GetPVPPlayerInfo(stIter->second->GetGroupID(), stIter->second->GetUid()));
        }
        
        user_info_key_t stKey;
        stKey.m_stRoleIDDetail.m_iGroupID = m_iGroupID;
        stKey.m_stRoleIDDetail.m_iUin = m_iUin;
        stRsp.m_iCurrentSelfRank = LOGIC_PVP_RANK_INS.GetRank(stKey.m_ulRoleID);
        stRsp.m_iMaxSelfRank = m_pUserInfo->m_stUserInfoTable.GetPvpHighestRank();
        
        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS" << std::endl;

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

        return (true);
    }
    else if((int)m_usCmd == MSG_LOGIC_GET_FIGHT_POWER_ACTIVE_STAGE_BONUS)
    {
        CResponseFightPowerRankActiveStageBonus stRsp;
        CRequestFightPowerRankActiveStageBonus stParam;

        if (DecodeMsgPack(stFormatData, stParam) == false) return (false);

        int iStageBonus = m_pUserInfo->m_stUserInfoTable.GetFightPowerRankActive();
        if (iStageBonus & (1 << stParam.m_iIndex))
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << FIGHT_POWER_RANK_ACTIVE_STAGE_BONUS_HAS_RECEIVED
                                << "|MSG:" << GET_ERRMSG_BY_ERRCODE(FIGHT_POWER_RANK_ACTIVE_STAGE_BONUS_HAS_RECEIVED)
                                << std::endl;

            SEND_ERRCODE_AND_RET(FIGHT_POWER_RANK_ACTIVE_STAGE_BONUS_HAS_RECEIVED)
        }

        auto pstConfig = CLogicConfig::GetFightPowerRankActiveConfig().GetStageInfo(stParam.m_iIndex);
        if (!pstConfig)
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg()
                                << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        if (pstConfig->m_iFightPower > m_pUserInfo->m_stUserInfoTable.GetRoleCombat())
        {
			int32_t iEstimatePower = 0;
			if (pstConfig->m_iFightPower < m_pUserInfo->m_stUserInfoTable.GetRoleCombat() * 1.05)
			{//当服务器战力与目标相差不大时，计算下估值战斗力，如果估值战斗力也满足的话，则可以让他领奖
                iEstimatePower = CLogicAttribute::CalcUserTeamFightPower(m_pUserInfo, CLogicConfigDefine::ETT_MainTeam);
			}

			if (pstConfig->m_iFightPower > iEstimatePower)
			{
				using namespace SERVER_ERRCODE;
				PROCESSOR_LOG_ERROR << "ERRCODE:" << NOT_ENOUGH_FIGHT_POWER
					<< "|MSG:" << GET_ERRMSG_BY_ERRCODE(NOT_ENOUGH_FIGHT_POWER)
					<< std::endl;

				SEND_ERRCODE_AND_RET(NOT_ENOUGH_FIGHT_POWER)
			}
        }

        iStageBonus |= (1 << stParam.m_iIndex);

        m_pUserInfo->m_stUserInfoTable.SetFightPowerRankActive(iStageBonus);

        static std::string strBonusItem;
        CLogicCacheCommon::AddGameItem(m_pUserInfo, pstConfig->m_stBonusVec, strBonusItem, m_usCmd);

        stRsp.m_iIndex = stParam.m_iIndex;
        stRsp.m_stBonusVec = pstConfig->m_stBonusVec;

        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|INDEX:" << stParam.m_iIndex
                            << "|BONUS_ITEM:" << strBonusItem
                            << std::endl;

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

        return (true);
    }
	else if((int)m_usCmd == MSG_LOGIC_GET_CLIMB_TOWER_RANK_LIST)
	{
		CResponseGetClimbTowerRankList stRsp;
		CRequestGetClimbTowerRankList stParam;

		if (DecodeMsgPack(stFormatData, stParam) == false) return (false);

		int iMaxRankAmount = stParam.m_iRankAmount;
		if (iMaxRankAmount > CLogicConfigDefine::MAX_CLIMB_TOWER_RANK_AMOUNT)
			iMaxRankAmount = CLogicConfigDefine::MAX_CLIMB_TOWER_RANK_AMOUNT;

		auto stItBeg = LOGIC_CLIMB_TOWER_RANK_INS.begin();
		auto stItEnd = LOGIC_CLIMB_TOWER_RANK_INS.end();

		int iRankAmount = 0;
		user_info_key_t stKey;
		CPackClimbTowerRankListData stRankDataElem;
		for (; (iRankAmount < iMaxRankAmount) && (stItBeg != stItEnd); ++stItBeg, ++iRankAmount)
		{
			stKey.m_ulRoleID = stItBeg->first;
			
			stRankDataElem = GetClimbTowerPlayerInfo(
				stKey.m_stRoleIDDetail.m_iGroupID,
				stKey.m_stRoleIDDetail.m_iUin,
				stItBeg->second);

			stRsp.m_stRankList.push_back(stRankDataElem);
		}

		stKey.m_stRoleIDDetail.m_iUin = m_iUin;
		stKey.m_stRoleIDDetail.m_iGroupID = m_iGroupID;
		stRsp.m_iSelfRankNum = LOGIC_CLIMB_TOWER_RANK_INS.GetRank(stKey.m_ulRoleID);
		stRsp.m_iSelfHighestClimbTowerRank = m_pUserInfo->m_stUserInfoTable.GetClimbTowerHighestRank();

		PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|RANK_AMOUNT:" << iRankAmount << std::endl;

		SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
	}
	else if((int)m_usCmd == MSG_LOGIC_GET_CLIMB_TOWER_SELF_RANK)
	{
		CResponseGetClimbTowerSelfRank stRsp;

		user_info_key_t stKey;

		stKey.m_stRoleIDDetail.m_iUin = m_iUin;
		stKey.m_stRoleIDDetail.m_iGroupID = m_iGroupID;

		stRsp.m_iRankNum = LOGIC_CLIMB_TOWER_RANK_INS.GetRank(stKey.m_ulRoleID);
		stRsp.m_iHighestClimbTowerRank = m_pUserInfo->m_stUserInfoTable.GetClimbTowerHighestRank();

		PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|RANK_NUM:" << stRsp.m_iRankNum << "|HIGHEST_RANK:" << stRsp.m_iHighestClimbTowerRank << std::endl;

		SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
	}
	else if (m_usCmd == MSG_LOGIC_GET_MILITARK_LEVEL_RANK)
	{
		CResponseGetMilitarkLevelRank stRspBody;
		int32_t iRank = 1;
		for (auto iter = LOGIC_MILITARY_LEVEL_RANK_INS.begin(); iter != LOGIC_MILITARY_LEVEL_RANK_INS.end() && iRank <= 30; ++iter, ++iRank)
		{
			const user_info_key_t stKey(iter->first);
			const auto pUserInfo = CLogicCacheCommon::LoadUserData(stKey.m_stRoleIDDetail.m_iGroupID, stKey.m_stRoleIDDetail.m_iUin);
			if (nullptr != pUserInfo)
			{
				stRspBody.m_stRank.push_back(CLogicCacheCommon::GetUserRankInfo(pUserInfo, iRank, iter->second));
			}
		}
		const user_info_key_t stSelfKey(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID);
		stRspBody.m_iSelfRankNum = LOGIC_MILITARY_LEVEL_RANK_INS.GetRank(stSelfKey.m_ulRoleID);
		stRspBody.m_iSelfMilitarkLevel = m_pUserInfo->m_stUserInfoTable.GetEntireWarMilitarkLevel();
		SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
		return (true);
	}
	else if (m_usCmd == MSG_LOGIC_GET_CHARM_RANK_LIST)
	{
		CResponseGetCharmRank stRspBody;
		int32_t iRank = 1;
		for (auto iter = LOGIC_CHARM_RANK_INS.begin(); iter != LOGIC_CHARM_RANK_INS.end() && iRank <= 30; ++iter, ++iRank)
		{
			const user_info_key_t stKey(iter->first);
			const auto pUserInfo = CLogicCacheCommon::LoadUserData(stKey.m_stRoleIDDetail.m_iGroupID, stKey.m_stRoleIDDetail.m_iUin);
			if (nullptr != pUserInfo)
			{
				stRspBody.m_stRank.push_back(CLogicCacheCommon::GetUserRankInfo(pUserInfo, iRank, iter->second));
			}
		}
		const user_info_key_t stSelfKey(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID);
		stRspBody.m_iSelfRankNum = LOGIC_CHARM_RANK_INS.GetRank(stSelfKey.m_ulRoleID);
		stRspBody.m_iCharm = m_pUserInfo->m_stUserInfoTable.GetCharm();

		SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
		return (true);
	}
	else if (m_usCmd == MSG_LOGIC_GET_MATCH_PVP_RANK_LIST)
	{
		CResponseGetMatchPvpRank stRspBody;
		int32_t iRank = 1;
		for (auto iter = LOGIC_MATCH_PVP_RANK_INS.begin(); iter != LOGIC_MATCH_PVP_RANK_INS.end() && iRank <= 30; ++iter, ++iRank)
		{
			const user_info_key_t stKey(iter->first);
			const auto pUserInfo = CLogicCacheCommon::LoadUserData(stKey.m_stRoleIDDetail.m_iGroupID, stKey.m_stRoleIDDetail.m_iUin);
			if (nullptr != pUserInfo)
			{
				stRspBody.m_stRank.push_back(CLogicCacheCommon::GetUserRankInfo(pUserInfo, iRank, iter->second));
			}
		}
		const user_info_key_t stSelfKey(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID);
		stRspBody.m_iSelfRankNum = LOGIC_MATCH_PVP_RANK_INS.GetRank(stSelfKey.m_ulRoleID);
		stRspBody.m_iSelfPvpScore = m_pUserInfo->m_stUserInfoTable.GetMatchScore();

		SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
		return (true);
	}
	else if ((int)m_usCmd == MSG_LOGIC_IBUSY_GET_SCORE_SELF_RANK)
	{
        CResponseGetFightPowerSelfRank stRsp;

		user_info_key_t stKey;

		stKey.m_stRoleIDDetail.m_iUin = m_iUin;
		stKey.m_stRoleIDDetail.m_iGroupID = m_iGroupID;

		stRsp.m_iRankNum = LOGIC_IBUSY_SCORE_RANK_INS.GetRank(stKey.m_ulRoleID);

		PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|RANK_NUM:" << stRsp.m_iRankNum << std::endl;

		SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
	}
	else if ((int)m_usCmd == MSG_LOGIC_IBUSY_GET_SCORE_RANK_LIST)
	{
		CResponseGetIbusyScoreRank stRspBody;
	
		int iMaxRankAmount = CLogicConfigDefine::MAX_FIGHT_POWER_RANK_AMOUNT;

		int32_t iRank = 1;
		for (auto iter = LOGIC_IBUSY_SCORE_RANK_INS.begin(); iter != LOGIC_IBUSY_SCORE_RANK_INS.end() && iRank <= iMaxRankAmount; ++iter, ++iRank)
		{
			const user_info_key_t stKey(iter->first);
			const auto pUserInfo = CLogicCacheCommon::LoadUserData(stKey.m_stRoleIDDetail.m_iGroupID, stKey.m_stRoleIDDetail.m_iUin);
			if (nullptr != pUserInfo)
			{
				stRspBody.m_stRank.push_back(CLogicCacheCommon::GetUserRankInfo(pUserInfo, iRank, iter->second));
			}
		}
		const user_info_key_t stSelfKey(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID);
		stRspBody.m_iSelfRankNum = LOGIC_IBUSY_SCORE_RANK_INS.GetRank(stSelfKey.m_ulRoleID);
		stRspBody.m_iScore = m_pUserInfo->m_stIBusyData.GetScore();

		SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	}
	else if ((int)m_usCmd == MSG_LOGIC_GET_FIGHT_REPORT)
    {
        //1.解析协议
        CResponseGetFightReport	stRsp;
        CRequestGetFightReport	stParam;

        if (!DecodeMsgPack(stFormatData, stParam)) return (false);

        //2.返回战报信息
        auto typeReport = m_pUserInfo->m_stFightReportMap.find(stParam.m_iReportType);
        if (typeReport != m_pUserInfo->m_stFightReportMap.end())
        {
            for (auto iter = typeReport->second.Begin(); iter != typeReport->second.End(); ++iter)
            {
                CPackFightReportItem reportItem;
                reportItem.m_iReportType = stParam.m_iReportType;
                reportItem.m_iReportID = iter->first;
                reportItem.m_strReportContent = iter->second->GetReportContent();
                reportItem.m_cFightResult = iter->second->GetFightResult();
                reportItem.m_stParams = iter->second->GetParams();

                stRsp.m_stFightReports.emplace(reportItem);
            }
        }

        //全服战报
        typeReport = LOGIC_GLOBAL_CACHE_INS->m_stFightReportMap.find(stParam.m_iReportType);
        if (typeReport != LOGIC_GLOBAL_CACHE_INS->m_stFightReportMap.end())
        {
            for (auto iter = typeReport->second.Begin(); iter != typeReport->second.End(); ++iter)
            {
                CPackFightReportItem reportItem;
                reportItem.m_iReportType = stParam.m_iReportType;
                reportItem.m_iReportID = iter->first;
                reportItem.m_strReportContent = iter->second->GetReportContent();
                reportItem.m_cFightResult = iter->second->GetFightResult();
                reportItem.m_stParams = iter->second->GetParams();

                stRsp.m_stFightReports.emplace(reportItem);
            }
        }

        /*
        //公会战报
        auto* pstGuild = CLogicGuild::GetGuild(m_pUserInfo);
        if (pstGuild)
        {
            auto guild_reports = pstGuild->GetGuildFightReportListPack(stParam.m_iReportType);
            stRsp.m_stFightReports.insert(guild_reports.begin(), guild_reports.end());
        }
         */

        //如果超过最大数量，则做删减
        size_t maxCount = CLogicConfig::GetFightReportConfig().GetMaxFightReportCount(stParam.m_iReportType);
        while (stRsp.m_stFightReports.size() > uint32_t(maxCount))
        {
            stRsp.m_stFightReports.erase(stRsp.m_stFightReports.begin());
        }

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    }
	else if((int)m_usCmd == MSG_LOGIC_GET_SELF_SIMPLE_RANK_INFO)
    {
        CResponseGetSelfSimpleRankInfo stRsp;

        user_info_key_t stKey;
        stKey.m_stRoleIDDetail.m_iGroupID = m_iGroupID;
        stKey.m_stRoleIDDetail.m_iUin = m_iUin;
        // PVP排行榜
        stRsp.m_iPVPRankNum = LOGIC_PVP_RANK_INS.GetRank(stKey.m_ulRoleID);
        stRsp.m_iPVPRankMaxNum = m_pUserInfo->m_stUserInfoTable.GetPvpHighestRank();
        // 巅峰对决
        stRsp.m_iMatchPVPScore = m_pUserInfo->m_stUserInfoTable.GetMatchScore();
        // 爬塔
        stRsp.m_iTowerRankCurNum = LOGIC_CLIMB_TOWER_RANK_INS.GetRank(stKey.m_ulRoleID);
        stRsp.m_iTowerMaxOrder = m_pUserInfo->m_stUserInfoTable.GetClimbTowerMaxOrder();
        // 全民战役
        stRsp.m_iEntireWarMilitarkLevel = m_pUserInfo->m_stUserInfoTable.GetEntireWarMilitarkLevel();

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    }
    else if((int)m_usCmd == MSG_LOGIC_TOUHOU_OPEN_COMMON_RANK)
    {
        CRequestOpenCommonRank stReq;
        CResponseOpenCommonRank stRsp;

        if(!DecodeMsgPack(stFormatData, stReq)) return (false);

        stRsp.m_iRankID = stReq.m_iRankID;

        auto iterRank = CLogicGlobal::GetGlobalCache()->m_stCommonRankMap.find(stReq.m_iRankID);
        if(iterRank == CLogicGlobal::GetGlobalCache()->m_stCommonRankMap.end())
        {
            SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
            return (true);
        }

        int32_t iRank = 1;
        for (auto iter = iterRank->second.begin(); iter != iterRank->second.end() && iRank <= 30; ++iter, ++iRank)
        {
            const user_info_key_t stKey(iter->first);
            const auto pUserInfo = CLogicCacheCommon::LoadUserData(stKey.m_stRoleIDDetail.m_iGroupID, stKey.m_stRoleIDDetail.m_iUin);
            if (nullptr != pUserInfo)
            {
                stRsp.m_stRank.push_back(CLogicCacheCommon::GetUserRankInfo(pUserInfo, iRank, iter->second));
            }
        }

        const user_info_key_t stSelfKey(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID);
        auto iterMyRank = iterRank->second.find(stSelfKey.m_ulRoleID);
        if(iterMyRank != iterRank->second.end())
        {
            stRsp.m_iSelfRankNum = iterRank->second.GetRank(iterMyRank);
            stRsp.m_iValue = iterMyRank->second;
        }

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
        return (true);
    }
    else if((int)m_usCmd == MSG_LOGIC_TOUHOU_GET_COMMON_RANK_SELF_INFO)
    {
        CRequestOpenCommonRank stReq;
        CResponseGetCommonRankSelfInfo stRsp;

        if(!DecodeMsgPack(stFormatData, stReq)) return (false);

        stRsp.m_iRankID = stReq.m_iRankID;

        auto iterRank = CLogicGlobal::GetGlobalCache()->m_stCommonRankMap.find(stReq.m_iRankID);
        if(iterRank == CLogicGlobal::GetGlobalCache()->m_stCommonRankMap.end())
        {
            SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
            return (true);
        }

        const user_info_key_t stSelfKey(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID);
        auto iterMyRank = iterRank->second.find(stSelfKey.m_ulRoleID);
        if(iterMyRank != iterRank->second.end())
        {
            stRsp.m_iSelfRankNum = iterRank->second.GetRank(iterMyRank);
            stRsp.m_iValue = iterMyRank->second;
        }

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
        return (true);
    }

    return (true);
}
