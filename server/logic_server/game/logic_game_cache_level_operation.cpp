#include "server_errcode.h"
#include "logic_link_error.h"
#include "server_cmd_cross_define.h"
#include "server_cross_protocol.h"
#include "logic_link_common.h"
#include "logic_game_cache_common.h"
#include "logic_config.h"
#include "logic_game_buff.h"
#include "logic_game_currency_check.h"
#include "logic_game_publisher.h"
#include "logic_game_cache_level_operation.h"
#include "logic_game_user_notify_system.h"
#include "logic_game_active.h"

////////////////////////////////////////////////////////////////////////////////
CLogicCacheLevelOperation::CLogicCacheLevelOperation(user_data_table_ptr_type pUserData)
: m_pUserData(pUserData) { assert(nullptr != m_pUserData && "USERDATA CAN NOT INIT nullptr"); }

bool
CLogicCacheLevelOperation::CheckCanFightLevel(int iLevelID, const TLogicLevelElem* pLevelInfoConfigElem)
{
    // return false;

    int iNeedLevel = pLevelInfoConfigElem->m_iNeedLevel;
    int iPreLevel = pLevelInfoConfigElem->m_iPreLevel;
    int iEnterTimes = pLevelInfoConfigElem->m_iEnterTimes;
    int iConsumePhyPower = pLevelInfoConfigElem->m_iBonusTeamExp;

    if (false == CLogicCurrencyCheck::CheckEnoughRoleLevel(m_pUserData, iNeedLevel))
        return false;
    if (false == CLogicCurrencyCheck::CheckEnoughPhyPower(m_pUserData, iConsumePhyPower))
        return false;

    // 好感度副本
    if(pLevelInfoConfigElem->m_iLevelType == CLogicConfigDefine::ELT_HeartLevel)
    {
        const auto stLevelInfoIT = m_pUserData->m_stLevelInfoTableMap.Find(iLevelID);
        if((stLevelInfoIT == m_pUserData->m_stLevelInfoTableMap.End() || stLevelInfoIT->second->GetTotalVictoryTimes() <= 0) &&
            !CLogicCurrencyCheck::CheckEnoughHeroHeartLevel(m_pUserData, pLevelInfoConfigElem->m_iLevelTypePara, pLevelInfoConfigElem->m_iNeedHeartLevel))
            return false;
    }
    else if(pLevelInfoConfigElem->m_iLevelType == CLogicConfigDefine::ELT_Constel)
    {
        // 礼装副本
        if(int(m_pUserData->m_stConstelMap.Size()) + CLogicConfig::GetConstelAttrConfig().m_iOnceNeedBagSpace > CLogicConfig::GetConstelAttrConfig().m_iBagMaxSize)
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(BAG_SPACE_NOT_ENOUGH);
            return false;
        }
    }
    else if(pLevelInfoConfigElem->m_iLevelType == CLogicConfigDefine::ELT_EquipLevel)
    {
        // 装备副本
        if(int(m_pUserData->m_stEquipMap.Size()) + CLogicConfig::GetEquipAttrConfig().m_iOnceNeedBagSpace > CLogicConfig::GetEquipAttrConfig().m_iEquipBagMax)
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(EQUIPMENT_BAG_OVER_SIZE);
            return false;
        }
    }

    if (iPreLevel > 0)
    {
        auto stPreLevelInfoIT = m_pUserData->m_stLevelInfoTableMap.Find(iPreLevel);
        if (stPreLevelInfoIT == m_pUserData->m_stLevelInfoTableMap.End() || stPreLevelInfoIT->second->GetTotalVictoryTimes() <= 0)
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(PRE_LEVEL_NOT_PASSED);
            return false;
        }
    }

    if (iEnterTimes > 0)
    {
        const std::map<int32_t, int32_t> levelBackTypeMap = {
			{ CLogicConfigDefine::PAO_MA_LEVEL_ID, static_cast<int32_t>(CEnumBuyBackType::HORSE) },
			{ CLogicConfigDefine::DIAOCHAN_LEVEL_ID, static_cast<int32_t>(CEnumBuyBackType::DIAOCHAN) },
			{ CLogicConfigDefine::SINGLE_LEVEL_ID, static_cast<int32_t>(CEnumBuyBackType::SINGLE_LEVEL) },
		};

        bool bHaveBackCount = false;
        const auto iter = levelBackTypeMap.find(iLevelID);
        if(iter != levelBackTypeMap.end())
        {
            const auto iterBack = m_pUserData->m_stBuyBackMap.Find(iter->second);
            bHaveBackCount = (iterBack != m_pUserData->m_stBuyBackMap.End() && iterBack->second->GetUseBackCount() < iterBack->second->GetBuyBackCount());
        }

	    if (!bHaveBackCount)
	    {
			const auto stLevelInfoIT = m_pUserData->m_stLevelInfoTableMap.Find(iLevelID);
			if (stLevelInfoIT != m_pUserData->m_stLevelInfoTableMap.End())
			{
				if (stLevelInfoIT->second->GetPlayTimes() >= iEnterTimes)
				{
					using namespace SERVER_ERRCODE;
					SET_ERR_INFO(REACH_ENTER_LEVEL_TIMES_PER_DAY_LIMIT);
					return false;
				}
			}
	    }
    }

    int iNowTime = CLogicCommon::GetSec();
    struct tm stNowTM = CLogicCommon::GetDate();
    int iNowChecksum = (stNowTM.tm_hour * 10000) + (stNowTM.tm_min * 100) + stNowTM.tm_sec;
    int iConfigStartTimeChecksum = pLevelInfoConfigElem->m_stLevelOpenTime.m_iStartHour * 10000;
    iConfigStartTimeChecksum += pLevelInfoConfigElem->m_stLevelOpenTime.m_iStartMinute * 100;
    iConfigStartTimeChecksum += pLevelInfoConfigElem->m_stLevelOpenTime.m_iStartSecond;
    int iConfigEndTimeChecksum = pLevelInfoConfigElem->m_stLevelOpenTime.m_iEndHour * 10000;
    iConfigEndTimeChecksum += pLevelInfoConfigElem->m_stLevelOpenTime.m_iEndMinute * 100;
    iConfigEndTimeChecksum += pLevelInfoConfigElem->m_stLevelOpenTime.m_iEndSecond;

    if (!(pLevelInfoConfigElem->m_stLevelOpenTime.m_cWeekDays & (1 << stNowTM.tm_wday)))
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(LEVEL_IS_NOT_IN_OPEN_TIME);
        return false;
    }

    if ((pLevelInfoConfigElem->m_stLevelOpenTime.m_bHasOpenDate) &&
        ((iNowTime < pLevelInfoConfigElem->m_stLevelOpenTime.m_iStartDayTimestamp) ||
         (iNowTime > pLevelInfoConfigElem->m_stLevelOpenTime.m_iEndDayTimestamp)))
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(LEVEL_IS_NOT_IN_OPEN_TIME);
        return false;
    }

    if ((pLevelInfoConfigElem->m_stLevelOpenTime.m_bHasOpenHour) &&
        ((iNowChecksum < iConfigStartTimeChecksum) ||
         (iNowChecksum > iConfigEndTimeChecksum)))
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(LEVEL_IS_NOT_IN_OPEN_TIME);
        return false;
    }

    return true;
}

