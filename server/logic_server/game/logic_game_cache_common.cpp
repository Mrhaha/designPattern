#include "server_errcode.h"
#include "server_cmd_logic_define.h"
#include "server_cmd_admin_define.h"
#include "logic_game_cache_common.h"
#include "logic_config.h"
#include "logic_game_item.h"
#include "logic_game_global.h"
#include "logic_game_user_notify_system.h"
#include "logic_game_daily_task_system.h"
#include "logic_game_publisher.h"
#include "logic_record_server_nick_table.h"
#include "logic_game_item_factory.h"
#include "logic_game_monitor.h"
#include "logic_game_common_publisher.h"
#include "server_cmd_data_report_define.h"
#include "server_data_report_protocol.h"


list<pair<int, int>>
CLogicCacheCommon::GetCacheUserID(int iCount)
{
    std::list<std::pair<int, int>> stUser;

    for (auto stIter = LOGIC_USER_DATA_HASHMAP_INS->begin();
              stIter != LOGIC_USER_DATA_HASHMAP_INS->end();
              stIter ++)
    {
        user_info_key_t stKey;
        stKey.m_ulRoleID = stIter->first;
        stUser.push_back(std::make_pair(stKey.m_stRoleIDDetail.m_iGroupID,
                                        stKey.m_stRoleIDDetail.m_iUin));

        if (--iCount <= 0) break;
    }

    return (stUser);
}

user_data_table_ptr_type
CLogicCacheCommon::LoadUserData(int iGroupID, int iUin,bool bNotMove)
{
    user_info_key_t stKey;
    stKey.m_stRoleIDDetail.m_iGroupID = iGroupID;
    stKey.m_stRoleIDDetail.m_iUin = iUin;

//    if (bNotMove)
//    {
    auto stUserDetailIT = LOGIC_USER_DATA_HASHMAP_INS->FindNoMove(stKey.m_ulRoleID);
    if (stUserDetailIT != LOGIC_USER_DATA_HASHMAP_INS->end())
    {
        return (stUserDetailIT->second);
    }
    else
    {
        return (GetUserDataFromDB(iGroupID, iUin));
    }
//    } else if ()
//    {
//        auto stUserDetailIT = LOGIC_USER_DATA_HASHMAP_INS->find(stKey.m_ulRoleID);
//        if (stUserDetailIT != LOGIC_USER_DATA_HASHMAP_INS->end())
//        {
//            return (stUserDetailIT->second);
//        }
//        else
//        {
//            return (GetUserDataFromDB(iGroupID, iUin));
//        }
//    }
}

user_data_table_ptr_type
CLogicCacheCommon::GetInCacheUserData(int iGroupID, int iUin)
{
    user_info_key_t stKey;
    stKey.m_stRoleIDDetail.m_iGroupID = iGroupID;
    stKey.m_stRoleIDDetail.m_iUin = iUin;

    auto stUserDetailIT = LOGIC_USER_DATA_HASHMAP_INS->FindNoMove(stKey.m_ulRoleID);
    if (stUserDetailIT != LOGIC_USER_DATA_HASHMAP_INS->end())
    {
        return (stUserDetailIT->second);
    }

    return (nullptr);
}

bool CLogicCacheCommon::IsBanAccount(user_data_table_ptr_type pUserData)
{
	return ((pUserData->m_stUserInfoTable.GetBanEndTime() > CLogicCommon::GetSec() &&
             CLogicConfigDefine::EUBT_BanUser == pUserData->m_stUserInfoTable.GetBanType()) || IsBanDelUser(pUserData));
}

bool CLogicCacheCommon::IsBanDelUser(user_data_table_ptr_type pUserData)
{
    return (pUserData->m_stUserInfoTable.GetBanEndTime() > CLogicCommon::GetSec() && CLogicConfigDefine::EUBT_DelUser == pUserData->m_stUserInfoTable.GetBanType());
}

bool CLogicCacheCommon::IsBanTalk(user_data_table_ptr_type pUserData)
{
	return (pUserData->m_stUserInfoTable.GetBanEndTime() > CLogicCommon::GetSec() && CLogicConfigDefine::EUBT_BanTalk == pUserData->m_stUserInfoTable.GetBanType());
}

int32_t CLogicCacheCommon::GetUserPrimaryKeyIndex(user_data_table_ptr_type pUserData)
{
    int32_t iKey = pUserData->m_stUserInfoTable.GetPrimaryKeyIndex();
    pUserData->m_stUserInfoTable.AddPrimaryKeyIndex(1);
    return iKey;
}

CPackUserBrief CLogicCacheCommon::GetUserBriefInfo(user_data_table_ptr_type pUserData)
{
	CPackUserBrief stUser;
	stUser.m_iUid = pUserData->m_iUin;
	stUser.m_iGroupID = pUserData->m_iGroupID;
	stUser.m_strNick = pUserData->m_strNick;
    stUser.m_iLevel = pUserData->m_stUserInfoTable.GetLevel();
	stUser.m_iAvatarID = pUserData->m_stUserInfoTable.GetAvatarID();
	stUser.m_iAvatarBorder = pUserData->m_stUserInfoTable.GetAvatarBorder();
    stUser.m_iLastLogoutTime = pUserData->m_stUserInfoTable.GetLastLogout();
    stUser.m_iCombat = pUserData->m_stUserInfoTable.GetRoleCombat();

	return stUser;
}

CPackUserBriefCard CLogicCacheCommon::GetCardBriefInfo(user_data_table_ptr_type pUserData, int32_t iCardID,int32_t iPosition)
{
	CPackUserBriefCard stBrief;
	const auto stIter = pUserData->m_stHeroCardTableMap.Find(iCardID);
	if (stIter != pUserData->m_stHeroCardTableMap.End())
	{
		stBrief.m_iCardID = iCardID;
		stBrief.m_iStar = stIter->second->GetCardStar();
        stBrief.m_iLevel = stIter->second->GetCardLevel();
		stBrief.m_iColor = stIter->second->GetCardColor();
        stBrief.m_stTalentMap = stIter->second->GetTalentMap();
        stBrief.m_iEquipFashionID = stIter->second->GetFashionID();
        stBrief.m_iPosition = iPosition;

        // 装备的星纹
        std::vector<int32_t> stUniqueIDs;
        stUniqueIDs.push_back(stIter->second->GetEquipConstellation1());
        stUniqueIDs.push_back(stIter->second->GetEquipConstellation2());
        stUniqueIDs.push_back(stIter->second->GetEquipConstellation3());
        stUniqueIDs.push_back(stIter->second->GetEquipConstellation4());

        for(auto id : stUniqueIDs)
        {
            if(id <= 0) continue;
            auto iterConstel = pUserData->m_stConstelMap.Find(id);
            if(iterConstel != pUserData->m_stConstelMap.End())
            {
                stBrief.m_stConstelVec.push_back(CLogicCacheCommon::GetConstellationInfo(iterConstel->second->m_stData));
            }
        }

        // 装备
        std::vector<int32_t> stEquipIDs;
        stEquipIDs.push_back(stIter->second->GetEquip1());
        stEquipIDs.push_back(stIter->second->GetEquip2());
        stEquipIDs.push_back(stIter->second->GetEquip3());
        stEquipIDs.push_back(stIter->second->GetEquip4());
        for(auto id : stEquipIDs)
        {
            if(id <= 0) continue;
            auto iterEquip = pUserData->m_stEquipMap.Find(id);
            if(iterEquip != pUserData->m_stEquipMap.End())
            {
                stBrief.m_stEquipVec.push_back(GetEquipInfo(iterEquip->second->m_stData));
            }
        }
	}

	return stBrief;
}

CPackUserMatch CLogicCacheCommon::GetUserCrossInfo(user_data_table_ptr_type pUserData)
{
	assert(nullptr != pUserData && "get user cross but data nullptr");

	CPackUserMatch stPack;
	stPack.m_iUin = pUserData->m_iUin;
	stPack.m_iGroupID = pUserData->m_iGroupID;
	stPack.m_iMatchLevel = CLogicConfig::GetMatchPvpConfig().GetMatchLevelByScore(pUserData->m_stUserInfoTable.GetMatchScore());

	return stPack;
}

CPackUserRank CLogicCacheCommon::GetUserRankInfo(user_data_table_ptr_type pUserData, int32_t iRank, int32_t iRankScore)
{
	CPackUserRank stUser;
	stUser.m_iUid = pUserData->m_iUin;
	stUser.m_iGroupID = pUserData->m_iGroupID;
	stUser.m_iRank = iRank;
	stUser.m_strNick = pUserData->m_strNick;
	stUser.m_iLevel = pUserData->m_stUserInfoTable.GetLevel();
	stUser.m_iAvatarIcon = pUserData->m_stUserInfoTable.GetAvatarID();
	stUser.m_iAvatarBorder = pUserData->m_stUserInfoTable.GetAvatarBorder();
	stUser.m_iRankScore = iRankScore;
	stUser.m_iFightPower = pUserData->m_stUserInfoTable.GetRoleCombat();
	stUser.m_iVipLevel = pUserData->m_stUserInfoTable.GetVipLevel();
	stUser.m_iShowGroupID = CLogicConfig::GetSystemConfig().GetGroupShowID(pUserData->m_iGroupID);
	stUser.m_strGroupName = CLogicConfig::GetSystemConfig().GetGroupName(pUserData->m_iGroupID);

	return stUser;
}

CPackHonorInfo CLogicCacheCommon::GetHonorInfo(const TUserHonorTableValueType& value)
{
	CPackHonorInfo stHonor;
	stHonor.m_iHonorID = value.m_iHonorID;
	stHonor.m_iHonorStatus = value.m_iHonorStatus;
	stHonor.m_iExpireTime = value.m_iExpireTime;
	stHonor.m_iProgressVec.push_back(value.m_iTaskProgress1);
	stHonor.m_iProgressVec.push_back(value.m_iTaskProgress2);
	stHonor.m_iProgressVec.push_back(value.m_iTaskProgress3);
	stHonor.m_iProgressVec.push_back(value.m_iTaskProgress4);
	stHonor.m_iProgressVec.push_back(value.m_iTaskProgress5);
	return stHonor;
}

CPackConstellationInfo CLogicCacheCommon::GetConstellationInfo(const TUserConstellationTableValueType& value)
{
    CPackConstellationInfo stConstell;
    stConstell.m_iUniqueID      = value.m_iUniqueID;
    stConstell.m_iGrade         = value.m_iGrade;
    stConstell.m_iEquipmentTag  = value.m_iEquipmentTag;
    stConstell.m_iSuit          = value.m_iSuit;
    stConstell.m_iLevel         = value.m_iLevel;
    stConstell.m_iExp           = value.m_iExp;
    stConstell.m_iTotalExp      = value.m_iTotalExp;
    stConstell.m_iEquipCardID   = value.m_iEquipCardID;
    stConstell.m_iIsLocked      = value.m_cIsLocked;
    stConstell.m_iIsDiscard     = value.m_cIsDiscard;

    stConstell.m_iMainAttrType  = value.m_iMainAttrType;
    stConstell.m_stExAttr       = value.m_stExAttr;

    return stConstell;
}

CPackEquipInfo CLogicCacheCommon::GetEquipInfo(const TUserEquipTableValueType& value)
{
    CPackEquipInfo stEquip;
    stEquip.m_iUniqueID     = value.m_iUniqueID;
    stEquip.m_iGrade        = value.m_iGrade;
    stEquip.m_iEquipmentTag = value.m_iEquipmentTag;
    stEquip.m_iAttrIndex    = value.m_iAttrIndex;
    stEquip.m_iIsLocked     = value.m_cIsLocked;

    return stEquip;
}

CPackFashionInfo CLogicCacheCommon::GetFashionInfo(const TFashionInfoTableValueType& value)
{
    CPackFashionInfo stInfo;
    stInfo.m_iFashionID = value.m_iFashionID;
    stInfo.m_iExpiredTime = value.m_iExpiredTime;

    return stInfo;
}

CPackHomeExploreInfo CLogicCacheCommon::GetExploreQuestInfo(const TUserHomeExploreTableValueType& value)
{
    CPackHomeExploreInfo stInfo;
    stInfo.m_iQuestID = value.m_iExploreQuestID;
    stInfo.m_iEndTime = value.m_iEndTime;
    stInfo.m_stTeam = value.m_stTeam;
    stInfo.m_cState = value.m_cState;
    stInfo.m_bIsCbt = value.m_cIsCbt;

    return stInfo;
}

CPackAvatarInfo CLogicCacheCommon::GetAvatarInfo(const TUserAvatarTableValueType& value)
{
    CPackAvatarInfo stInfo;
    stInfo.m_iAvatarID = value.m_iAvatarID;
    stInfo.m_iAvatarType = value.m_iAvatarType;
    stInfo.m_iTime = value.m_iTime;

    return stInfo;
}

CPackAvyInfo CLogicCacheCommon::GetUserAvyInfoPack(const TUserAvyTableValueType& stValue)
{
    CPackAvyInfo stInfo;
    stInfo.m_iAvyID = stValue.m_iAvyID;
    stInfo.m_strCompleteBitmap = int2bin(stValue.m_ulCompleteBitmap);

    return stInfo;
}

CPackLevelInfo CLogicCacheCommon::GetUserLevelInfo(const TLevelInfoTableValueType& stValue)
{
    CPackLevelInfo stInfo;
    stInfo.m_iLevelID = stValue.m_iLevelID;
    stInfo.m_cLevelStar = stValue.m_cLevelStar;
    stInfo.m_cBonusTag = stValue.m_cBonusTag;
    stInfo.m_cAchieveTag = stValue.m_cAchieveTag;
    stInfo.m_bIsPass = (stValue.m_iTotalVictoryTimes > 0);
    stInfo.m_iCostTime = stValue.m_iCostTime;
    if(!LOGIC_CHECK_DAY_REFRESH(stValue.m_iLastPlayTime))
    {
        stInfo.m_cSharedTimes = stValue.m_iSharedTimes;
        stInfo.m_cPlayTimes = stValue.m_iPlayTimes;
        stInfo.m_bAdvRewardTimes = stValue.m_iAdvRewardTimes;
    }

    if(!LOGIC_CHECK_DAY_REFRESH(stValue.m_iLastResetTime))
    {
        stInfo.m_cResetTimes = stValue.m_iResetTimes;
    }

    return stInfo;
}

int32_t CLogicCacheCommon::GetEquipHonorID(user_data_table_ptr_type pUserData)
{
	for (auto iter = pUserData->m_stUserHonorMap.Begin(); iter != pUserData->m_stUserHonorMap.End(); ++iter)
	{
		if (0 == iter->second->GetExpireTime() || CLogicCommon::GetSec() < iter->second->GetExpireTime())
		{
			if (CLogicConfigDefine::HONOR_STATUS_EQUIP == iter->second->GetHonorStatus())
			{
				return iter->first;
			}
		}
	}
	return 0;
}

CPackPvpChallengerInfo CLogicCacheCommon::GetPvpChallengerInfo(unsigned int uiRank)
{
	CPackPvpChallengerInfo stPlayerInfo;
	auto stIter = LOGIC_PVP_RANK_INS.FindByRank(uiRank);
	if (stIter != LOGIC_PVP_RANK_INS.end())
	{
		user_info_key_t stKey;
		stKey.m_ulRoleID = stIter->first;
		stPlayerInfo = GetPvpChallengerInfo(stKey.m_stRoleIDDetail.m_iGroupID, stKey.m_stRoleIDDetail.m_iUin);
	}

	return (stPlayerInfo);
}

CPackPvpChallengerInfo CLogicCacheCommon::GetPvpChallengerInfo(int iGroupID, int iUin)
{
	CPackPvpChallengerInfo stPlayerInfo;
	if (iGroupID > 0)
	{
		auto pUserInfo = CLogicCacheCommon::LoadUserData(iGroupID, iUin);
		if (pUserInfo)
		{
            stPlayerInfo.m_stBriefInfo = GetUserBriefInfo(pUserInfo);

            auto stTeam = CLogicCacheCommon::GetTeam(pUserInfo, CLogicConfigDefine::ETT_PvpDefense);
            for(std::size_t i = 0; i < stTeam.size(); ++i)
            {
                if(stTeam[i] <= 0) continue;

                stPlayerInfo.m_stCardInfoList.push_back(GetPvpChallengerAttackCard(pUserInfo, stTeam[i], i+1));
            }
		}
	}
	else
	{
        auto pElem = CLogicConfig::GetPVPRobotConfig().GetConfig(iUin, iGroupID);
        if (pElem)
        {
            stPlayerInfo.m_stBriefInfo.m_strNick = pElem->m_strNick;
            stPlayerInfo.m_stBriefInfo.m_iGroupID = iGroupID;
            stPlayerInfo.m_stBriefInfo.m_iUid = iUin;
            stPlayerInfo.m_stBriefInfo.m_iLevel = pElem->m_iLevel;
            stPlayerInfo.m_stBriefInfo.m_iAvatarID = pElem->m_iAvatarID;
            stPlayerInfo.m_stBriefInfo.m_iAvatarBorder = pElem->m_iAvatarBorder;
            stPlayerInfo.m_stBriefInfo.m_iCombat = pElem->m_iTotalCombat;
            for(auto& stCard : pElem->m_stCardInfoList)
            {
                CPackPvpChallengerCardInfo stCardInfo;
                stCardInfo.m_iCardID = stCard.m_iCardID;
                stCardInfo.m_iColor = stCard.m_iAtkPara;
                stCardInfo.m_iStar = stCard.m_iHpPara;
                stCardInfo.m_iLevel = stCard.m_iDiffculty;
                stCardInfo.m_iRobotSkillLevelIndex = stCard.m_iSkillLevelIndex;
                stCardInfo.m_iRobotDefPara = stCard.m_iDefPara;
                stPlayerInfo.m_stCardInfoList.push_back(stCardInfo);
            }
        }
	}

	return (stPlayerInfo);
}

CPackUserDetailInfo CLogicCacheCommon::GetUserDetailInfo(int iGroupID, int iUin, int iTeamType/* = 0*/)
{
    CPackUserDetailInfo stInfo;
    if (iGroupID > 0)
    {
        auto pUserInfo = CLogicCacheCommon::LoadUserData(iGroupID, iUin,true);
        if(nullptr == pUserInfo) return stInfo;

        stInfo.m_iGuildID = pUserInfo->m_stUserInfoTable.GetGuildID();
        stInfo.m_strGuildName = pUserInfo->m_stUserInfoTable.GetGuildName();
        stInfo.m_stBriefInfo = GetUserBriefInfo(pUserInfo);
        stInfo.m_iTeamType = iTeamType;
        stInfo.m_bUnLockHome = !pUserInfo->m_stHomeMap.Empty();
        if(stInfo.m_iTeamType == 0) stInfo.m_iTeamType = CLogicConfigDefine::ETT_MainTeam;

        auto stTeam = CLogicCacheCommon::GetTeam(pUserInfo, stInfo.m_iTeamType);
        int32_t iPosition = 0;
        for(auto card_id : stTeam)
        {
            if(card_id <= 0) continue;

            stInfo.m_stTeamBrief.push_back(GetCardBriefInfo(pUserInfo, card_id,iPosition++));
        }
    }
    else
    {
        auto pElem = CLogicConfig::GetPVPRobotConfig().GetConfig(iUin, iGroupID);
        if (pElem)
        {
            stInfo.m_stBriefInfo.m_strNick = pElem->m_strNick;
        }

        if((uint32_t)iUin == LOGIC_PVP_RANK_INS.max_rank()+1)
        {
            pElem = &(CLogicConfig::GetPVPRobotConfig().m_stBaseRobot);
            stInfo.m_stBriefInfo.m_strNick = CLogicConfig::GetPVPRobotConfig().m_stBotsNameList[
                CLogicCommon::GetRandNum() % CLogicConfig::GetPVPRobotConfig().m_stBotsNameList.size()];
        }

        if(pElem)
        {
            stInfo.m_stBriefInfo.m_iGroupID = iGroupID;
            stInfo.m_stBriefInfo.m_iUid = iUin;
            stInfo.m_stBriefInfo.m_iLevel = pElem->m_iLevel;
            stInfo.m_stBriefInfo.m_iAvatarID = pElem->m_iAvatarID;
            stInfo.m_stBriefInfo.m_iAvatarBorder = pElem->m_iAvatarBorder;
            stInfo.m_stBriefInfo.m_iCombat = pElem->m_iTotalCombat;
            for(auto& stCard : pElem->m_stCardInfoList)
            {
                CPackUserBriefCard stCardInfo;
                stCardInfo.m_iCardID = stCard.m_iCardID;
                stCardInfo.m_iColor = stCard.m_iAtkPara;
                stCardInfo.m_iStar = stCard.m_iHpPara;
                stCardInfo.m_iLevel = stCard.m_iDiffculty;
                stInfo.m_stTeamBrief.push_back(stCardInfo);
            }
        }
    }

    return (stInfo);
}

CPackPvpUserInfo CLogicCacheCommon::GetPvpUserInfo(int iGroupID, int iUin)
{
    user_info_key_t stKey;
    stKey.m_stRoleIDDetail.m_iGroupID = iGroupID;
    stKey.m_stRoleIDDetail.m_iUin = iUin;
    CPackPvpUserInfo stPlayerInfo;
    if (iGroupID > 0)
    {
        auto pUserInfo = CLogicCacheCommon::LoadUserData(iGroupID, iUin, true);
        if (pUserInfo)
        {
            stPlayerInfo.m_iPvpRank = pUserInfo->m_stUserInfoTable.GetPvpScore();
            stPlayerInfo.m_stInfo = CLogicCacheCommon::GetUserDetailInfo(iGroupID, iUin, CLogicConfigDefine::ETT_PvpDefense);

            auto stTeam = GetTeam(pUserInfo, CLogicConfigDefine::ETT_PvpDefense);
            if(!stTeam.empty()) stPlayerInfo.m_iCardID = stTeam[0];
            if(stPlayerInfo.m_iCardID <= 0) stPlayerInfo.m_iCardID = GetMainRoleCardID(pUserInfo);

            auto iterCard = pUserInfo->m_stHeroCardTableMap.Find(stPlayerInfo.m_iCardID);
            if(iterCard != pUserInfo->m_stHeroCardTableMap.End())
                stPlayerInfo.m_iEquipFashionID = iterCard->second->GetFashionID();
        }
    }
    else
    {
        auto pElem = CLogicConfig::GetPVPRobotConfig().GetConfig(iUin, iGroupID);

        if(pElem)
        {
            //int iRank = LOGIC_PVP_RANK_INS.GetRank(stKey.m_ulRoleID);
            //(iRank ? iRank : LOGIC_PVP_RANK_INS.max_rank()+1);
            auto iRank = pElem->m_iPvpScore;
            stPlayerInfo.m_iPvpRank = iRank;
            stPlayerInfo.m_iCardID = pElem->m_stCardInfoList[0].m_iCardID;
            stPlayerInfo.m_stInfo = CLogicCacheCommon::GetUserDetailInfo(iGroupID, iUin, CLogicConfigDefine::ETT_PvpDefense);
        }
    }

    return (stPlayerInfo);
}

CPackPvpChallengerCardInfo CLogicCacheCommon::GetPvpChallengerAttackCard(user_data_table_ptr_type pUserInfo, int iCardID, int iPosition/* = 0*/)
{
	CPackPvpChallengerCardInfo stCardInfo;
	auto stCardIter = pUserInfo->m_stHeroCardTableMap.Find(iCardID);
	if (stCardIter != pUserInfo->m_stHeroCardTableMap.End())
	{
		stCardInfo.m_iCardID = stCardIter->second->m_stData.m_iCardID;
		stCardInfo.m_iColor = stCardIter->second->GetCardColor();
		stCardInfo.m_iStar = stCardIter->second->GetCardStar();
		stCardInfo.m_iLevel = stCardIter->second->GetCardLevel();
        stCardInfo.m_stTalent = stCardIter->second->GetTalentMap();
        stCardInfo.m_iEquipFashionID = stCardIter->second->GetFashionID();
		stCardInfo.m_iPosition = iPosition;

        // 装备的星纹
        std::vector<int32_t> stUniqueIDs;
        stUniqueIDs.push_back(stCardIter->second->GetEquipConstellation1());
        stUniqueIDs.push_back(stCardIter->second->GetEquipConstellation2());
        stUniqueIDs.push_back(stCardIter->second->GetEquipConstellation3());
        stUniqueIDs.push_back(stCardIter->second->GetEquipConstellation4());
        for(auto id : stUniqueIDs)
        {
            if(id <= 0) continue;
            auto iterConstel = pUserInfo->m_stConstelMap.Find(id);
            if(iterConstel != pUserInfo->m_stConstelMap.End())
            {
                stCardInfo.m_stConstelVec.push_back(GetConstellationInfo(iterConstel->second->m_stData));
            }
        }

        // 装备
        std::vector<int32_t> stEquipIDs;
        stEquipIDs.push_back(stCardIter->second->GetEquip1());
        stEquipIDs.push_back(stCardIter->second->GetEquip2());
        stEquipIDs.push_back(stCardIter->second->GetEquip3());
        stEquipIDs.push_back(stCardIter->second->GetEquip4());
        for(auto id : stEquipIDs)
        {
            if(id <= 0) continue;
            auto iterEquip = pUserInfo->m_stEquipMap.Find(id);
            if(iterEquip != pUserInfo->m_stEquipMap.End())
            {
                stCardInfo.m_stEquipVec.push_back(GetEquipInfo(iterEquip->second->m_stData));
            }
        }
	}

	return (stCardInfo);
}