bool CLogicCacheLevelOperation::CheckLevelCommonLimit(TLevelInfoTable* pLevelData, const TLogicLevelElem* pConfig, int extraLevel/*=0*/ ,int iTimes/* = 1*/)
{
    if (iTimes <= 0 || iTimes >= 99)
    {
        SET_ERR_INFO(SERVER_ERRCODE::COUNT_NOT_ENOUGH);
        return false;
    }

    // 首通判断
    if(pLevelData->GetTotalVictoryTimes() <= 0)
    {
        if (false == CLogicCurrencyCheck::CheckEnoughRoleLevel(m_pUserData, pConfig->m_iNeedLevel))
            return false;

        if (pConfig->m_iPreLevel > 0)
        {
            auto iterPreLevelData = m_pUserData->m_stLevelInfoTableMap.Find(pConfig->m_iPreLevel);
            if (iterPreLevelData == m_pUserData->m_stLevelInfoTableMap.End() || iterPreLevelData->second->GetTotalVictoryTimes() <= 0)
            {
                SET_ERR_INFO(SERVER_ERRCODE::PRE_LEVEL_NOT_PASSED);
                return false;
            }
        }
    }

    if (false == CLogicCurrencyCheck::CheckEnoughPhyPower(m_pUserData, pConfig->m_iBonusTeamExp * iTimes))
        return false;

    switch (pConfig->m_iLevelType)
    {
    case CLogicConfigDefine::ELT_HeartLevel:
    {
        // 好感度副本
        if(pLevelData->GetTotalVictoryTimes() <= 0 && !CLogicCurrencyCheck::CheckEnoughHeroHeartLevel(m_pUserData, pConfig->m_iLevelTypePara, pConfig->m_iNeedHeartLevel))
            return false;

        break;
    }
    case CLogicConfigDefine::ELT_Constel:
    {
        // 礼装副本
        int iNeedSpace = CLogicConfig::GetConstelAttrConfig().m_iOnceNeedBagSpace * iTimes;
        if(int(m_pUserData->m_stConstelMap.Size()) + iNeedSpace > CLogicConfig::GetConstelAttrConfig().m_iBagMaxSize)
        {
            SET_ERR_INFO(SERVER_ERRCODE::BAG_SPACE_NOT_ENOUGH);
            return false;
        }

        break;
    }
    case CLogicConfigDefine::ELT_EquipLevel:
    {
        // 装备副本
        int iNeedSpace = CLogicConfig::GetEquipAttrConfig().m_iOnceNeedBagSpace * iTimes;
        if(int(m_pUserData->m_stEquipMap.Size()) + iNeedSpace > CLogicConfig::GetEquipAttrConfig().m_iEquipBagMax)
        {
            SET_ERR_INFO(SERVER_ERRCODE::EQUIPMENT_BAG_OVER_SIZE);
            return false;
        }

        break;
    }
    case CLogicConfigDefine::ELT_ZhuHaiLevel:
    {
        // 判断难度
        if(pLevelData->m_stData.m_iLevelID != m_pUserData->m_stIBusyData.GetZhuHaiLevel())
        {
            SET_ERR_INFO(SERVER_ERRCODE::ZHUHAI_LEVEL_ERROR);
            return false;
        }

        break;
    }
    case CLogicConfigDefine::ELT_GuildBossLevel:
    {
        if(!LOGIC_SERVICE_API_INS->GetCrossConnection()->IsConnected())
        {
            SET_ERR_INFO(SERVER_ERRCODE::MATCH_SERVER_NOT_CONN);
            return false;
        }

        // 检查时间
        int iCurTime = CLogicCommon::GetSec();
        if(!is_same_day(iCurTime, iCurTime + 600))
        {
            SET_ERR_INFO(SERVER_ERRCODE::GUILD_BOSS_RESETING);
            return false;
        }

        // 判断是否有公会
        if(m_pUserData->m_stUserInfoTable.GetGuildID() <= 0)
        {
            SET_ERR_INFO(SERVER_ERRCODE::GUILD_MEMBER_NOT_FOUND);
            return false;
        }

        //判断当前是否可以打对应的念写卡副本
        auto levelConfig = CLogicConfig::GetLevelConfig().GetLevelConfigElem(extraLevel);
        if (nullptr == levelConfig)
        {
            SET_ERR_INFO(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
            return false;
        }

        if (levelConfig->m_iPreLevel > 0)
        {
            auto iterPreLevelData = m_pUserData->m_stLevelInfoTableMap.Find(levelConfig->m_iPreLevel);
            if (iterPreLevelData == m_pUserData->m_stLevelInfoTableMap.End() || iterPreLevelData->second->GetTotalVictoryTimes() <= 0)
            {
                SET_ERR_INFO(SERVER_ERRCODE::PRE_LEVEL_NOT_PASSED);
                return false;
            }
        }

        break;
    }
    default:
        break;
    }

    // 判断次数
    if(!pConfig->m_bFirstPassFree || pLevelData->GetTotalVictoryTimes())
    {
        // 该副本每日次数
        if(pConfig->m_iEnterTimes > 0)
        {
            if (LOGIC_CHECK_DAY_REFRESH(pLevelData->GetLastPlayTime()))
            {
                pLevelData->SetSharedTimes(0);
                pLevelData->SetPlayTimes(0);
                pLevelData->SetLastPlayTime(CLogicCommon::GetSec());
            }

            if (pLevelData->GetPlayTimes() + iTimes > pConfig->m_iEnterTimes)
            {
                SET_ERR_INFO(SERVER_ERRCODE::REACH_ENTER_LEVEL_TIMES_PER_DAY_LIMIT);
                return false;
            }
        }

        // 共享副本次数
        if(pConfig->m_iSharedLevel > 0)
        {
            auto* pTimesConfig = CLogicConfig::GetLevelConfig().GetLevelConfigElem(pConfig->m_iSharedLevel);
            if(nullptr == pTimesConfig)
            {
                SET_ERR_INFO(SERVER_ERRCODE::INTERN_ERROR);
                return false;
            }

            auto pTimesData = GetOrCreateLevelInfo(pConfig->m_iSharedLevel);
            if (nullptr == pTimesData)
            {
                return false;
            }

            if (LOGIC_CHECK_DAY_REFRESH(pTimesData->GetLastPlayTime()))
            {
                pTimesData->SetSharedTimes(0);
                pTimesData->SetPlayTimes(0);
                pTimesData->SetLastPlayTime(CLogicCommon::GetSec());
                pTimesData->SetAdvRewardTimes(0);
            }

            // 月卡次数
            int iAddTimes = 0;
            if(CLogicBuff::IsHaveMonthCard(m_pUserData))
            {
                iAddTimes = CLogicConfig::GetAwardConfig().GetMonthCardAddLevelTimes(pConfig->m_iLevelType);
            }

            if (pTimesData->GetSharedTimes() + iTimes > pTimesConfig->m_iSharedTimes + iAddTimes)
            {
                SET_ERR_INFO(SERVER_ERRCODE::REACH_ENTER_LEVEL_TIMES_PER_DAY_LIMIT);
                return false;
            }
        }
    }

    int iNowTime = CLogicCommon::GetSec();
    struct tm stNowTM = CLogicCommon::GetDate();
    int iNowChecksum = (stNowTM.tm_hour * 10000) + (stNowTM.tm_min * 100) + stNowTM.tm_sec;
    int iConfigStartTimeChecksum = pConfig->m_stLevelOpenTime.m_iStartHour * 10000;
    iConfigStartTimeChecksum += pConfig->m_stLevelOpenTime.m_iStartMinute * 100;
    iConfigStartTimeChecksum += pConfig->m_stLevelOpenTime.m_iStartSecond;
    int iConfigEndTimeChecksum = pConfig->m_stLevelOpenTime.m_iEndHour * 10000;
    iConfigEndTimeChecksum += pConfig->m_stLevelOpenTime.m_iEndMinute * 100;
    iConfigEndTimeChecksum += pConfig->m_stLevelOpenTime.m_iEndSecond;

    if (!(pConfig->m_stLevelOpenTime.m_cWeekDays & (1 << stNowTM.tm_wday)))
    {
        SET_ERR_INFO(SERVER_ERRCODE::LEVEL_IS_NOT_IN_OPEN_TIME);
        return false;
    }

    if ((pConfig->m_stLevelOpenTime.m_bHasOpenDate) &&
        ((iNowTime < pConfig->m_stLevelOpenTime.m_iStartDayTimestamp) || (iNowTime > pConfig->m_stLevelOpenTime.m_iEndDayTimestamp)))
    {
        SET_ERR_INFO(SERVER_ERRCODE::LEVEL_IS_NOT_IN_OPEN_TIME);
        return false;
    }

    if ((pConfig->m_stLevelOpenTime.m_bHasOpenHour) &&
        ((iNowChecksum < iConfigStartTimeChecksum) || (iNowChecksum > iConfigEndTimeChecksum)))
    {
        SET_ERR_INFO(SERVER_ERRCODE::LEVEL_IS_NOT_IN_OPEN_TIME);
        return false;
    }

    return true;
}

bool CLogicCacheLevelOperation::CheckLevelFightBefore(TLevelInfoTable* pLevelData, const TLogicLevelElem* pConfig, const CPackLevelFightPara& stParam)
{
    if(stParam.m_iGetCoinNum < 0)
    {
        SET_ERR_INFO(SERVER_ERRCODE::PARAMETER_ERROR);
        return false;
    }

    if(stParam.m_stCardStatus.size() > CLogicConfigDefine::ESD_TeamMaxCardNum)
    {
        SET_ERR_INFO(SERVER_ERRCODE::PARAMETER_ERROR);
        return false;
    }

    for(auto iter : stParam.m_stCardStatus)
    {
        if(iter.first > 0 && m_pUserData->m_stHeroCardTableMap.Find(iter.first) == m_pUserData->m_stHeroCardTableMap.End())
        {
            SET_ERR_INFO(SERVER_ERRCODE::PARAMETER_ERROR);
            return false;
        }
    }

    switch (pConfig->m_iLevelType)
    {
    case CLogicConfigDefine::ELT_ZhuHaiLevel:
    {
        if(m_pUserData->m_stIBusyData.GetZhuHaiLevel() != pLevelData->m_stData.m_iLevelID)
        {
            SET_ERR_INFO(SERVER_ERRCODE::ZHUHAI_LEVEL_ERROR);
            return false;
        }

        break;
    }
    case CLogicConfigDefine::ELT_GuildBossLevel:
    {
        if(!LOGIC_SERVICE_API_INS->GetCrossConnection()->IsConnected())
        {
            SET_ERR_INFO(SERVER_ERRCODE::MATCH_SERVER_NOT_CONN);
            return false;
        }

        // 检查时间
        int iCurTime = CLogicCommon::GetSec();
        if(!is_same_day(iCurTime, iCurTime + 600))
        {
            SET_ERR_INFO(SERVER_ERRCODE::GUILD_BOSS_RESETING);
            return false;
        }

        // 公会BOSS副本伤害量限制
        if(stParam.m_ulPara <= 0 || stParam.m_ulPara/10 >= std::numeric_limits<int32_t>::max())
        {
            SET_ERR_INFO(SERVER_ERRCODE::PARAMETER_ERROR);
            return false;
        }

        break;
    }
    }

    return true;
}

bool CLogicCacheLevelOperation::HandleLevelFightAfter(TLevelInfoTable* pLevelData, const TLogicLevelElem* pConfig, const CPackLevelFightPara& stParam)
{
    switch (pConfig->m_iLevelType)
    {
    case CLogicConfigDefine::ELT_GuildBossLevel:
    {
        // 更新伤害量
        auto stHurtMap = m_pUserData->m_stIBusyData.GetGuildBossHurt();
        if(stHurtMap.find(pLevelData->m_stData.m_iLevelID) == stHurtMap.end())
        {
            if(stHurtMap.size() >= CLogicConfigGuildParser::E_MaxGuildBoss)
            {
                stHurtMap.clear();
            }
        }

        if(stHurtMap[pLevelData->m_stData.m_iLevelID] < stParam.m_ulPara)
        {
            stHurtMap[pLevelData->m_stData.m_iLevelID] = stParam.m_ulPara;
            m_pUserData->m_stIBusyData.SetGuildBossHurt(stHurtMap);
        }

        CMsgHeadCross stMsgHead;
        stMsgHead.m_byCmdType = enmCmdType_request;
        stMsgHead.m_usCmd = MSG_CROSS_GUILD_BOSS_HURT_NOTIFY;
        stMsgHead.m_iUin = m_pUserData->m_iUin;
        stMsgHead.m_nGroupID = m_pUserData->m_iGroupID;

        CCrossGuildBossHurtNotify stCross;
        stCross.m_ulHurt = stParam.m_ulPara;
        LOGIC_SERVICE_API_INS->GetCrossConnection()->SendToServer(stMsgHead, stCross);

        break;
    }
    }

    return true;
}

bool
CLogicCacheLevelOperation::CheckFightLevelID(int iLevelID)
{
    if (iLevelID <= 0 || m_pUserData->m_stUserInfoTable.GetFightLevelID() != iLevelID)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(FIGHT_LEVEL_MISMATCH);
        return false;
    }
    return true;
}

char CLogicCacheLevelOperation::CalcLevelStar(const TLogicLevelElem* pConfig, const CPackLevelFightPara& stParam)
{
    char cStarFlag = 0;
    if(pConfig->m_iLevelType == CLogicConfigDefine::ELT_ShowLevel)
        return cStarFlag;

    for(std::size_t i = 0; i < CLogicConfigDefine::ESD_LevelStarEvaluateNum; ++i)
    {
        bool bReach = false;
        if(pConfig->m_stEvaluate.size() > i)
        {
            auto& stEvaluate = pConfig->m_stEvaluate[i];
            switch (stEvaluate.m_iType)
            {
            case CLogicConfigDefine::ELET_GoldCollectRate:
                bReach = CalcGoldCollectingRate(pConfig, stParam, stEvaluate);
                break;
            case CLogicConfigDefine::ELET_MonsterKillRate:
                bReach = CalcMonsterKillRate(pConfig, stParam, stEvaluate);
                break;
            case CLogicConfigDefine::ELET_DeadCardNum:
                bReach = CalcDeadRoleNum(pConfig, stParam, stEvaluate);
                break;
            case CLogicConfigDefine::ELET_TeamRemainHpRate:
                bReach = CalcTeamRemainHPRate(pConfig, stParam, stEvaluate);
                break;
            case CLogicConfigDefine::ELET_BossTimeLimit:
                bReach = CalcBOSSTimeLimit(pConfig, stParam, stEvaluate);
                break;
            case CLogicConfigDefine::ELET_GreaterAmount:
                bReach = CalcGreaterAmount(pConfig, stParam, stEvaluate);
                break;
            default:
                break;
            }
        }
        else
        {
            bReach = true;
        }

        if(bReach) cStarFlag |= (1 << i);
    }

    return cStarFlag;
}