user_data_table_ptr_type
CLogicCacheCommon::GetUserDataFromDB(int iGroupID, int iUin)
{
    user_data_table_ptr_type pUserData = nullptr;

	static int64_t lStartMicroSecond = 0;
	static int64_t lEndMicroSecond = 0;
	static int64_t lCostMicroSecond = 0;

	lStartMicroSecond = taf::TC_Common::now2us();

	if (CLogicConfig::GetSystemConfig().isSelfGroup(iGroupID))
	{
        auto stUserInfoList = CRecordUserInfoTable::SelectDataFromSQL(iUin, iGroupID);
        if (!stUserInfoList.empty())
        {
            pUserData = new user_data_table_t();
            pUserData->m_iUin = iUin;
            pUserData->m_iGroupID = iGroupID;
            pUserData->m_strNick = stUserInfoList[0].m_strNick;
            pUserData->m_stUserInfoTable.InitializeWithoutSQL(stUserInfoList[0]);

            user_info_key_t stKey;
            stKey.m_stRoleIDDetail.m_iGroupID = stUserInfoList[0].m_iGroupID;
            stKey.m_stRoleIDDetail.m_iUin = stUserInfoList[0].m_iUid;

            LOGIC_USER_DATA_HASHMAP_INS->insert(std::make_pair(stKey.m_ulRoleID, pUserData));

            LOGIC_LOG_SLOW_ERROR << "INSERT INTO USER PTR TO CACHE|UIN:" << iUin << "|GROUP_ID:" << iGroupID << "|CACHE TOTAL USER:" << LOGIC_USER_DATA_HASHMAP_INS->size() << std::endl;

            auto stUserDailyInfo = CRecordUserDailyTable::SelectDataFromSQL(iUin, iGroupID);
            if (!stUserDailyInfo.empty())
            {
                pUserData->m_stUserDailyTable.InitializeWithoutSQL(stUserDailyInfo[0]);
            }

            auto stUserTeamInfo = CRecordUserTeamInfoTable::SelectDataFromSQL(iUin, iGroupID);
            if (!stUserTeamInfo.empty())
            {
                pUserData->m_stUserTeamInfoTable.InitializeWithoutSQL(stUserTeamInfo[0]);
            }

            auto stItemList = CRecordUserItemTable::SelectDataFromSQL(iUin, iGroupID);
            for (auto stIter = stItemList.begin(); stIter != stItemList.end(); ++stIter)
            {
                pUserData->m_stItemMap.InsertWithoutSQL(std::make_pair(stIter->m_iItemID, *stIter));
            }

            auto stTeamPresetList = CRecordUserTeamPresetInfoTable::SelectDataFromSQL(iUin, iGroupID);
            for(auto& team_preset : stTeamPresetList)
            {
                pUserData->m_stUserTeamPresetMap.InsertWithoutSQL(std::make_pair(team_preset.m_iTeamIndex, team_preset));
            }

            auto stHeroCardList = CRecordHeroCardInfoTable::SelectDataFromSQL(iUin, iGroupID);
            for (auto stIter = stHeroCardList.begin();
                stIter != stHeroCardList.end();
                ++stIter)
            {
                pUserData->m_stHeroCardTableMap.InsertWithoutSQL(
                    std::make_pair(stIter->m_iCardID, *stIter));
            }

            auto stLevelInfoList = CRecordLevelInfoTable::SelectDataFromSQL(iUin, iGroupID);
            for (auto stIter = stLevelInfoList.begin();
                stIter != stLevelInfoList.end();
                ++stIter)
            {
                pUserData->m_stLevelInfoTableMap.InsertWithoutSQL(
                    std::make_pair(stIter->m_iLevelID, *stIter));
            }

            auto stBuyCommodityList = CRecordUserBuyCommodityTable::SelectDataFromSQL(iUin, iGroupID);
            for (auto stIter = stBuyCommodityList.begin();
                stIter != stBuyCommodityList.end();
                ++stIter)
            {
                pUserData->m_stUserBuyCommodityMap.InsertWithoutSQL(
                    std::make_pair(stIter->m_iCommodityID, *stIter));
            }

            auto stMutiStepTaskList = CRecordMutiStepTaskTable::SelectDataFromSQL(iUin, iGroupID);
            for (auto stIter = stMutiStepTaskList.begin();
                stIter != stMutiStepTaskList.end();
                ++stIter)
            {
                pUserData->m_stMutiStepTaskTableMap.InsertWithoutSQL(
                    std::make_pair(stIter->m_iTaskID, *stIter));
            }

            auto stUserMailList = CRecordUserMailTable::SelectDataFromSQL(iUin, iGroupID);
            for (auto stIter = stUserMailList.begin(); stIter != stUserMailList.end(); ++stIter)
            {
                pUserData->m_stUserMailMap.InsertWithoutSQL(std::make_pair(stIter->m_iMailID, *stIter));
            }

            auto stActiveList = CRecordUserActiveTable::SelectDataFromSQL(iUin, iGroupID);
            for (auto stIter = stActiveList.begin(); stIter != stActiveList.end(); ++stIter)
            {
                pUserData->m_stUserActiveMap.InsertWithoutSQL(
                    std::make_pair(stIter->m_iActiveID, *stIter));
            }

            auto stFlowerLanternList = CRecordFlowerLanternTable::SelectDataFromSQL(iUin, iGroupID);
            for (const auto& lantern : stFlowerLanternList)
            {
                pUserData->m_stFlowerLanternMap.InsertWithoutSQL(std::make_pair(lantern.m_iLogIndex, lantern));
            }

            auto stUserDeposit = CRecordUserDepositTable::SelectDataFromSQL(iUin, iGroupID);
            for (auto stIter = stUserDeposit.begin();
                stIter != stUserDeposit.end();
                ++stIter)
            {
                pUserData->m_stUserDepositMap.InsertWithoutSQL(
                    std::make_pair(stIter->m_iOrderID, *stIter));
            }

            auto stAvatarList = CRecordUserAvatarTable::SelectDataFromSQL(iUin, iGroupID);
            for (auto& avatar : stAvatarList)
            {
                pUserData->m_stUserAvatarMap.InsertWithoutSQL(std::make_pair(avatar.m_iAvatarID, avatar));
            }

            auto stCrossFightList = CRecordCrossFightInfoTable::SelectDataFromSQL(iUin, iGroupID);
            for (auto stIter = stCrossFightList.begin(); stIter != stCrossFightList.end(); ++stIter)
            {
                pUserData->m_stCrossFightInfoMap.InsertWithoutSQL(std::make_pair(stIter->m_iTeamID, *stIter));
            }

            auto stUserStatList = CRecordUserStatTable::SelectDataFromSQL(iUin, iGroupID);
            for (auto stIter = stUserStatList.begin(); stIter != stUserStatList.end(); ++stIter)
            {
                pUserData->m_stUserStatTableMap.InsertWithoutSQL(std::make_pair(stIter->m_iStatDate, *stIter));
            }

            auto stShopList = CRecordUserShopTable::SelectDataFromSQL(iUin, iGroupID);
            for(auto& shop : stShopList)
            {
                pUserData->m_stUserShopMap.InsertWithoutSQL(std::make_pair(shop.m_iItemIndex, shop));
            }

            auto stBuffList = CRecordUserBuffTable::SelectDataFromSQL(iUin, iGroupID);
            for(auto& buff : stBuffList)
            {
                pUserData->m_stUserBuffMap.InsertWithoutSQL(std::make_pair(buff.m_iEffectIndex, buff));
            }

            auto stFirstRechargeList = CRecordFirstRechargeTable::SelectDataFromSQL(iUin, iGroupID);
            for (auto stIter = stFirstRechargeList.begin(); stIter != stFirstRechargeList.end(); ++stIter)
            {
                pUserData->m_stFirstRechargeMap.InsertWithoutSQL(std::make_pair(stIter->m_iUid, *stIter));
            }

            auto stActivityTaskList = CRecordActivityTaskTable::SelectDataFromSQL(iUin, iGroupID);
            for (auto stIter = stActivityTaskList.begin(); stIter != stActivityTaskList.end(); ++stIter)
            {
                auto taskKey = std::make_tuple(stIter->m_iActivityType, stIter->m_iActivityID, stIter->m_iTaskID);
                pUserData->m_stActivityTaskMap.InsertWithoutSQL(std::make_pair(taskKey, *stIter));
            }

            auto stMergeActivityList = CRecordMergeActivityTable::SelectDataFromSQL(iUin, iGroupID);
            for (auto stIter = stMergeActivityList.begin(); stIter != stMergeActivityList.end(); ++stIter)
            {
                pUserData->m_stMergeActivityMap.InsertWithoutSQL(std::make_pair(stIter->m_iTaskID, *stIter));
            }

            auto stAvyList = CRecordUserAvyTable::SelectDataFromSQL(iUin, iGroupID);
            for(auto& avy : stAvyList)
            {
                pUserData->m_stUserAvyMap.InsertWithoutSQL(std::make_pair(avy.m_iAvyID, avy));
            }

            auto stHomeGarden = CRecordUserHomeGardenTable::SelectDataFromSQL(iUin, iGroupID);
            for(auto& garden : stHomeGarden)
            {
                pUserData->m_stHomeGardenMap[garden.m_iHomeID].InsertWithoutSQL(std::make_pair(garden.m_iLandID, garden));
            }

            auto stHomeExplore = CRecordUserHomeExploreTable::SelectDataFromSQL(iUin, iGroupID);
            for(auto& explore : stHomeExplore)
            {
                pUserData->m_stHomeExploreMap.InsertWithoutSQL(std::make_pair(explore.m_iExploreQuestID, explore));
            }

            auto stBuyBackList = CRecordUserBuyBackTable::SelectDataFromSQL(iUin, iGroupID);
            for (auto stIter = stBuyBackList.begin(); stIter != stBuyBackList.end(); ++stIter)
            {
                pUserData->m_stBuyBackMap.InsertWithoutSQL(std::make_pair(stIter->m_iBackType, *stIter));
            }

            auto stFashionList = CRecordFashionInfoTable::SelectDataFromSQL(iUin, iGroupID);
            for (auto stIter = stFashionList.begin(); stIter != stFashionList.end(); ++stIter)
            {
                pUserData->m_stFashionInfoMap.InsertWithoutSQL(std::make_pair(stIter->m_iFashionID, *stIter));
            }

            auto stHonorList = CRecordUserHonorTable::SelectDataFromSQL(iUin, iGroupID);
            for (auto stIter = stHonorList.begin(); stIter != stHonorList.end(); ++stIter)
            {
                pUserData->m_stUserHonorMap.InsertWithoutSQL(std::make_pair(stIter->m_iHonorID, *stIter));
            }

            auto stFightReportList = CRecordFightReportTable::SelectDataFromSQL(iUin, iGroupID);
            for (auto stIter = stFightReportList.begin(); stIter != stFightReportList.end(); ++stIter)
            {
                pUserData->m_stFightReportMap[stIter->m_iReportType].InsertWithoutSQL(std::make_pair(stIter->m_iReportID, *stIter));
            }

            auto stQiyuList = CRecordUserQiyuTable::SelectDataFromSQL(iUin, iGroupID);
            for (auto stIter = stQiyuList.begin(); stIter != stQiyuList.end(); ++stIter)
            {
                pUserData->m_stQiyuMap[stIter->m_iMapID].InsertWithoutSQL(std::make_pair(stIter->m_iNodeID, *stIter));
            }

            auto stThemeActivityChargeList = CRecordLogicThemeActivityCharge::SelectDataFromSQL(iUin, iGroupID);
            for (auto stIter = stThemeActivityChargeList.begin(); stIter != stThemeActivityChargeList.end(); ++stIter)
            {
                pUserData->m_stThemeActivityChargeMap[stIter->m_iActivityID].InsertWithoutSQL(std::make_pair(stIter->m_iTaskID, *stIter));
            }

            auto stIBusyMarketList = CRecordUserIBusyMarketTable::SelectDataFromSQL(iUin, iGroupID);
            for (auto stIter = stIBusyMarketList.begin();
                stIter != stIBusyMarketList.end();
                ++stIter)
            {
                pUserData->m_stIBusyMarket.InsertWithoutSQL(
                    std::make_pair(stIter->m_iCommodityID, *stIter));
            }

            auto stUserIBusyInfo = CRecordUserIBusyInfoTable::SelectDataFromSQL(iUin, iGroupID);
            if (!stUserIBusyInfo.empty())
            {
                pUserData->m_stIBusyData.InitializeWithoutSQL(stUserIBusyInfo[0]);
            }

            auto stScoreLevelInfo = CRecordScoreLevelInfoTable::SelectDataFromSQL(iUin, iGroupID);
            for (const auto& info : stScoreLevelInfo)
            {
                pUserData->m_stScoreLevelMap.InsertWithoutSQL(std::make_pair(info.m_iActivityID, info));
            }

            auto stDaySingleRechargeTaskList = CRecordDaySingleRechargeTaskTable::SelectDataFromSQL(iUin, iGroupID);
            for (auto stIter = stDaySingleRechargeTaskList.begin(); stIter != stDaySingleRechargeTaskList.end(); ++stIter)
            {
                const auto taskKey = std::make_pair(stIter->m_iActivityID, stIter->m_iTaskID);
                pUserData->m_stDaySingleRechargeMap.InsertWithoutSQL(std::make_pair(taskKey, *stIter));
            }

            auto stProtoMonitorList = CRecordProtoMonitorTable::SelectDataFromSQL(iUin, iGroupID);
            for(const auto& info : stProtoMonitorList)
            {
                pUserData->m_stProtoMonitorMap.InsertWithoutSQL(std::make_pair(info.m_iCmd, info));
            }

            auto stItemMonitorList = CRecordItemMonitorTable::SelectDataFromSQL(iUin, iGroupID);
            for(const auto& info : stItemMonitorList)
            {
                const auto key = std::make_pair(info.m_iItemType, info.m_iItemID);
                pUserData->m_stItemMonitorMap.InsertWithoutSQL(std::make_pair(key, info));
            }

            auto stMonthPassTaskList = CRecordMonthPassTaskTable::SelectDataFromSQL(iUin, iGroupID);
            for (const auto& task : stMonthPassTaskList)
            {
                pUserData->m_stMonthPassTaskMap[task.m_iTaskType].InsertWithoutSQL(std::make_pair(task.m_iTaskID, task));
            }

            auto stWenWenNewsList = CRecordUserWenWenNewsTable::SelectDataFromSQL(iUin, iGroupID);
            for(const auto& news : stWenWenNewsList)
            {
                pUserData->m_stWenWenNewsMap[news.m_iNewsType].InsertWithoutSQL(std::make_pair(news.m_iNewsID, news));
            }

            auto stHeroHistoryList = CRecordUserHeroHistoryTable::SelectDataFromSQL(iUin, iGroupID);
            for(const auto& history : stHeroHistoryList)
            {
                pUserData->m_stHeroHistoryMap[history.m_iCardID].InsertWithoutSQL(std::make_pair(history.m_iHistoryIndex, history));
            }

            auto stRandomStoreList = CRecordUserRandomStoreTable::SelectDataFromSQL(iUin, iGroupID);
            for(const auto& store : stRandomStoreList)
            {
                pUserData->m_stRandomStoreMap[store.m_iStoreID].InsertWithoutSQL(std::make_pair(store.m_iCommodityID, store));
            }

            auto stConstellationList = CRecordUserConstellationTable::SelectDataFromSQL(iUin, iGroupID);
            for(const auto& constellation : stConstellationList)
            {
                pUserData->m_stConstelMap.InsertWithoutSQL(std::make_pair(constellation.m_iUniqueID, constellation));
            }

            auto stConstelPresetList = CRecordUserConstelPresetTable::SelectDataFromSQL(iUin, iGroupID);
            for(const auto& constel : stConstelPresetList)
            {
                pUserData->m_stConstelPresetMap.InsertWithoutSQL(std::make_pair(constel.m_iUniqueID, constel));
            }

            auto stEquipList = CRecordUserEquipTable::SelectDataFromSQL(iUin, iGroupID);
            for(const auto& equip : stEquipList)
            {
                pUserData->m_stEquipMap.InsertWithoutSQL(std::make_pair(equip.m_iUniqueID, equip));
            }

            auto stHomeList = CRecordUserHomeTable::SelectDataFromSQL(iUin, iGroupID);
            for(const auto& home : stHomeList)
            {
                pUserData->m_stHomeMap.InsertWithoutSQL(std::make_pair(home.m_iHomeID, home));
            }

            auto stHomeGoodsList = CRecordUserHomeGoodsTable::SelectDataFromSQL(iUin, iGroupID);
            for(const auto& goods : stHomeGoodsList)
            {
                pUserData->m_stHomeGoodsMap[goods.m_iHomeID].InsertWithoutSQL(std::make_pair(goods.m_iGoodsIndex, goods));
            }

            auto stHomeFootclothList = CRecordUserHomeFootclothTable::SelectDataFromSQL(iUin, iGroupID);
            for(const auto& foot : stHomeFootclothList)
            {
                pUserData->m_stHomeFootclothMap[foot.m_iHomeID].InsertWithoutSQL(std::make_pair(foot.m_iFootclothID, foot));
            }

            auto stFishList = CRecordUserFishItemTable::SelectDataFromSQL(iUin, iGroupID);
            for(const auto& fish : stFishList)
            {
                pUserData->m_stFishItemMap.InsertWithoutSQL(std::make_pair(fish.m_iFishIndex, fish));
            }

            auto stQuestList = CRecordUserQuestTable::SelectDataFromSQL(iUin, iGroupID);
            for(const auto& quest : stQuestList)
            {
                pUserData->m_stQuestMap.InsertWithoutSQL(std::make_pair(quest.m_iQuestID, quest));
            }

            auto stFriendList = CRecordUserFriendTable::SelectDataFromSQL(iUin, iGroupID);
            for(auto& info : stFriendList)
            {
                pUserData->m_stFriendMap.InsertWithoutSQL(std::make_pair(info.m_iFriendUid, info));
            }

            auto stFriendApplyList = CRecordUserFriendApplyTable::SelectDataFromSQL(iUin, iGroupID);
            for(auto& info : stFriendApplyList)
            {
                pUserData->m_stFriendApplyMap.InsertWithoutSQL(std::make_pair(info.m_iFriendUid, info));
            }

            auto stBadgeList = CRecordUserBadgeTable::SelectDataFromSQL(iUin, iGroupID);
            for(auto& info : stBadgeList)
            {
                pUserData->m_stUserBadgeMap.InsertWithoutSQL(std::make_pair(info.m_iBadgeID, info));
            }

            auto stPayDiamondList = CRecordUserPayDiamondTable::SelectDataFromSQL(iUin,iGroupID);
            if (!stPayDiamondList.empty())
            {
                pUserData->m_stUserPayDiamondTable.InitializeWithoutSQL(stPayDiamondList[0]);
            }

            auto stHomeTopicList = CRecordUserHomeTopicTable::SelectDataFromSQL(iUin, iGroupID);
            for(auto& info : stHomeTopicList)
            {
                pUserData->m_stUserHomeTopicMap.InsertWithoutSQL(std::make_pair(info.m_iHomeTopicID, info));
            }

        }

        if (nullptr != pUserData)
        {
            pUserData->m_stUserInfoTable.SetVipLevel(CLogicConfig::GetVipConfig().GetVipLevel(pUserData->m_stUserInfoTable.GetVipExp()));
        }
	}

	lEndMicroSecond = taf::TC_Common::now2us();
	lCostMicroSecond = lEndMicroSecond - lStartMicroSecond;
	LOGIC_LOG_SLOW_ERROR << "GET USER DATA FROM DB|UIN:" << iUin << "|GROUP_ID:" << iGroupID << "|CACHE TOTAL USER:" << LOGIC_USER_DATA_HASHMAP_INS->size()
		<< "|CONSUME_MICRO_TIME:" << lCostMicroSecond << std::endl;
    
    return (pUserData);
}