std::pair<user_data_table_t::level_info_container_type::iterator, bool>
CLogicCacheLevelOperation::GetUserLevelInfo(user_data_table_ptr_type pUserData, int iLevelID)
{
    std::pair<user_data_table_t::level_info_container_type::iterator, bool> stRet;
    stRet.second = true;
    stRet.first = pUserData->m_stLevelInfoTableMap.Find(iLevelID);

    if (stRet.first == pUserData->m_stLevelInfoTableMap.End())
    {
        TLevelInfoTableValueType stVal;
        stVal.m_iLevelID = iLevelID;
        stVal.m_iGroupID = pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        stVal.m_iUid = pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iPlayTimes = 0;
        stVal.m_iLastPlayTime = 0;
        stVal.m_iResetTimes = 0;
        stVal.m_iLastResetTime = 0;
        stVal.m_iTotalVictoryTimes = 0;

        stRet = pUserData->m_stLevelInfoTableMap.Insert(std::make_pair(iLevelID, stVal));
        if (stRet.second == false)
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(INTERN_ERROR);
            std::stringstream strErrInfo;
            strErrInfo << "ERROR|ERRCODE:" << INTERN_ERROR << "|MSG:" << GET_ERRMSG_BY_ERRCODE(INTERN_ERROR)
                       << "|REQ_LEVEL_ID:" << iLevelID << "|INSERT_INTO_CACHE_FAILD " << std::endl;
            CLogicError::AppendErrMsg(strErrInfo.str());
            return stRet;
        }
    }
    return stRet;
}

TLevelInfoTable* CLogicCacheLevelOperation::GetOrCreateLevelInfo(int iLevelID)
{
    auto iterData = m_pUserData->m_stLevelInfoTableMap.Find(iLevelID);
    if (iterData == m_pUserData->m_stLevelInfoTableMap.End())
    {
        TLevelInfoTableValueType stVal;
        stVal.m_iLevelID = iLevelID;
        stVal.m_iGroupID = m_pUserData->m_stUserInfoTable.m_stData.m_iGroupID;
        stVal.m_iUid = m_pUserData->m_stUserInfoTable.m_stData.m_iUid;
        stVal.m_iPlayTimes = 0;
        stVal.m_iLastPlayTime = CLogicCommon::GetSec();
        stVal.m_iResetTimes = 0;
        stVal.m_iLastResetTime = 0;
        stVal.m_iTotalVictoryTimes = 0;
        stVal.m_iAdvRewardTimes = 0;

        auto stRet = m_pUserData->m_stLevelInfoTableMap.Insert(std::make_pair(iLevelID, stVal));
        if (stRet.second == false)
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(INTERN_ERROR);
            std::stringstream strErrInfo;
            strErrInfo << "ERROR|ERRCODE:" << INTERN_ERROR << "|MSG:" << GET_ERRMSG_BY_ERRCODE(INTERN_ERROR)
                       << "|REQ_LEVEL_ID:" << iLevelID << "|INSERT_INTO_CACHE_FAILD " << std::endl;
            CLogicError::AppendErrMsg(strErrInfo.str());
            return nullptr;
        }

        iterData = stRet.first;
    }

    return iterData->second;
}

std::pair<uint64_t, bool>
CLogicCacheLevelOperation::CheckGetChapterBonus(user_data_table_ptr_type pUserData, const TLogicGameChapterElem* pstChapterConfigElem, int iChapterID, int iPosition)
{
    std::pair<uint64_t, bool> stRet(0, true);

    if (iPosition < 0 || iPosition > CLogicConfigDefine::MAX_NORMAL_CHAPTER_BONUS_AMOUNT)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(PARAMETER_ERROR);
        stRet.second = false;
        return stRet;
    }

    uint64_t ulChapterBonus = GetChapterBonusBitmap(pUserData, iChapterID, iPosition);
    if (ulChapterBonus == (uint64_t)-1)
    {
        stRet.second = false;
        return stRet;
    }

    uint64_t ulChapterPos = (1 << ((iChapterID % 1000) - 1));
    if (ulChapterBonus & ulChapterPos) stRet.second = false;
    else stRet.first = ulChapterBonus | ulChapterPos;

    if (false == stRet.second)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(CHAPTER_STAR_BONUS_HAS_RECEIVED);
        return stRet;
    }

    if (pstChapterConfigElem->m_stStarLimit.size() < (size_t)iPosition)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(CHAPTER_BONUS_NOT_CONFIGED);
        stRet.second = false;
        return stRet;
    }

    int iTotalStar = 0;
    int iStarLimit = pstChapterConfigElem->m_stStarLimit[iPosition];
    for (int iLevelID : pstChapterConfigElem->m_stLevelIDList)
    {
        auto stUserLevelInfoIT = pUserData->m_stLevelInfoTableMap.Find(iLevelID);
        if (stUserLevelInfoIT != pUserData->m_stLevelInfoTableMap.End())
        {
            for(int i = 0; i < CLogicConfigDefine::ESD_LevelStarEvaluateNum; ++i)
            {
                if(stUserLevelInfoIT->second->GetLevelStar() & (1 << i))
                    ++iTotalStar;
            }
        }
    }

    if (iTotalStar < iStarLimit)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(CHAPTER_NOT_ENOUGHT_LEVEL_STAR);
        stRet.second = false;
        return stRet;
    }

    return stRet;
}

const std::vector<CPackGameItem>*
CLogicCacheLevelOperation::GetChapterBonus(const TLogicGameChapterElem* pstChapterConfigElem, int iPosition)
{
    if (pstChapterConfigElem == nullptr) return nullptr;

    if (pstChapterConfigElem->m_stChapterBonus.size() > (size_t)iPosition)
    {
        return (&(pstChapterConfigElem->m_stChapterBonus[iPosition]));
    }

    using namespace SERVER_ERRCODE;
    SET_ERR_INFO(CHAPTER_BONUS_NOT_CONFIGED);
    return nullptr;
}

uint64_t
CLogicCacheLevelOperation::GetChapterBonusBitmap(user_data_table_ptr_type pUserData, int iChapterID, int iPosition)
{
    int iChapterType = CLogicConfig::GetChapterTypeByChapterID(iChapterID);
    uint64_t ulChapterBonus;

    if (CLogicConfigDefine::NORMAL_CHAPTER == iChapterType)
    {
        if (CLogicConfigDefine::NORMAL_FIRST_CHAPTER_BONUS == iPosition)
        {
            ulChapterBonus = pUserData->m_stUserInfoTable.GetNormalChapterBonus1();
        }
        else if (CLogicConfigDefine::NORMAL_SECOND_CHAPTER_BONUS == iPosition)
        {
            ulChapterBonus = pUserData->m_stUserInfoTable.GetNormalChapterBonus2();
        }
        else if (CLogicConfigDefine::NORMAL_THIRD_CHAPTER_BONUS == iPosition)
        {
            ulChapterBonus = pUserData->m_stUserInfoTable.GetNormalChapterBonus3();
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(UNKNOW_CHAPTER_TYPE);
            return (-1);
        }
    }
    else if (CLogicConfigDefine::ELITE_CHAPTER == iChapterType)
    {
        if (CLogicConfigDefine::NORMAL_FIRST_CHAPTER_BONUS == iPosition)
        {
            ulChapterBonus = pUserData->m_stUserInfoTable.GetEliteChapterBonus1();
        }
        else if (CLogicConfigDefine::NORMAL_SECOND_CHAPTER_BONUS == iPosition)
        {
            ulChapterBonus = pUserData->m_stUserInfoTable.GetEliteChapterBonus2();
        }
        else if (CLogicConfigDefine::NORMAL_THIRD_CHAPTER_BONUS == iPosition)
        {
            ulChapterBonus = pUserData->m_stUserInfoTable.GetEliteChapterBonus3();
        }
        else
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(UNKNOW_CHAPTER_TYPE);
            return (-1);
        }
    }
    else
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(UNKNOW_CHAPTER_TYPE);
        return (-1);
    }
    return ulChapterBonus;
}

void
CLogicCacheLevelOperation::SetChapterBonusBitmap(user_data_table_ptr_type pUserData, int iChapterID, int iPosition, uint64_t uiBitmap)
{
    int iChapterType = CLogicConfig::GetChapterTypeByChapterID(iChapterID);
    if (CLogicConfigDefine::NORMAL_CHAPTER == iChapterType)
    {
        if (CLogicConfigDefine::NORMAL_FIRST_CHAPTER_BONUS == iPosition)
        {
            pUserData->m_stUserInfoTable.SetNormalChapterBonus1(uiBitmap);
        }
        else if (CLogicConfigDefine::NORMAL_SECOND_CHAPTER_BONUS == iPosition)
        {
            pUserData->m_stUserInfoTable.SetNormalChapterBonus2(uiBitmap);
        }
        else if (CLogicConfigDefine::NORMAL_THIRD_CHAPTER_BONUS == iPosition)
        {
            pUserData->m_stUserInfoTable.SetNormalChapterBonus3(uiBitmap);
        }
    }
    else if (CLogicConfigDefine::ELITE_CHAPTER == iChapterType)
    {
        if (CLogicConfigDefine::NORMAL_FIRST_CHAPTER_BONUS == iPosition)
        {
            pUserData->m_stUserInfoTable.SetEliteChapterBonus1(uiBitmap);
        }
        else if (CLogicConfigDefine::NORMAL_SECOND_CHAPTER_BONUS == iPosition)
        {
            pUserData->m_stUserInfoTable.SetEliteChapterBonus2(uiBitmap);
        }
        else if (CLogicConfigDefine::NORMAL_THIRD_CHAPTER_BONUS == iPosition)
        {
            pUserData->m_stUserInfoTable.SetEliteChapterBonus3(uiBitmap);
        }
    }
}

bool CLogicCacheLevelOperation::UpdateLevelTimes(TLevelInfoTable* pLevelData, const TLogicLevelElem* pConfig, int iTimes/* = 1*/)
{
    if (iTimes <= 0 || iTimes >= 99)
    {
        SET_ERR_INFO(SERVER_ERRCODE::COUNT_NOT_ENOUGH);
        return false;
    }

    // 消耗体力
    if(!CLogicCacheCommon::ConsumeGameItem(m_pUserData, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_EnergyID,
                                            pConfig->m_iBonusTeamExp * iTimes, m_pUserData->m_iCommand))
    {
        return false;
    }

    if(!pConfig->m_bFirstPassFree || pLevelData->GetTotalVictoryTimes())
    {
        // 更新该副本次数
        if(pConfig->m_iEnterTimes > 0)
        {
            if (LOGIC_CHECK_DAY_REFRESH(pLevelData->GetLastPlayTime()))
            {
                pLevelData->SetSharedTimes(0);
                pLevelData->SetPlayTimes(0);
                pLevelData->SetLastPlayTime(CLogicCommon::GetSec());
            }

            pLevelData->AddPlayTimes(iTimes);
        }

        // 更新共享副本次数
        if(pConfig->m_iSharedLevel > 0)
        {
            auto pTimesData = GetOrCreateLevelInfo(pConfig->m_iSharedLevel);
            if (pTimesData)
            {
                if (LOGIC_CHECK_DAY_REFRESH(pTimesData->GetLastPlayTime()))
                {
                    pTimesData->SetSharedTimes(0);
                    pTimesData->SetPlayTimes(0);
                    pTimesData->SetLastPlayTime(CLogicCommon::GetSec());
                }

                pTimesData->AddSharedTimes(iTimes);
            }
        }
    }

    pLevelData->AddTotalVictoryTimes(iTimes);

    return (true);
}