void
CLogicCacheCommon::AutoAddVitality(user_data_table_ptr_type pUserData)
{
    int iTimestamp = CLogicCommon::GetSec();

    int iMaxVitality = CLogicConfig::GetGlobalConfig().m_iMaxVitality;
    int iCurVitality = pUserData->m_stUserInfoTable.GetVitality();

    int iGrowInterval = CLogicConfig::GetGlobalConfig().m_iNumSecondByAutoIncreaseOneVitality;

    int iAddAutoTime = iTimestamp - pUserData->m_stUserInfoTable.GetAddVitalityTime();
    int iAutoAddVitality = iAddAutoTime / iGrowInterval;
    int iAddLastAutoAddTime = iAutoAddVitality * iGrowInterval;

    if (iCurVitality >= iMaxVitality)
    {
        pUserData->m_stUserInfoTable.SetAddVitalityTime(iTimestamp);
    }
    else if ((iCurVitality + iAutoAddVitality) >= iMaxVitality)
    {
        CLogicCacheCommon::AddGameItem(pUserData, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_VitalityID, iMaxVitality - iCurVitality, MSG_LOGIC_NUM_AUTO_INCREASE);

        pUserData->m_stUserInfoTable.SetAddVitalityTime(iTimestamp);
    }
    else
    {
        if (iAutoAddVitality > 0)
        {
            CLogicCacheCommon::AddGameItem(pUserData, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_VitalityID, iAutoAddVitality, MSG_LOGIC_NUM_AUTO_INCREASE);
        }

        if (iAddLastAutoAddTime > 0)
        {
            pUserData->m_stUserInfoTable.AddAddVitalityTime(iAddLastAutoAddTime);
        }
    }
}

void CLogicCacheCommon::AutoAddPhyPower(user_data_table_ptr_type pUserData)
{
    int iNowTimestamp = CLogicCommon::GetSec();
    int iNumSecondPerEnergyGrow = CLogicConfig::GetGlobalConfig().m_iNumSecondByAutoIncreaseOneEnergy;
    int iAddAutoTime = iNowTimestamp - pUserData->m_stUserInfoTable.GetLastAutoAddTime();
    int iAutoAddEnergyNum = iAddAutoTime / iNumSecondPerEnergyGrow;

    if(iAutoAddEnergyNum <= 0)
        return;

    int iNowPhyPower = pUserData->m_stUserInfoTable.GetPhyPower();
    int iMaxEnengyNum = CLogicConfig::GetGlobalConfig().m_iMaxEnergy;
    int iNowPhyStore = pUserData->m_stUserInfoTable.GetPhyStore();
    int iMaxPhyStore = CLogicConfig::GetGlobalConfig().m_iEnergyStoreMax * CLogicConfigDefine::ESD_NormalPctWeight;

    // 所有体力存储都已满
    if(iNowPhyPower >= iMaxEnengyNum && iNowPhyStore >= iMaxPhyStore)
    {
        pUserData->m_stUserInfoTable.SetLastAutoAddTime(iNowTimestamp);
        return;
    }

    int iAddLastAutoAddTime = iAutoAddEnergyNum * iNumSecondPerEnergyGrow;

    // 体力未满
    if(iNowPhyPower < iMaxEnengyNum)
    {
        int iAddValue = std::min(iAutoAddEnergyNum, iMaxEnengyNum - iNowPhyPower);
        iAutoAddEnergyNum -= iAddValue;

        CLogicCacheCommon::AddGameItem(pUserData, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_EnergyID, iAddValue, MSG_LOGIC_NUM_AUTO_INCREASE);
    }

    // 食盒未满
    if(iAutoAddEnergyNum > 0 && iNowPhyStore < iMaxPhyStore)
    {
        int iStorePct = CLogicConfig::GetGlobalConfig().m_iEnergyStorePct;
        // 月卡加成
        if(CLogicBuff::IsHaveMonthCard(pUserData))
        {
            iStorePct += CLogicConfig::GetAwardConfig().m_stMonthCardConfig.m_iPhyStorePctInc;
        }

        int iAddValue = std::min(iAutoAddEnergyNum * iStorePct, iMaxPhyStore - iNowPhyStore);
        pUserData->m_stUserInfoTable.AddPhyStore(iAddValue);

        // 通知
        CLogicUserNotifySystem::NotifyUserPhyData(pUserData);
    }

    // 所有体力已满
    if(pUserData->m_stUserInfoTable.GetPhyPower() >= iMaxEnengyNum && pUserData->m_stUserInfoTable.GetPhyStore() >= iMaxPhyStore)
    {
        pUserData->m_stUserInfoTable.SetLastAutoAddTime(iNowTimestamp);
    }
    else
    {
        pUserData->m_stUserInfoTable.AddLastAutoAddTime(iAddLastAutoAddTime);
    }

    LOGIC_LOG_TRACE_DEBUG << "AUTO_ADD_ENERGY SUCCESS|GROUP_ID:" << pUserData->m_stUserInfoTable.m_stData.m_iGroupID
                          << "|UID:" << pUserData->m_stUserInfoTable.m_stData.m_iUid
                          << "|ADD_ENERGY_NUM:" << iAutoAddEnergyNum
                          << "|ADD_AUTO_ADD_TIME:" << iAddLastAutoAddTime << std::endl;
}

bool
CLogicCacheCommon::AddGameItem(user_data_table_ptr_type pUserData, const std::vector<CPackGameItem>& stBonusItemList
		, std::string& strBonusList, int32_t iCmd, const std::map<std::string, std::string>& info, bool isMonitor/* = true*/)
{
    static std::string strBonus;
    if (stBonusItemList.empty()) {
        strBonusList = "{}";
        return true;
    }

	std::vector<CPackGameItem> stCombineItemVec;
	for (const auto& stBonusItem : stBonusItemList)
	{
		PushGameItem(stCombineItemVec, stBonusItem);
	}

    strBonusList = "{";
    for (const auto& stBonusItem : stCombineItemVec)
    {
        if (false == AddGameItem(pUserData, stBonusItem, strBonus, iCmd, info, isMonitor))
            return false;
        strBonusList += strBonus;
    }
    strBonusList[strBonusList.length() - 1] = '}';
    return true;
}

bool
CLogicCacheCommon::AddGameItem(user_data_table_ptr_type pUserData, const std::vector<CPackGameItem>& stBonusItemList, int32_t iCmd, const std::map<std::string, std::string>& info, bool isMonitor/* = true*/)
{
	std::vector<CPackGameItem> stCombineItemVec;
	for (const auto& stBonusItem : stBonusItemList)
	{
		PushGameItem(stCombineItemVec, stBonusItem);
	}

    for (const auto& stBonusItem : stCombineItemVec)
    {
        if (!AddGameItem(pUserData, stBonusItem, iCmd, info, isMonitor))
            return false;
    }
    return true;
}

bool
CLogicCacheCommon::AddGameItem(user_data_table_ptr_type pUserData, const CPackGameItem& stBonusItem, std::string& strBonus, int32_t iCmd, const std::map<std::string, std::string>& info, bool isMonitor/* = true*/)
{
    if (false == AddGameItem(pUserData, stBonusItem, iCmd, info, isMonitor))
        return false;

    static char szBuffer[128];
    snprintf(szBuffer, sizeof(szBuffer), "[%d, %d, %d],",
            stBonusItem.m_iItemType,
            stBonusItem.m_iCardID,
            stBonusItem.m_iNum);

    strBonus = szBuffer;
    return true;
}

bool
CLogicCacheCommon::AddGameItem(user_data_table_ptr_type pUserData, int iItemType, int iCardID, int iNum, int32_t iCmd, const std::map<std::string, std::string>& info, bool isMonitor/* = true*/)
{
    if (iNum == 0) return true;

    static CPackGameItem stBonusItem;
    stBonusItem.m_iItemType = iItemType;
    stBonusItem.m_iCardID = iCardID;
    stBonusItem.m_iNum = iNum;

    return AddGameItem(pUserData, stBonusItem, iCmd, info, isMonitor);
}

bool
CLogicCacheCommon::AddGameItem(user_data_table_ptr_type pUserData, const CPackGameItem& stBonusItem, int32_t iCmd, const std::map<std::string, std::string>& info, bool isMonitor/* = true*/)
{
    if (stBonusItem.m_iNum <= 0) return true;

    auto stGameItem = LOGIC_GAME_ITEM_FACTORY_INS->CreateGameItem(
            (CLogicConfigDefine::GAME_ITEM_COLLECTION) stBonusItem.m_iItemType,
                                                         stBonusItem.m_iCardID,
                                                         stBonusItem.m_iNum,
                                                         iCmd,
                                                         pUserData,
                                                         info);

    if (stGameItem.get())
    {
        stGameItem->Add();
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(UNKNOW_GAME_ITEM_NOT_IN_CONFIG);
        char szBuffer[64];
        snprintf(szBuffer, sizeof(szBuffer), "|UIN:%d|GROUP_ID:%d|CMD:%d|UNKNOW_GAME_ITEM:[%d, %d, %d]",
                pUserData->m_iUin, pUserData->m_iGroupID, iCmd,
                stBonusItem.m_iItemType,
                stBonusItem.m_iCardID,
                stBonusItem.m_iNum);

        CLogicError::AppendErrMsg(szBuffer);
        return false;
    }

    // 卡片和角色等级由于特殊性, 必须单独在 GameItem 中处理
    if (stBonusItem.m_iItemType != CLogicConfigDefine::HERO_CARD)
    {
        CLogicGameItemAddChannel::data_type stData;
        stData.m_iItemType = stBonusItem.m_iItemType;
        stData.m_iCardID = stBonusItem.m_iCardID;
        stData.m_iNum = stBonusItem.m_iNum;

        LOGIC_PUBLISHER_INS->PublishGameItemAddInfo(pUserData, stData);
    }

    //发送变动通知
    if (iCmd != MSG_LOGIC_HOME_SAVE_HOME)
    {
        CLogicEventData stEventData;
        stEventData.m_eType = CLogicConfigDefine::ECET_GetHomeLevelItem;
        stEventData.m_iCmd = iCmd;
        stEventData.m_iPara = stBonusItem.m_iCardID;
        stEventData.m_iCount = stBonusItem.m_iNum;
        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(pUserData, stEventData);

        //如果是指定的家园道具需要添加buff
        CLogicEventData stEventBuffData;
        stEventBuffData.m_eType = CLogicConfigDefine::ECET_GetHomeBuffItem;
        stEventBuffData.m_iCmd = iCmd;
        stEventBuffData.m_iPara = stBonusItem.m_iCardID;
        stEventBuffData.m_iCount = stBonusItem.m_iNum;
        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(pUserData, stEventBuffData);
    }




    // 物品监控
    if((iCmd > MSG_ADMIN_END || iCmd < MSG_ADMIN_START) && isMonitor)
    	CLogicMonitor::ItemMonitor(pUserData, stBonusItem.m_iItemType, stBonusItem.m_iCardID, stBonusItem.m_iNum, iCmd);



    return true;
}

bool
CLogicCacheCommon::ConsumeGameItem(user_data_table_ptr_type pUserData, const CPackGameItem& stConsumeItem, int32_t iCmd, const std::map<std::string, std::string>& info)
{
    return ConsumeGameItem(pUserData, (CLogicConfigDefine::GAME_ITEM_COLLECTION)
            stConsumeItem.m_iItemType,
            stConsumeItem.m_iCardID,
            stConsumeItem.m_iNum,
			iCmd,
			info);
}

bool
CLogicCacheCommon::ConsumeGameItem(user_data_table_ptr_type pUserData, const std::vector<CPackGameItem>& stConsumeItemVec, int32_t iCmd, const std::map<std::string, std::string>& info)
{
	std::vector<CPackGameItem> stCombineItemVec;
	for (const auto& stBonusItem : stConsumeItemVec)
	{
        if(stBonusItem.m_iNum < 0)
        {
            SET_ERR_INFO(SERVER_ERRCODE::INTERN_ERROR);
            return false;
        }

        if(0 == stBonusItem.m_iNum) continue;

		PushGameItem(stCombineItemVec, stBonusItem);
	}

    std::vector<CLogicGameItemFactory::CRkLogicGameItemBasePtr> stConsumeVec;
    for (const auto& stConsumeItem : stCombineItemVec)
    {
        auto stConsume = LOGIC_GAME_ITEM_FACTORY_INS->CreateGameItem(
                (CLogicConfigDefine::GAME_ITEM_COLLECTION) stConsumeItem.m_iItemType,
                                                             stConsumeItem.m_iCardID,
                                                             stConsumeItem.m_iNum,
															 iCmd,
                                                             pUserData,
															info);

        if (!stConsume.get() || !stConsume->CanConsume()) return false;

        stConsumeVec.push_back(stConsume); 
    }

    for (const auto& stConsume : stConsumeVec)
    {
        stConsume->Consume();
    }

    return true;
}

bool 
CLogicCacheCommon::CanConsume(user_data_table_ptr_type pUserData, int iItemType, int iCardID, int iNum, int32_t iCmd)
{
    if(iNum < 0)
    {
        SET_ERR_INFO(SERVER_ERRCODE::INTERN_ERROR);
        return false;
    }

    if(0 == iNum) return true;

	auto stConsume = LOGIC_GAME_ITEM_FACTORY_INS->CreateGameItem(
		(CLogicConfigDefine::GAME_ITEM_COLLECTION) iItemType, iCardID, iNum, iCmd, pUserData);

	if (!stConsume.get() || !stConsume->CanConsume()) return (false);

	return true;
}

bool
CLogicCacheCommon::ConsumeGameItem(user_data_table_ptr_type pUserData, int iItemType, int iCardID, int iNum, int32_t iCmd, const std::map<std::string, std::string>& info)
{
    if(iNum < 0)
    {
        SET_ERR_INFO(SERVER_ERRCODE::INTERN_ERROR);
        return false;
    }

    if(0 == iNum) return true;

    auto stConsume = LOGIC_GAME_ITEM_FACTORY_INS->CreateGameItem(
            (CLogicConfigDefine::GAME_ITEM_COLLECTION) iItemType, iCardID, iNum, iCmd, pUserData, info);

    if(!stConsume.get() || !stConsume->CanConsume()) return (false);

    stConsume->Consume();

    return true;
}

bool
CLogicCacheCommon::SellGameItem(user_data_table_ptr_type pUserData, int iItemType, int iCardID, int iNum, int32_t iCmd)
{
    if(iNum < 0)
    {
        SET_ERR_INFO(SERVER_ERRCODE::INTERN_ERROR);
        return false;
    }

    if(0 == iNum) return true;

    auto stSell = LOGIC_GAME_ITEM_FACTORY_INS->CreateGameItem(
        (CLogicConfigDefine::GAME_ITEM_COLLECTION)iItemType, iCardID, iNum, iCmd, pUserData);

    if (!stSell.get() || stSell->Sell() == -1) return false;

    return true;
}

bool CLogicCacheCommon::ChangeRoleNick(user_data_table_ptr_type pUserData, const std::string& strNick, CLogicSQLTableRecordBase::sql_operation_t iOperation)
{
	try
	{
		const std::string strEscapeNick = CLogicSQLTableRecordBase::GetMysqlConn()->escapeString(strNick);

		CRecordServerNickTable stServerNickTableRecord;
		stServerNickTableRecord.SetOperation(iOperation);
		stServerNickTableRecord.SetPrimaryKey(pUserData->m_iUin, pUserData->m_iGroupID);
		stServerNickTableRecord.SetNick(strEscapeNick);

		CLogicSQLTableRecordBase::GetMysqlConn()->execute(stServerNickTableRecord.MakeSQL());
	}
	catch (const taf::TC_Mysql_Exception& stEx)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(USER_ROLE_NICK_EXIST);
		return false;
	}

	pUserData->m_strNick = strNick;
	pUserData->m_stUserInfoTable.SetNick(strNick);

	return true;
}

void 
CLogicCacheCommon::PushGameItem(std::vector<CPackGameItem>& stBonusItemList, const CPackGameItem& stBonusItem)
{
	 CLogicConfigUtil::CombineGameItem(stBonusItemList, stBonusItem);
}

int
CLogicCacheCommon::GetMainRoleCardID(user_data_table_ptr_type pUserData)
{
    // Female
    return CLogicConfigDefine::FEMALE_LEADING_HERO_ID;
}

bool CLogicCacheCommon::IsMainRoleCardID(int32_t iCardID)
{
	return iCardID == CLogicConfigDefine::MALE_LEADING_HERO_ID || iCardID == CLogicConfigDefine::FEMALE_LEADING_HERO_ID;
}