bool
CLogicCacheLevelOperation::CheckRspLevelDrop(const std::vector<CPackGameItem>& stRspBonus)
{
    size_t uiRspBonusSize = stRspBonus.size();
    size_t uiCacheBonusSize = m_pUserData->m_stCacheFightLevelDropBonus.size();

    if (uiRspBonusSize > uiCacheBonusSize)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(TOO_MUCH_FIGHT_LEVEL_DROP_BONUS);
        return false;
    }

    unsigned char ucDropedBonusFlag[uiCacheBonusSize];
    ::memset(ucDropedBonusFlag, 0, sizeof(ucDropedBonusFlag));

    size_t uiRspIndex, uiCacheIndex;
    for (uiRspIndex = 0; uiRspIndex < uiRspBonusSize; ++uiRspIndex)
    {
        for (uiCacheIndex = 0; uiCacheIndex < uiCacheBonusSize; ++uiCacheIndex)
        {
            if (stRspBonus[uiRspIndex].m_iItemType == m_pUserData->m_stCacheFightLevelDropBonus[uiCacheIndex].m_iItemType &&
                stRspBonus[uiRspIndex].m_iCardID == m_pUserData->m_stCacheFightLevelDropBonus[uiCacheIndex].m_iCardID &&
                stRspBonus[uiRspIndex].m_iNum <= m_pUserData->m_stCacheFightLevelDropBonus[uiCacheIndex].m_iNum &&
                ucDropedBonusFlag[uiCacheIndex] == 0)
            {
                ucDropedBonusFlag[uiCacheIndex] = 1;
                break;
            }
        }
        if (uiCacheIndex == uiCacheBonusSize)
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(FIGHT_LEVEL_DROP_BONUS_NOT_IN_CACHE);
            return false;
        }
    }
    return true;
}

void CLogicCacheLevelOperation::FlushLevelTimes(int iLevelID)
{
    auto stUserLevelInfoPair = GetUserLevelInfo(m_pUserData, iLevelID);
    if (!stUserLevelInfoPair.second) return;
    
    if (LOGIC_CHECK_DAY_REFRESH(stUserLevelInfoPair.first->second->GetLastPlayTime()))
    {
        stUserLevelInfoPair.first->second->SetPlayTimes(0);
        stUserLevelInfoPair.first->second->SetLastPlayTime(CLogicCommon::GetSec());
    }
}

int32_t CLogicCacheLevelOperation::GetOtherLevelReward(int iLevelID, const TLogicLevelElem* pConfig, uint64_t ulPara,int extraLevel ,std::vector<CPackGameItem>& stRewards,std::vector<CPackGameItem>& stExtraRewards)
{
    auto stLevelInfoIT = m_pUserData->m_stLevelInfoTableMap.Find(iLevelID);
    if (stLevelInfoIT == m_pUserData->m_stLevelInfoTableMap.End())
    {
        SET_ERR_INFO(SERVER_ERRCODE::INTERN_ERROR);
        return false;
    }

    // 根据副本类型的特殊掉落
    int32_t iGuildNum = 0;
    switch (pConfig->m_iLevelType)
    {
    case CLogicConfigDefine::ELT_Constel:
    {
        auto pConstelConfig = CLogicConfig::GetConstelLevelDropConfig().GetConstelDropConfig(iLevelID);
        if(pConstelConfig == nullptr)
            return 0;

        int32_t iConstelNum = CLogicCacheCommon::RandStlCont(pConstelConfig->m_stConstelNumRand, pConstelConfig->m_iTotalWeight);

        if (CLogicBuff::HasTargetBuff(m_pUserData,CLogicConfigDefine::EBT_AddLevelTypeExplodeRate,CLogicConfigDefine::ELT_Constel))
        {
            CLogicBuff::ReduceBuffTimes(m_pUserData,CLogicConfigDefine::EBT_AddLevelTypeExplodeRate,CLogicConfigDefine::ELT_Constel);
            CLogicUserNotifySystem::NotifyUserBuffData(m_pUserData);
            iConstelNum *= 2;
        }
        if(iConstelNum > 0)
        {
            CLogicConfig::GetCardbagConfig().GetRandBonusByName(pConstelConfig->m_strCardBag, stRewards, iConstelNum);
        }

        if(m_pUserData->m_stIBusyData.GetConstelAdvTimes() < pConfig->m_iAdvRewardTimes && stLevelInfoIT->second->GetTotalVictoryTimes() != 1)
        {
            int32_t iExtraConstelNum = CLogicCacheCommon::RandStlCont(pConstelConfig->m_stConstelExtraNumRand, pConstelConfig->m_iExtraTotalWeight);
            if(iExtraConstelNum > 0)
            {
                CLogicConfig::GetCardbagConfig().GetRandBonusByName(pConstelConfig->m_strCardBag, stExtraRewards, iExtraConstelNum);
            }
            m_pUserData->m_stIBusyData.AddConstelAdvTimes(1);
        }
        break;
    }
    case CLogicConfigDefine::ELT_EquipLevel:
    {
        //装备本要判断当前是高额奖励还是普通奖励
        auto pEquipConfig = CLogicConfig::GetConstelLevelDropConfig().GetEquipDropConfig(iLevelID);
        if(pEquipConfig == nullptr)
            return 0;

        auto* pTimesConfig = CLogicConfig::GetLevelConfig().GetLevelConfigElem(pConfig->m_iSharedLevel);
        if(nullptr == pTimesConfig)
        {
            SET_ERR_INFO(SERVER_ERRCODE::INTERN_ERROR);
            return false;
        }

        auto pTimesData = GetOrCreateLevelInfo(pConfig->m_iSharedLevel);
        if (pTimesData)
        {
            auto iCurAdvTime = pTimesData->GetAdvRewardTimes();
            // 月卡次数
            int iAddTimes = 0;
            if(CLogicBuff::IsHaveMonthCard(m_pUserData))
            {
                iAddTimes = CLogicConfig::GetAwardConfig().GetMonthCardAdvLevelTimes(pConfig->m_iLevelType);
            }

            //走高级掉落
            if (iCurAdvTime < pTimesConfig->m_iAdvRewardTimes + iAddTimes && stLevelInfoIT->second->GetTotalVictoryTimes() != 1)
            {
                for(auto& bag_name : pEquipConfig->m_stAdvCardBagVec)
                {
                    CLogicConfig::GetCardbagConfig().GetRandBonusByName(bag_name, stRewards);
                }

                for(auto& bag_name : pEquipConfig->m_stExtraCardBagVec)
                {
                    CLogicConfig::GetCardbagConfig().GetRandBonusByName(bag_name, stExtraRewards);
                }

                pTimesData->AddAdvRewardTimes( 1);
            } else//普通掉落
            {
                for(auto& bag_name : pEquipConfig->m_stCardBagVec)
                {
                    CLogicConfig::GetCardbagConfig().GetRandBonusByName(bag_name, stRewards);
                }
            }

            if (LOGIC_CHECK_DAY_REFRESH(pTimesData->GetLastPlayTime()))
            {
                pTimesData->SetSharedTimes(0);
                pTimesData->SetPlayTimes(0);
                pTimesData->SetLastPlayTime(CLogicCommon::GetSec());
                pTimesData->SetAdvRewardTimes(0);
            }
        }
        break;
    }
    case CLogicConfigDefine::ELT_GuildBossLevel:
    {
        auto& stConfig = CLogicConfig::GetGuildConfig();
        // 首通奖励
        const uint64_t ulAwardState = m_pUserData->m_stIBusyData.GetOnceEventStat();
        const uint64_t ulFlag = 1ULL << CLogicConfigDefine::EURST_GuildBossFirstAward;
        if(!(ulAwardState & ulFlag))
        {
            stRewards.insert(stRewards.end(), stConfig.m_stBossFirstAwards.begin(), stConfig.m_stBossFirstAwards.end());
            m_pUserData->m_stIBusyData.SetOnceEventStat(ulAwardState | ulFlag);
        }

        auto pRewardConfig = stConfig.GetBossPersonReward(CLogicCommon::GetDate().tm_wday, ulPara, m_pUserData->m_stUserInfoTable.GetLevel());
        if(pRewardConfig)
        {
            if(!pRewardConfig->m_strCardBag.empty())
                CLogicConfig::GetCardbagConfig().GetRandBonusByName(pRewardConfig->m_strCardBag, stRewards);

            for(auto& item : pRewardConfig->m_stRewards)
            {
                stRewards.emplace_back(item.m_iType, item.m_iID, CLogicCommon::GetRandNum(item.m_iNumMin, item.m_iNumMax, true));
            }
        }
        //添加公会Boss的念写卡奖励
        auto pConstelConfig = CLogicConfig::GetConstelLevelDropConfig().GetConstelDropConfig(extraLevel);
        if(pConstelConfig == nullptr)
            return 0;

        if (pRewardConfig)
        {
            iGuildNum = CLogicCacheCommon::RandStlCont(pConstelConfig->m_stConstelNumRand, pConstelConfig->m_iTotalWeight) * pRewardConfig->m_iRewardTimes;
        }

        if(iGuildNum > 0)
        {
            CLogicConfig::GetCardbagConfig().GetRandBonusByName(pConstelConfig->m_strCardBag, stRewards, iGuildNum);
        }

        break;
    }
    case CLogicConfigDefine::ELT_ExpLevel:
    {
        //检查是否含有地区buff
        auto userAtlasBuff = m_pUserData->m_stIBusyData.GetAtlasBuffInfo();
        auto iterAtlasBuff = userAtlasBuff.find(EnumCompleteAtlasBuffType::AtlasBuff_Exp);
        auto atlasConfig = CLogicConfig::GetCompleteAtlasConfig().m_mapBuffID2Config;

        if (iterAtlasBuff != userAtlasBuff.end() && iterAtlasBuff->second != 0)
        {
            auto targetAtlasConfig = atlasConfig.find(EnumCompleteAtlasBuffType::AtlasBuff_Exp);
            if (atlasConfig.end() != targetAtlasConfig)
            {
                float fLevelParam = targetAtlasConfig->second.m_vecBuffParam[iterAtlasBuff->second];
                for(auto& item : stRewards)
                {
                    item.m_iNum = (int32_t) ((float)item.m_iNum * (1 + fLevelParam/ 10000));
                }
            }
        }
    }
    case CLogicConfigDefine::ELT_GoldLevel:
    {
        //检查是否含有地区buff
        auto userAtlasBuff = m_pUserData->m_stIBusyData.GetAtlasBuffInfo();
        auto iterAtlasBuff = userAtlasBuff.find(EnumCompleteAtlasBuffType::AtlasBuff_Gold);
        auto atlasConfig = CLogicConfig::GetCompleteAtlasConfig().m_mapBuffID2Config;
        if (iterAtlasBuff != userAtlasBuff.end() && iterAtlasBuff->second != 0)
        {
            auto targetAtlasConfig = atlasConfig.find(EnumCompleteAtlasBuffType::AtlasBuff_Gold);
            if (atlasConfig.end() != targetAtlasConfig)
            {
                float fLevelParam = targetAtlasConfig->second.m_vecBuffParam[iterAtlasBuff->second];
                for(auto& item : stRewards)
                {
                    item.m_iNum = (int32_t) ((float)item.m_iNum * (1 + fLevelParam/ 10000));
                }
            }
        }
    }
    default:
        break;
    }


    return iGuildNum;
}