int32_t CLogicCacheCommon::ResultMatchPVPScore(user_data_table_ptr_type pUserData, bool is_win)
{
	int32_t iDeltaScore = 0;
	const auto& config = CLogicConfig::GetMatchPvpConfig();
	if (pUserData->m_stOnlyInCacheData.m_stMatchPvpPlayer.m_stBriefInfo.m_iUid > 0)
	{
		const int32_t level = config.GetMatchLevelByScore(pUserData->m_stUserInfoTable.GetMatchScore());
		const auto* pstLevel = config.GetMatchLevelConfig(level);
		if (nullptr != pstLevel)
		{
			iDeltaScore = is_win ? abs(pstLevel->m_iWinScore) : -abs(pstLevel->m_iLoseScore);
			is_win ? pUserData->m_stUserInfoTable.AddMatchWinCount(1) : pUserData->m_stUserInfoTable.AddMatchLoseCount(1);
			pUserData->m_stUserInfoTable.AddMatchScore(iDeltaScore);	
			if (pUserData->m_stUserInfoTable.GetMatchScore() > pUserData->m_stUserInfoTable.GetMatchMaxScore())
			{
				pUserData->m_stUserInfoTable.SetMatchMaxScore(pUserData->m_stUserInfoTable.GetMatchScore());

				CLogicDailyTaskSystem::SetProgress(pUserData, CLogicDailyTaskSystem::USER_GAME_STATUS_TASK, CLogicDailyTaskSystem::TASK_STATUS_MATCH_PVP_LEVEL, CLogicConfig::GetMatchPvpConfig().GetMatchLevelByScore(pUserData->m_stUserInfoTable.GetMatchMaxScore()));
			}
			CLogicUserNotifySystem::NotifyMatchPvpInfo(pUserData);

			CLogicGlobal::UpdateOrInsertMatchRank(pUserData->m_iUin, pUserData->m_iGroupID, pUserData->m_stUserInfoTable.GetMatchScore());

		}

		if (is_win)
        {
            logic_game_user_action_channel_data_t stData;
            stData.m_iTimes = 1;
            stData.m_iAction = CLogicGameChannelEnum::ACTION_MATCH_PVP_WIN_COUNT;
            LOGIC_PUBLISHER_INS->PublishGameUserActionInfo(pUserData, stData);
        }

		pUserData->m_stOnlyInCacheData.m_stMatchPvpHadUins.push_back(pUserData->m_stOnlyInCacheData.m_stMatchPvpPlayer.m_stBriefInfo.m_iUid);
		if (pUserData->m_stOnlyInCacheData.m_stMatchPvpHadUins.size() > 10)
		{
			pUserData->m_stOnlyInCacheData.m_stMatchPvpHadUins.pop_front();
		}

		if (is_win || pUserData->m_stOnlyInCacheData.m_iMatchBaodiFailCount >= CLogicConfig::GetMatchPvpConfig().m_iBaodiFailCount)
		{
			pUserData->m_stOnlyInCacheData.m_iMatchBaodiFailCount = 0;
		}
		else if (level >= CLogicConfig::GetMatchPvpConfig().m_iBaodiMatchLevel)
		{
			++pUserData->m_stOnlyInCacheData.m_iMatchBaodiFailCount;
		}

		//添加战报
		{
			int triggerValue = is_win ? 2 : 0;
			for (auto iter = CLogicConfig::GetFightReportConfig().m_stConfigMap.lower_bound("dianfengjingji"); iter != CLogicConfig::GetFightReportConfig().m_stConfigMap.upper_bound("dianfengjingji"); ++iter)
			{
				if (iter->second.m_iTriggerValue == triggerValue)
				{
					CPackFightReportItem reportItem;
					reportItem.m_iReportType = CLogicConfigDefine::FIGHT_REPORT_DIANFENGJINGJI;
					reportItem.m_iReportID = CLogicCommon::GetSec();
					std::map<std::string, std::string> stParamMap;
					stParamMap["nick"] = pUserData->m_stOnlyInCacheData.m_stMatchPvpPlayer.m_stBriefInfo.m_strNick;
					stParamMap["score"] = std::to_string(iDeltaScore);

					reportItem.m_strReportContent = iter->second.m_stContentObj.Replace(stParamMap);
					reportItem.m_cFightResult = triggerValue;
					reportItem.m_stParams.push_back(iDeltaScore);
					CLogicCacheCommon::AddUserFightReport(pUserData, reportItem);
					break;
				}
			}			
		}

		pUserData->m_stOnlyInCacheData.m_stMatchPvpPlayer = {};
	}

	return iDeltaScore;
}

int32_t CLogicCacheCommon::ResultFairPVPScore(user_data_table_ptr_type pUserData, bool is_win)
{
	if (pUserData->m_stOnlyInCacheData.m_stFairPvpPlayer.m_stBriefInfo.m_iUid > 0)
	{
		is_win ? pUserData->m_stUserInfoTable.AddFairPvpWinCount(1) : pUserData->m_stUserInfoTable.AddFairPvpLoseCount(1);
		//添加战报
		{
			int triggerValue = is_win ? 2 : 0;
			for (auto iter = CLogicConfig::GetFightReportConfig().m_stConfigMap.lower_bound("fairpvp"); iter != CLogicConfig::GetFightReportConfig().m_stConfigMap.upper_bound("fairpvp"); ++iter)
			{
				if (iter->second.m_iTriggerValue == triggerValue)
				{
					CPackFightReportItem reportItem;
					reportItem.m_iReportType = CLogicConfigDefine::FIGHT_REPORT_FAIR_PVP;
					reportItem.m_iReportID = CLogicCommon::GetSec();
					std::map<std::string, std::string> stParamMap;
					stParamMap["nick"] = pUserData->m_stOnlyInCacheData.m_stFairPvpPlayer.m_stBriefInfo.m_strNick;
					stParamMap["winCount"] = std::to_string(pUserData->m_stUserInfoTable.GetFairPvpWinCount());
					stParamMap["loseCount"] = std::to_string(pUserData->m_stUserInfoTable.GetFairPvpLoseCount());

					reportItem.m_strReportContent = iter->second.m_stContentObj.Replace(stParamMap);
					reportItem.m_cFightResult = triggerValue;
					CLogicCacheCommon::AddUserFightReport(pUserData, reportItem);
					break;
				}
			}
		}

		//公告
		if (is_win)
		{
			const auto* pstWinConfig = CLogicConfig::GetFairPvpConfig().GetFairPvpWinConfig(pUserData->m_stUserInfoTable.GetFairPvpWinCount(), CLogicCommon::GetSec());
			if (nullptr != pstWinConfig && !pstWinConfig->m_strBroadCastID.empty())
			{
				CLogicGlobal::PublishBroadcast(pstWinConfig->m_strBroadCastID, 0, {
					{"nick", pUserData->m_strNick},
					{"num", std::to_string(pUserData->m_stUserInfoTable.GetFairPvpWinCount())}
				});
			}

			if (pUserData->m_stUserInfoTable.GetFairPvpWinCount() == CLogicConfig::GetFairPvpConfig().m_iTotalWinCount)
			{
				logic_game_user_action_channel_data_t stActionData;
				stActionData.m_iTimes = 1;
				stActionData.m_iAction = CLogicGameChannelEnum::ACTION_FAIR_PVP_12_WIN;
				LOGIC_PUBLISHER_INS->PublishGameUserActionInfo(pUserData, stActionData);
			}
		}

		LOGIC_STATISTICS_INS->UpdatePlayerFairResult(pUserData->m_iUin, pUserData->m_iGroupID, int32_t(is_win), pUserData->m_stUserInfoTable.GetFairPvpTeamCard());
		for (const auto& card_id : pUserData->m_stUserInfoTable.GetFairPvpTeamCard())
		{
			LOGIC_STATISTICS_INS->AddEventStat("fair_pvp_result", std::to_string(card_id), 1, int32_t(is_win));
		}		

		pUserData->m_stOnlyInCacheData.m_stFairPvpPlayer = {};

		CLogicUserNotifySystem::NotifyFairPvpInfo(pUserData);
	}

	return pUserData->m_stUserInfoTable.GetFairPvpWinCount();
}

int 
CLogicCacheCommon::GetLeaderCardID(user_data_table_ptr_type pUserData)
{
    return (CLogicConfigDefine::FEMALE_LEADING_HERO_ID);//女主角
}

std::string
CLogicCacheCommon::GetGameItemString(const std::vector<CPackGameItem>& stGameItems)
{
    static char szBuffer[256];

    if (stGameItems.empty()) {
        return std::string("{}");
    }

    std::string strBonusList = "{";
    for (const auto& stItem : stGameItems)
    {
        snprintf(szBuffer, sizeof(szBuffer), "[%d, %d, %d],",
                stItem.m_iItemType,
                stItem.m_iCardID,
                stItem.m_iNum);

        strBonusList += szBuffer; 
    }
    strBonusList[strBonusList.length() - 1] = '}';

    return strBonusList;
}

void CLogicCacheCommon::AddCharm(user_data_table_ptr_type pUserInfo, int32_t iAddCharm)
{
	if (iAddCharm > 0)
	{
		pUserInfo->m_stUserInfoTable.AddCharm(iAddCharm);
		CLogicUserNotifySystem::NotifyUserDailyData(pUserInfo);

		CLogicGlobal::UpdateOrInsertCharmRank(pUserInfo->m_iUin, pUserInfo->m_iGroupID, pUserInfo->m_stUserInfoTable.GetCharm());

		CLogicDailyTaskSystem::SetProgress(pUserInfo, CLogicDailyTaskSystem::USER_GAME_STATUS_TASK, CLogicDailyTaskSystem::TASK_STATUS_CHARM, pUserInfo->m_stUserInfoTable.GetCharm());

	}
}

bool CLogicCacheCommon::AddUserFightReport(user_data_table_ptr_type pUser, CPackFightReportItem &stReportItem)
{
    //1.添加战报
    auto &typeReport = pUser->m_stFightReportMap[stReportItem.m_iReportType];

    TFightReportTableValueType newReport;
    newReport.m_iUid = pUser->m_iUin;
    newReport.m_iGroupID = pUser->m_iGroupID;
    newReport.m_iReportType = stReportItem.m_iReportType;
    newReport.m_iReportID = stReportItem.m_iReportID;
    newReport.m_strReportContent = stReportItem.m_strReportContent;
    newReport.m_cFightResult = stReportItem.m_cFightResult;
    newReport.m_stParams = stReportItem.m_stParams;
    if (typeReport.Find(newReport.m_iReportID) != typeReport.End())
    {
        if (typeReport.Find(newReport.m_iReportID + 1) == typeReport.End())
        {
            ++newReport.m_iReportID;
        }
        else
        {
            return false;
        }
    }

    typeReport.Insert(make_pair(newReport.m_iReportID, newReport));

    //2.如果战报数量超过限制，则删除最早的战报
    size_t maxCount = CLogicConfig::GetFightReportConfig().GetMaxFightReportCount(stReportItem.m_iReportType);
    while (typeReport.Size() > maxCount)
    {
        typeReport.Erase(typeReport.Begin());
    }

    return true;
}

bool CLogicCacheCommon::ClearUserFightReport(user_data_table_ptr_type pUser, int32_t iFightReportType)
{
    auto findIter = pUser->m_stFightReportMap.find(iFightReportType);
    if (findIter != pUser->m_stFightReportMap.end())
    {
        auto &typeReport = findIter->second;

        while (typeReport.Size() > 0)
        {
            typeReport.Erase(typeReport.Begin());
        }
    }

    return true;
}

// iIndex:从0开始
bool CLogicCacheCommon::VecBitmapCheckState(const CVecBitmap& stData, int32_t iIndex)
{
    if(iIndex < 0) return true;
    if(stData.size() <= 0) return false;

    const std::size_t uiSize = sizeof(VecBitmapSubtype) * 8;
    std::size_t iXIndex = iIndex / uiSize;
    std::size_t iYIndex = iIndex % uiSize;

    if(stData.size() <= iXIndex) return false;

    if(stData[iXIndex] & ((VecBitmapSubtype)0x1 << iYIndex))
        return true;

    return false;
}

// iIndex:从0开始
void CLogicCacheCommon::VecBitmapSetState(CVecBitmap& stData, int32_t iIndex)
{
    if(iIndex < 0) return;

    const std::size_t uiSize = sizeof(VecBitmapSubtype) * 8;
    std::size_t iXIndex = iIndex / uiSize;
    std::size_t iYIndex = iIndex % uiSize;

    for(std::size_t i = stData.size(); i <= iXIndex; ++i)
    {
        stData.push_back((VecBitmapSubtype)0x0);
    }

    stData[iXIndex] = stData[iXIndex] | ((VecBitmapSubtype)0x1 << iYIndex);
}

std::string CLogicCacheCommon::VecBitmap2Bin(const CVecBitmap& stData)
{
    if(stData.size() == 0) return std::string("0");

    const std::size_t uiSize = sizeof(VecBitmapSubtype) * 8;
    const std::size_t iMaxLength = stData.size() * uiSize;
    std::string strRet(iMaxLength, '0');
    std::size_t iIndex = 0;
    for(std::size_t i = 0; i < stData.size(); ++i)
    {
        VecBitmapSubtype iStateData = stData[i];
        for(std::size_t j = 0; j < uiSize && iIndex < iMaxLength; ++j)
        {
            strRet[iIndex++] = (iStateData & ((VecBitmapSubtype)0x1 << j)) ? '1' : '0';
        }
    }

    return strRet;
}

int32_t CLogicCacheCommon::GetItemNum(user_data_table_ptr_type pUserData, int32_t iItemType, int32_t iItemID)
{
    int32_t iCurNum = 0;

    switch (iItemType)
    {
        case CLogicConfigDefine::EXCHANGE_ITEM:
        {
            auto stIter = pUserData->m_stItemMap.Find(iItemID);
            if (stIter != pUserData->m_stItemMap.End())
            {
                iCurNum = stIter->second->GetAmount();
            }

            break;
        }
        case CLogicConfigDefine::CURRENCY:
        {
            switch (iItemID)
            {
            case CLogicConfigDefine::ECIT_GoldID:
                iCurNum = pUserData->m_stUserInfoTable.GetGold();
                break;
            case CLogicConfigDefine::ECIT_EnergyID:
                iCurNum = pUserData->m_stUserInfoTable.GetPhyPower();
                break;
            case CLogicConfigDefine::ECIT_YuanBaoID:
                iCurNum = pUserData->m_stUserInfoTable.GetRmb() + pUserData->m_stUserInfoTable.GetPayRmb();
                break;
            case CLogicConfigDefine::ECIT_PayYuanBaoID:
                iCurNum = pUserData->m_stUserInfoTable.GetPayRmb();
                break;
            case CLogicConfigDefine::ECIT_VIPExpID:
                iCurNum = pUserData->m_stUserInfoTable.GetVipExp();
                break;
            case CLogicConfigDefine::ECIT_EvilBrandID:
                iCurNum = pUserData->m_stUserInfoTable.GetEvilBrand();
                break;
            case CLogicConfigDefine::ECIT_VitalityID:
                iCurNum = pUserData->m_stUserInfoTable.GetVitality();
                break;
            case CLogicConfigDefine::ECIT_GuildCoinID:
                iCurNum = pUserData->m_stUserInfoTable.GetGuildCoin();
                break;
            case CLogicConfigDefine::ECIT_WarContributionID:
                iCurNum = pUserData->m_stUserInfoTable.GetEntireWarContribution();
                break;
            case CLogicConfigDefine::ECIT_ConstelExpID:
                iCurNum = pUserData->m_stIBusyData.GetConstelExpPool();
                break;
            case CLogicConfigDefine::ECIT_ZhuHaiScore:
                iCurNum = pUserData->m_stUserInfoTable.GetZhuhaiScore();
                break;
            default:
                break;
            }

            break;
        }
        default:
            break;
    }

    return iCurNum;
}