void CLogicCacheLevelOperation::CalcExtraLevelReward(int iLevelID, const TLogicLevelElem* pConfig,std::vector<CPackGameItem>& stExtraReward,CPackTotalItemData* totalItemData)
{

    switch (pConfig->m_iLevelType)
    {
    case CLogicConfigDefine::ELT_Constel:
    {
        for (auto constel : stExtraReward)
        {
            auto constelConfig = CLogicConfig::GetConstelConfig().GetConfig(constel.m_iCardID);
            if (nullptr != constelConfig)
            {
                CPackConstellationInfo info;
                info.m_iGrade = constelConfig->m_iGrade;
                totalItemData->m_stBonus.m_stConstelVec.emplace_back(info);
            }
            else
            {
                SET_ERR_INFO(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
            }
        }
        break;
    }
    case CLogicConfigDefine::ELT_EquipLevel:
    {
        for (auto equipment : stExtraReward)
        {
            auto pEquipConfig = CLogicConfig::GetEquipItemConfig().GetConfig(equipment.m_iCardID);
            if (nullptr != pEquipConfig)
            {
                CPackEquipInfo info;
                info.m_iGrade = pEquipConfig->m_iGrade;
                totalItemData->m_stBonus.m_stEquipVec.emplace_back(info);
            }
            else
            {
                SET_ERR_INFO(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
            }
        }
        break;
    }
    }
}


bool CLogicCacheLevelOperation::CalcGoldCollectingRate(const TLogicLevelElem* pConfig, const CPackLevelFightPara& stParam, const TLogicEvaluateElem& stEvaluate)
{
    if(pConfig->m_iMaxCoinNum <= 0) return false;
    // check coin
    if (stParam.m_iGetCoinNum > pConfig->m_iMaxCoinNum)
    {
        return false;
    }

    // check box
    if (stParam.m_iGetBoxNum > pConfig->m_stBarrierMapDetail.m_iBoxNum)
    {
        return false;
    }

    int iRate = float(stParam.m_iGetCoinNum) / pConfig->m_iMaxCoinNum * 100;
    if (iRate >= stEvaluate.m_iAmount)
        return true;

    return false;
}

bool CLogicCacheLevelOperation::CalcMonsterKillRate(const TLogicLevelElem* pConfig, const CPackLevelFightPara& stParam, const TLogicEvaluateElem& stEvaluate)
{
    if(pConfig->m_stBarrierMapDetail.m_iMaxMonsterNum <= 0) return false;

    int iTotalKilledMonster = 0;

    auto stKilledMonsterITBeg = stParam.m_stKillMonsterMap.begin();
    auto stKilledMonsterITEnd = stParam.m_stKillMonsterMap.end();
    for ( ; stKilledMonsterITBeg != stKilledMonsterITEnd; ++stKilledMonsterITBeg)
    {
        // bad code, for lua msgpack bug
        if ((stKilledMonsterITBeg->first).compare("default") == 0)
            continue;

        auto stConfigMonsterIT = pConfig->m_stBarrierMapDetail.m_stMonsterMap.find(stKilledMonsterITBeg->first);
        if (stConfigMonsterIT != pConfig->m_stBarrierMapDetail.m_stMonsterMap.end() && stConfigMonsterIT->second >= stKilledMonsterITBeg->second)
        {
            iTotalKilledMonster += stKilledMonsterITBeg->second;
            continue;
        }

        return false;
    }

    int iRate = float(iTotalKilledMonster) / pConfig->m_stBarrierMapDetail.m_iMaxMonsterNum * 100;
    if (iRate >= stEvaluate.m_iAmount)
        return true;

    return false;
}

bool CLogicCacheLevelOperation::CalcDeadRoleNum(const TLogicLevelElem* pConfig, const CPackLevelFightPara& stParam, const TLogicEvaluateElem& stEvaluate)
{
    if(stParam.m_stCardStatus.size() > CLogicConfigDefine::ESD_TeamMaxCardNum)
        return false;

    int iDeadCard = 0;
    for(auto& iter : stParam.m_stCardStatus)
    {
        if(iter.first <= 0) continue;

        auto stCardInfoIT = m_pUserData->m_stHeroCardTableMap.Find(iter.first);
        if (stCardInfoIT != m_pUserData->m_stHeroCardTableMap.End())
        {
            if (iter.second <= 0) ++iDeadCard;
        }
        else
        {
            return false;
        }
    }

    if (iDeadCard < stEvaluate.m_iAmount)
        return true;

    return false;
}

bool CLogicCacheLevelOperation::CalcTeamRemainHPRate(const TLogicLevelElem* pConfig, const CPackLevelFightPara& stParam, const TLogicEvaluateElem& stEvaluate)
{
    auto arrCardID = CLogicCacheCommon::GetTeam(m_pUserData, pConfig->m_iTeamType);

    int iMaxTotalHP = 0, iUserTotalHP = 0;
    for(auto card_id : arrCardID)
    {
        if(card_id <= 0) continue;

        auto iterPara = stParam.m_stCardStatus.find(card_id);
        if(iterPara == stParam.m_stCardStatus.end())
            return false;

        auto stCardInfoIT = m_pUserData->m_stHeroCardTableMap.Find(card_id);
        if (stCardInfoIT == m_pUserData->m_stHeroCardTableMap.End())
            return false;

        int iTotalHP = stCardInfoIT->second->GetFightHp();
        if (iterPara->second > iTotalHP)
        {
            return false;
        }

        iMaxTotalHP += iTotalHP;
        iUserTotalHP += iterPara->second;
    }

    if(iMaxTotalHP <= 0) return false;

    int iRate = (float)iUserTotalHP / iMaxTotalHP * 100;
    if (iRate > stEvaluate.m_iAmount)
        return true;

    return false;
}

bool CLogicCacheLevelOperation::CalcBOSSTimeLimit(const TLogicLevelElem* pConfig, const CPackLevelFightPara& stParam, const TLogicEvaluateElem& stEvaluate)
{
    if (stParam.m_iBossCostTime <= stEvaluate.m_iAmount)
        return true;

    return false;
}

bool CLogicCacheLevelOperation::CalcGreaterAmount(const TLogicLevelElem* pConfig, const CPackLevelFightPara& stParam, const TLogicEvaluateElem& stEvaluate)
{
    return stParam.m_ulPara >= (uint64_t)stEvaluate.m_iAmount;
}

bool CLogicCacheLevelOperation::IsRefreshLimitBoss(int levelID)
{
    auto& limitBossConfig = CLogicConfig::GetLimitBossConfig();
    if (!CLogicActive::IsLimitBossLevelOpen(m_pUserData))
    {
        return false;
    }

    auto iterLevelRefresh = CLogicConfig::GetLimitBossConfig().m_mapLeveID2Weight.find(levelID);
    if(iterLevelRefresh == CLogicConfig::GetLimitBossConfig().m_mapLeveID2Weight.end())
    {
        return false;
    }

    bool isRefreshLimitBoss = false;
    int32_t refreshWeight = limitBossConfig.GetLevelRefreshWeight(levelID);
    int iRandNum = CLogicCommon::GetRandNum() % CLogicConfigDefine::ESD_RandomWeight;
    if (iRandNum < refreshWeight)
    {
        isRefreshLimitBoss = true;
    }

    if (!isRefreshLimitBoss)
    {
        //如果保底次数到了直接刷出来
        auto baodiTimes = m_pUserData->m_stIBusyData.GetLimitBossBaodiTime();
        if (baodiTimes >= CLogicConfig::GetLimitBossConfig().m_iBaodiTime)
        {
            //清空保底
            m_pUserData->m_stIBusyData.SetLimitBossBaodiTime(0);
            isRefreshLimitBoss = true;
        } else
        {
            m_pUserData->m_stIBusyData.AddLimitBossBaodiTime(1);
        }
    } else
    {
        m_pUserData->m_stIBusyData.SetLimitBossBaodiTime(0);
    }
    return isRefreshLimitBoss;
}


void CLogicCacheLevelOperation::InitLimitBoss(CPackNewLimitBossInfo* bossInfo)
{
    bossInfo->m_iOwnerUin = m_pUserData->m_iUin;
    bossInfo->m_iOwnerGroupID = m_pUserData->m_iGroupID;

    int32_t iTotalWeight = 0;
    int32_t iCurrWeight = 0;
    auto& limitBossInfo = CLogicConfig::GetLimitBossConfig().m_mapLevelID2Boss;
    for (const auto& iterLimitBoss: limitBossInfo)
    {
        iTotalWeight += iterLimitBoss.second.m_iWeight;
    }

    if (iTotalWeight != 0) {
        int32_t iRandNum = CLogicCommon::GetRandNum() % iTotalWeight;
        for (const auto& iterLimitBoss: limitBossInfo)
        {
            iCurrWeight += iterLimitBoss.second.m_iWeight;
            if (iRandNum < iCurrWeight)
            {
                bossInfo->m_iLevelID = iterLimitBoss.second.m_iLevelID;
                bossInfo->m_ulMaxHp = iterLimitBoss.second.m_iTotalHP;
                break;
            }
        }
    }

    bossInfo->m_iLimitTime = CLogicConfig::GetLimitBossConfig().m_iLimitTime;
}