bool CLogicCacheCommon::HasEnoughItem(user_data_table_ptr_type pUserData, const CPackGameItem& stNeedItem)
{
    return GetItemNum(pUserData, stNeedItem.m_iItemType, stNeedItem.m_iCardID) >= stNeedItem.m_iNum;
}

bool CLogicCacheCommon::CombineOneKey(user_data_table_ptr_type pUserData, int32_t iItemID, int32_t iNum, std::map<int, std::pair<int, int>>& stConsume)
{
    auto iterConfig = CLogicConfig::GetFragmentCombineConfig().GetConfig(iItemID);
    if(nullptr == iterConfig)
        return false;

    for(auto& item : iterConfig->m_stConsumeItem)
    {
        // 当前道具总消耗
        int iNeedNum = item.m_iNum * iNum;
        auto iter = stConsume.find(item.m_iCardID);
        if(iter != stConsume.end())
            iNeedNum += iter->second.second;

        int iCurNum = GetItemNum(pUserData, item.m_iItemType, item.m_iCardID);
        if(iCurNum >= iNeedNum)
        {
            stConsume[item.m_iCardID] = std::make_pair(item.m_iItemType, iNeedNum);
        }
        else
        {
            if(iCurNum > 0)
            {
                stConsume[item.m_iCardID] = std::make_pair(item.m_iItemType, iCurNum);
                iNeedNum -= iCurNum;
            }

            if(CombineOneKey(pUserData, item.m_iCardID, iNeedNum, stConsume) == false)
                return false;
        }
    }

    return true;
}

bool CLogicCacheCommon::AddConstelItem(user_data_table_ptr_type pUserData, int32_t iGrade, int32_t iEquipTag, int32_t iSuit)
{
    TUserConstellationTableValueType stValue;
    stValue.m_iUid = pUserData->m_iUin;
    stValue.m_iGroupID = pUserData->m_iGroupID;
    stValue.m_iUniqueID = CLogicCacheCommon::GetUserPrimaryKeyIndex(pUserData);
    stValue.m_iGrade = iGrade;
    stValue.m_iEquipmentTag = iEquipTag;
    stValue.m_iSuit = iSuit;

    const auto& stConfig = CLogicConfig::GetConstelAttrConfig();

    // 随机主属性类型
    auto iterMainAttr = stConfig.m_stMainAttrRand.find(iEquipTag);
    if(iterMainAttr == stConfig.m_stMainAttrRand.end())
        return false;

    stValue.m_iMainAttrType = RandStlCont(iterMainAttr->second.m_stAttrWeight, iterMainAttr->second.m_iTotalWeight);
    if(stValue.m_iMainAttrType < 0)
        return false;

    // 随机副属性
    auto iterGradeConfig = stConfig.m_stGradeAttrRand.find(iGrade);
    if(iterGradeConfig == stConfig.m_stGradeAttrRand.end())
        return false;

    auto& stGradeConfig = iterGradeConfig->second;

    // 随机副属性数量
    int32_t iExAttrNum = RandStlCont(stGradeConfig.m_stAttrNumWeight.m_stAttrWeight, stGradeConfig.m_stAttrNumWeight.m_iTotalWeight);
    if(iExAttrNum < 1)
        return false;

    int32_t iTotalWeight = 0;
    std::map<int32_t, int32_t> stAttrWeight;
    for(auto& iter : stGradeConfig.m_stExAttr)
    {
        stAttrWeight[iter.first] = iter.second.m_iWeight;
        iTotalWeight += iter.second.m_iWeight;
    }
    while (iExAttrNum--)
    {
        int32_t iAttrType = RandStlCont(stAttrWeight, iTotalWeight);
        if(iAttrType < 0)
            return false;

        auto iterEx = stGradeConfig.m_stExAttr.find(iAttrType);
        if(iterEx == stGradeConfig.m_stExAttr.end())
            return false;

        int32_t iAttrValue = CLogicCommon::GetRandNum(iterEx->second.m_iValueMin, iterEx->second.m_iValueMax, true);
        stValue.m_stExAttr[iAttrType] = std::make_pair(iAttrValue, 1);

        stAttrWeight.erase(iterEx->first);
        iTotalWeight -= iterEx->second.m_iWeight;
    }

    auto iterResult = pUserData->m_stConstelMap.Insert(std::make_pair(stValue.m_iUniqueID, stValue));
    if(!iterResult.second)
    {
        return false;
    }

    auto& iterData = iterResult.first;

    // 缓存
    pUserData->m_stCacheBonusData.m_stBonus.m_stConstelVec.push_back(CLogicCacheCommon::GetConstellationInfo(iterData->second->m_stData));
    // bill 记录
    CLogicBill::SaveConstelBill(pUserData->m_iUin, pUserData->m_iGroupID, pUserData->m_iChannelID, pUserData->m_iCommand, 1, iterData->second->m_stData);

    return true;
}

void CLogicCacheCommon::GetHomeExploreReward(user_data_table_ptr_type pUserData, int iQuestID, const CPackCardTeam& stTeam, std::vector<CPackGameItem>& stRewards)
{
    auto pConfig = CLogicConfig::GetHomeGardenExploreConfig().GetExploreQuestConfig(iQuestID);
    if(pConfig == nullptr)
        return;

    stRewards.insert(stRewards.end(), pConfig->m_stRewards.begin(), pConfig->m_stRewards.end());
    if(!pConfig->m_strCardBag.empty())
        CLogicConfig::GetCardbagConfig().GetRandBonusByName(pConfig->m_strCardBag, stRewards);

    for(const auto& stRecommend : pConfig->m_stRecommendVec)
    {
        if(std::find(stTeam.begin(), stTeam.end(), stRecommend.m_iCardID) == stTeam.end())
            continue;

        stRewards.insert(stRewards.end(), stRecommend.m_stRewards.begin(), stRecommend.m_stRewards.end());
        if(!stRecommend.m_strCardBag.empty())
            CLogicConfig::GetCardbagConfig().GetRandBonusByName(stRecommend.m_strCardBag, stRewards);
    }
}

bool CLogicCacheCommon::CheckTeamMemberValid(user_data_table_ptr_type pUserData, const CPackCardTeam& stTeam)
{
    if(stTeam.size() > CLogicConfigDefine::ESD_TeamMaxCardNum)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(PARAMETER_ERROR);
        return (false);
    }

    size_t iValidMemberCount = 0;
    std::set<int> stTeamCardID;
    for(auto& id : stTeam)
    {
        if(id == 0) continue;

        ++iValidMemberCount;
        stTeamCardID.insert(id);
    }

    if (stTeamCardID.empty())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(TEAM_MEMBER_CAN_NOT_EMPTY);
        return (false);
    }

    if (stTeamCardID.size() != iValidMemberCount)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(TEAM_MEMBER_HAS_DUPLICATE_CARD);
        return (false);
    }

    for (auto iCardID : stTeamCardID)
    {
        auto iter = pUserData->m_stHeroCardTableMap.Find(iCardID);
        if (iter == pUserData->m_stHeroCardTableMap.End())
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(TEAM_MEMBER_CARD_ID_IS_NOT_OWNED_BY_USER);
            return (false);
        }
    }

    return (true);
}

bool CLogicCacheCommon::SetTeam(user_data_table_ptr_type pUserData, int32_t iType, CPackCardTeam& stTeam)
{
    if(!CLogicConfigDefine::IsValidEnumTeamType(iType))
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(PARAMETER_ERROR);
        return false;
    }

    if (!CheckTeamMemberValid(pUserData, stTeam))
        return (false);

    if(stTeam.size() < CLogicConfigDefine::ESD_TeamMaxCardNum)
    {
        stTeam.resize(CLogicConfigDefine::ESD_TeamMaxCardNum);
    }

    switch (iType)
    {
    case CLogicConfigDefine::ETT_MainTeam:
        pUserData->m_stUserTeamInfoTable.SetTeam1(stTeam); break;
    case CLogicConfigDefine::ETT_PvpTeam:
        pUserData->m_stUserTeamInfoTable.SetTeam2(stTeam); break;
    case CLogicConfigDefine::ETT_CrossFight:
        pUserData->m_stUserTeamInfoTable.SetTeam3(stTeam); break;
    case CLogicConfigDefine::ETT_AtkTeam:
        pUserData->m_stUserTeamInfoTable.SetTeam4(stTeam); break;
    case CLogicConfigDefine::ETT_DefTeam:
        pUserData->m_stUserTeamInfoTable.SetTeam5(stTeam); break;
    case CLogicConfigDefine::ETT_SupTeam:
        pUserData->m_stUserTeamInfoTable.SetTeam6(stTeam); break;
    case CLogicConfigDefine::ETT_PvpDefense:
        pUserData->m_stUserTeamInfoTable.SetTeam7(stTeam); break;
    case CLogicConfigDefine::ETT_EntireWarTeam:
        pUserData->m_stUserTeamInfoTable.SetTeam8(stTeam); break;
    case CLogicConfigDefine::ETT_QiyuTeam:
        pUserData->m_stUserTeamInfoTable.SetTeam9(stTeam); break;
    case CLogicConfigDefine::ETT_CangbaotuTeam:
        pUserData->m_stUserTeamInfoTable.SetTeam10(stTeam); break;

    case CLogicConfigDefine::ETT_Team11:
        pUserData->m_stUserTeamInfoTable.SetTeam11(stTeam); break;
    case CLogicConfigDefine::ETT_Team12:
        pUserData->m_stUserTeamInfoTable.SetTeam12(stTeam); break;
    case CLogicConfigDefine::ETT_ZhuHaiTeam:
        pUserData->m_stUserTeamInfoTable.SetTeam13(stTeam); break;
    case CLogicConfigDefine::ETT_Team14:
        pUserData->m_stUserTeamInfoTable.SetTeam14(stTeam); break;
    case CLogicConfigDefine::ETT_Team15:
        pUserData->m_stUserTeamInfoTable.SetTeam15(stTeam); break;
    case CLogicConfigDefine::ETT_Team16:
        pUserData->m_stUserTeamInfoTable.SetTeam16(stTeam); break;
    case CLogicConfigDefine::ETT_Team17:
        pUserData->m_stUserTeamInfoTable.SetTeam17(stTeam); break;
    case CLogicConfigDefine::ETT_Team18:
        pUserData->m_stUserTeamInfoTable.SetTeam18(stTeam); break;
    case CLogicConfigDefine::ETT_Team19:
        pUserData->m_stUserTeamInfoTable.SetTeam19(stTeam); break;
    case CLogicConfigDefine::ETT_Team20:
        pUserData->m_stUserTeamInfoTable.SetTeam20(stTeam); break;

    case CLogicConfigDefine::ETT_Team21:
        pUserData->m_stUserTeamInfoTable.SetTeam21(stTeam); break;
    case CLogicConfigDefine::ETT_Team22:
        pUserData->m_stUserTeamInfoTable.SetTeam22(stTeam); break;
    case CLogicConfigDefine::ETT_Team23:
        pUserData->m_stUserTeamInfoTable.SetTeam23(stTeam); break;
    case CLogicConfigDefine::ETT_Team24:
        pUserData->m_stUserTeamInfoTable.SetTeam24(stTeam); break;
    case CLogicConfigDefine::ETT_Team25:
        pUserData->m_stUserTeamInfoTable.SetTeam25(stTeam); break;
    case CLogicConfigDefine::ETT_Team26:
        pUserData->m_stUserTeamInfoTable.SetTeam26(stTeam); break;
    case CLogicConfigDefine::ETT_Team27:
        pUserData->m_stUserTeamInfoTable.SetTeam27(stTeam); break;
    case CLogicConfigDefine::ETT_Team28:
        pUserData->m_stUserTeamInfoTable.SetTeam28(stTeam); break;
    case CLogicConfigDefine::ETT_Team29:
        pUserData->m_stUserTeamInfoTable.SetTeam29(stTeam); break;
    case CLogicConfigDefine::ETT_Team30:
        pUserData->m_stUserTeamInfoTable.SetTeam30(stTeam); break;

    default:
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(PARAMETER_ERROR);
        return false;
    }

    return true;
}

CPackCardTeam CLogicCacheCommon::GetTeam(user_data_table_ptr_type pUserData, int32_t iType)
{
    CPackCardTeam stEmpty;
    switch (iType)
    {
    case CLogicConfigDefine::ETT_MainTeam:
        return pUserData->m_stUserTeamInfoTable.GetTeam1();
    case CLogicConfigDefine::ETT_PvpTeam:
        return pUserData->m_stUserTeamInfoTable.GetTeam2();
    case CLogicConfigDefine::ETT_CrossFight:
        return pUserData->m_stUserTeamInfoTable.GetTeam3();
    case CLogicConfigDefine::ETT_AtkTeam:
        return pUserData->m_stUserTeamInfoTable.GetTeam4();
    case CLogicConfigDefine::ETT_DefTeam:
        return pUserData->m_stUserTeamInfoTable.GetTeam5();
    case CLogicConfigDefine::ETT_SupTeam:
        return pUserData->m_stUserTeamInfoTable.GetTeam6();
    case CLogicConfigDefine::ETT_PvpDefense:
        return pUserData->m_stUserTeamInfoTable.GetTeam7();
    case CLogicConfigDefine::ETT_EntireWarTeam:
        return pUserData->m_stUserTeamInfoTable.GetTeam8();
    case CLogicConfigDefine::ETT_QiyuTeam:
        return pUserData->m_stUserTeamInfoTable.GetTeam9();
    case CLogicConfigDefine::ETT_CangbaotuTeam:
        return pUserData->m_stUserTeamInfoTable.GetTeam10();

    case CLogicConfigDefine::ETT_Team11:
        return pUserData->m_stUserTeamInfoTable.GetTeam11();
    case CLogicConfigDefine::ETT_Team12:
        return pUserData->m_stUserTeamInfoTable.GetTeam12();
    case CLogicConfigDefine::ETT_ZhuHaiTeam:
        return pUserData->m_stUserTeamInfoTable.GetTeam13();
    case CLogicConfigDefine::ETT_Team14:
        return pUserData->m_stUserTeamInfoTable.GetTeam14();
    case CLogicConfigDefine::ETT_Team15:
        return pUserData->m_stUserTeamInfoTable.GetTeam15();
    case CLogicConfigDefine::ETT_Team16:
        return pUserData->m_stUserTeamInfoTable.GetTeam16();
    case CLogicConfigDefine::ETT_Team17:
        return pUserData->m_stUserTeamInfoTable.GetTeam17();
    case CLogicConfigDefine::ETT_Team18:
        return pUserData->m_stUserTeamInfoTable.GetTeam18();
    case CLogicConfigDefine::ETT_Team19:
        return pUserData->m_stUserTeamInfoTable.GetTeam19();
    case CLogicConfigDefine::ETT_Team20:
        return pUserData->m_stUserTeamInfoTable.GetTeam20();

    case CLogicConfigDefine::ETT_Team21:
        return pUserData->m_stUserTeamInfoTable.GetTeam21();
    case CLogicConfigDefine::ETT_Team22:
        return pUserData->m_stUserTeamInfoTable.GetTeam22();
    case CLogicConfigDefine::ETT_Team23:
        return pUserData->m_stUserTeamInfoTable.GetTeam23();
    case CLogicConfigDefine::ETT_Team24:
        return pUserData->m_stUserTeamInfoTable.GetTeam24();
    case CLogicConfigDefine::ETT_Team25:
        return pUserData->m_stUserTeamInfoTable.GetTeam25();
    case CLogicConfigDefine::ETT_Team26:
        return pUserData->m_stUserTeamInfoTable.GetTeam26();
    case CLogicConfigDefine::ETT_Team27:
        return pUserData->m_stUserTeamInfoTable.GetTeam27();
    case CLogicConfigDefine::ETT_Team28:
        return pUserData->m_stUserTeamInfoTable.GetTeam28();
    case CLogicConfigDefine::ETT_Team29:
        return pUserData->m_stUserTeamInfoTable.GetTeam29();
    case CLogicConfigDefine::ETT_Team30:
        return pUserData->m_stUserTeamInfoTable.GetTeam30();

    default:
        return stEmpty;
    }

    return stEmpty;
}

int CLogicCacheCommon::CheckGuildName(std::string& strGuildName)
{
    strGuildName.erase(0, strGuildName.find_first_not_of(" "));
    strGuildName.erase(strGuildName.find_last_not_of(" ") + 1);

    const auto iMaxDisplayNickCharWidth = CLogicConfig::GetGlobalConfig().m_iMaxNickNameCharWidth;
    const auto iMinDisplayNickCharWidth = CLogicConfig::GetGlobalConfig().m_iMinNickNameCharWidth;
    auto iInputNickDisplayWidth = 0;

    const char* illegal = ",\n\\\t\'\"/";
    for (size_t i = 0; i < strlen(illegal); ++i)
    {
        if (strGuildName.find(illegal[i]) != std::string::npos)
        {
            return SERVER_ERRCODE::GUILD_NAME_HAS_ILLEGAL_CHAR;
        }
    }

    if (LOGIC_SERVICE_API_INS->CheckNickCorrect(strGuildName, iInputNickDisplayWidth) != 0)
    {
        return SERVER_ERRCODE::GUILD_NAME_HAS_ILLEGAL_CHAR;
    }

    if (iInputNickDisplayWidth > iMaxDisplayNickCharWidth)
    {
        return SERVER_ERRCODE::GUILD_NAME_LENGTH_TOO_LARGE;
    }

    if (iInputNickDisplayWidth < iMinDisplayNickCharWidth)
    {
        return SERVER_ERRCODE::GUILD_NAME_LENGTH_TOO_SMALL;
    }

    if (LOGIC_SERVICE_API_INS->CheckNickHasDirtyWord(strGuildName))
    {
        return SERVER_ERRCODE::GUILD_NAME_HAS_DIRTY_WORD;
    }

    return SERVER_ERRCODE::SUCCESS;
}

int CLogicCacheCommon::CheckNickValid(std::string& strName)
{
    int iMaxDisplayNickCharWidth = CLogicConfig::GetGlobalConfig().m_iMaxNickNameCharWidth;
    int iMinDisplayNickCharWidth = CLogicConfig::GetGlobalConfig().m_iMinNickNameCharWidth;

    int iInputNickDisplayWidth = 0;
    auto iErrorCode = LOGIC_SERVICE_API_INS->CheckNickCorrect(strName, iInputNickDisplayWidth);
    if(iErrorCode == -4)
    {
        return SERVER_ERRCODE::NICK_HAS_BLANK;
    }
    else if(iErrorCode == -5)
    {
        return SERVER_ERRCODE::NICK_IS_ALL_DIGITAL;
    }
    else if (iErrorCode != 0)
    {
        return SERVER_ERRCODE::NICK_HAS_ILLEGAL_CHAR;
    }

    // 数据库最大长度限制
    const int iLenLimitByDB = 30;

    if(iInputNickDisplayWidth > iLenLimitByDB || strName.length() > iLenLimitByDB)
    {
        return SERVER_ERRCODE::NICK_LENGTH_TOO_LARGE;
    }

    if (iInputNickDisplayWidth > iMaxDisplayNickCharWidth)
    {
        return SERVER_ERRCODE::NICK_LENGTH_TOO_LARGE;
    }

    if (iInputNickDisplayWidth < iMinDisplayNickCharWidth)
    {
        return SERVER_ERRCODE::NICK_LENGTH_TOO_SMALL;
    }

    if (LOGIC_SERVICE_API_INS->CheckNickHasDirtyWord(strName))
    {
        return SERVER_ERRCODE::NICK_HAS_DIRTY_WORD;
    }

    if (strName.find(CLogicConfig::GetGlobalConfig().m_strChangeNickStr) != std::string::npos)
    {
        return SERVER_ERRCODE::USER_ROLE_NICK_EXIST;
    }

    return SERVER_ERRCODE::SUCCESS;
}

int CLogicCacheCommon::CheckAliasNickValid(std::string& strName)
{
    int iInputNickDisplayWidth = 0;
    auto iErrorCode = LOGIC_SERVICE_API_INS->CheckNickCorrect(strName, iInputNickDisplayWidth);
    if(iErrorCode == -4)
    {
        return SERVER_ERRCODE::NICK_HAS_BLANK;
    }
    else if(iErrorCode == -5)
    {
        // return SERVER_ERRCODE::NICK_IS_ALL_DIGITAL;
    }
    else if (iErrorCode != 0)
    {
        return SERVER_ERRCODE::NICK_HAS_ILLEGAL_CHAR;
    }

    // 数据库最大长度限制
    const int iLenLimitByDB = 60;

    if(iInputNickDisplayWidth > iLenLimitByDB || strName.length() > iLenLimitByDB)
    {
        return SERVER_ERRCODE::NICK_LENGTH_TOO_LARGE;
    }

    if (LOGIC_SERVICE_API_INS->CheckNickHasDirtyWord(strName))
    {
        return SERVER_ERRCODE::NICK_HAS_DIRTY_WORD;
    }

    if (strName.find(CLogicConfig::GetGlobalConfig().m_strChangeNickStr) != std::string::npos)
    {
        return SERVER_ERRCODE::USER_ROLE_NICK_EXIST;
    }

    return SERVER_ERRCODE::SUCCESS;
}

//刷新念写卡商店
void CLogicCacheCommon::RefreshConstelShop(user_data_table_ptr_type pUserData)
{
    //刷新前先清空
    if (!pUserData->m_stIBusyData.GetConstelShop().empty())
    {
        pUserData->m_stIBusyData.SetConstelShop({});
    }

    //首先还是获取念写卡的配置，根据权重对直接用就完事了
    auto constelShopConfig = CLogicConfig::GetShopConfig().m_stConstelShopConfig;
    auto constelShopMap = CLogicConfig::GetShopConfig().m_stConstelShop;

    //首先判断是否是天降洪福
    std::vector<CPackConstelShopInfo> vecRandom;
    std::vector<std::pair<int32_t,int32_t>> luckyWeightPair;
    luckyWeightPair.emplace_back(1,constelShopConfig.m_iSpecialPro);
    luckyWeightPair.emplace_back(2,CLogicConfigDefine::ESD_RandomWeight - constelShopConfig.m_iSpecialPro);
    auto luckyRet = RandStlCont(luckyWeightPair);

    //天降洪福
    if (1 == luckyRet)
    {
        std::unordered_map<int32_t,char> selectedIndex;
        while(selectedIndex.size() != CONSTEL_SHOP_COUNT)
        {
            auto iterGradeShop = constelShopMap.find(4);
            if (iterGradeShop == constelShopMap.end())
            {
                continue;
            }

            if (iterGradeShop->second.empty())
            {
                continue;
            }

            auto shopIndex = CLogicCommon::GetRandNum(0,(int32_t)iterGradeShop->second.size());

            if (selectedIndex.find(shopIndex) != selectedIndex.end())
            {
                continue;
            }

            selectedIndex.insert(std::make_pair(shopIndex,1));
            CPackConstelShopInfo shopInfo;
            shopInfo.m_iHasBuy = false;
            shopInfo.m_iHalfPrice = true;
            shopInfo.m_iIndex = iterGradeShop->second[shopIndex];
            vecRandom.emplace_back(shopInfo);
        }
    } else if (2 == luckyRet)
    {
        std::vector<std::pair<int32_t,int32_t>> constelShopWeightPair;
        constelShopWeightPair.emplace_back(1,constelShopConfig.m_iGradePro3);
        constelShopWeightPair.emplace_back(2,constelShopConfig.m_iGradePro4);
        constelShopWeightPair.emplace_back(3,constelShopConfig.m_iGradePro5);
        constelShopWeightPair.emplace_back(4,constelShopConfig.m_iGradePro6);

        std::unordered_map<int32_t,char> selectedIndex;
        //先随机品阶，在随机具体的商品
        while(selectedIndex.size() != CONSTEL_SHOP_COUNT)
        {
            auto ret = RandStlCont(constelShopWeightPair);
            auto iterGradeShop = constelShopMap.find(ret);
            if (iterGradeShop == constelShopMap.end())
            {
                continue;
            }

            if (iterGradeShop->second.empty())
            {
                continue;
            }

            auto shopIndex = CLogicCommon::GetRandNum(0,(int32_t)iterGradeShop->second.size());
            if (selectedIndex.find(shopIndex) != selectedIndex.end())
            {
                continue;
            }
            selectedIndex.insert(std::make_pair(shopIndex,1));
            CPackConstelShopInfo shopInfo;
            shopInfo.m_iHasBuy = false;
            shopInfo.m_iHalfPrice = false;
            shopInfo.m_iIndex = iterGradeShop->second[shopIndex];
            vecRandom.emplace_back(shopInfo);
        }
    }

    pUserData->m_stIBusyData.SetConstelShop(vecRandom);
}










