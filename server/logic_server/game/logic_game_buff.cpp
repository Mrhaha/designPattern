//
// Created by lxhbroser on 2020/4/1.
//

#include "logic_game_buff.h"
#include "logic_config.h"
#include "logic_link_common.h"
#include "server_errcode.h"
#include "logic_game_user_notify_system.h"

int32_t CLogicBuff::AddBuff(user_data_table_ptr_type pUserData, int iBuffID)
{
    UpdateBuff(pUserData);

    auto pConfig = CLogicConfig::GetBuffConfig().GetConfig(iBuffID);
    if(nullptr == pConfig)
        return SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST;

    auto pEffConfig = CLogicConfig::GetBuffConfig().GetEffectConfig(pConfig->m_iBuffEffectIndex);
    if(nullptr == pEffConfig)
        return SERVER_ERRCODE::INTERN_ERROR;

    // 时间换算成秒
    int iDurTime = pConfig->m_iDurTime;
    if(pConfig->m_iDurDays > 0)
    {
        iDurTime = 1_day * pConfig->m_iDurDays;
    }

    auto iterData = pUserData->m_stUserBuffMap.Find(pConfig->m_iBuffEffectIndex);
    if(iterData != pUserData->m_stUserBuffMap.End())
    {
        // 判断同效果组的互斥
        switch (pEffConfig->m_iMutexType)
        {
        case CLogicConfigBuffParser::EMT_Mutex:
            return SERVER_ERRCODE::BUFF_ERR_MUTEX;

        case CLogicConfigBuffParser::EMT_Replace:
            iterData->second->SetBuffType(pEffConfig->m_iBuffType);
            iterData->second->SetBuffPara(pEffConfig->m_iBuffPara);
            iterData->second->SetBuffParaEx(pEffConfig->m_iBuffParaEx);
            iterData->second->SetEndTime(CLogicCommon::GetSec() + iDurTime);
            iterData->second->SetEffectTimes(pConfig->m_iEffectTimes);
            break;

        case CLogicConfigBuffParser::EMT_Superpose:
            iterData->second->AddEndTime(iDurTime);
            iterData->second->AddEffectTimes(pConfig->m_iEffectTimes);
            break;

        default:
            return SERVER_ERRCODE::INTERN_ERROR;
        }
    }
    else
    {
        // 没有相同效果的BUFF，检查其他效果互斥，替代情况
        // 检查是否已有互斥BUFF
        for(auto& index : pEffConfig->m_stMutexVec)
        {
            if(pUserData->m_stUserBuffMap.Find(index) != pUserData->m_stUserBuffMap.End())
                return SERVER_ERRCODE::BUFF_ERR_MUTEX;
        }

        // 加入新BUFF
        TUserBuffTableValueType stValue;
        stValue.m_iUid = pUserData->m_iUin;
        stValue.m_iGroupID = pUserData->m_iGroupID;
        stValue.m_iEffectIndex = pConfig->m_iBuffEffectIndex;
        stValue.m_iBuffType = pEffConfig->m_iBuffType;
        stValue.m_iBuffPara = pEffConfig->m_iBuffPara;
        stValue.m_iBuffParaEx = pEffConfig->m_iBuffParaEx;
        stValue.m_iEndTime = CLogicCommon::GetSec() + iDurTime;
        stValue.m_iEffectTimes = pConfig->m_iEffectTimes;

        // 持续天数BUFF时间修正
        if(pConfig->m_iDurDays > 0)
        {
            struct tm stNowDate = CLogicCommon::GetDate();
            if (stNowDate.tm_hour < 5)
            {
                stValue.m_iEndTime = get_day_begin_time(stValue.m_iEndTime) - 1 + 5_hour - 1_day;
            }
            else
            {
                stValue.m_iEndTime = get_day_begin_time(stValue.m_iEndTime) - 1 + 5_hour;
            }
        }

        auto stRet = pUserData->m_stUserBuffMap.Insert(std::make_pair(stValue.m_iEffectIndex, stValue));
        if(stRet.second == false)
            return SERVER_ERRCODE::INTERN_ERROR;

        // 顶替掉互替效果的BUFF
        for(auto& index : pEffConfig->m_stReplaceVec)
        {
            pUserData->m_stUserBuffMap.Erase(index);
        }
    }

    // 通知
    CLogicUserNotifySystem::NotifyUserBuffData(pUserData);

    return SERVER_ERRCODE::SUCCESS;
}

bool CLogicBuff::IsHaveMonthCard(user_data_table_ptr_type pUserData)
{
    auto iter = pUserData->m_stUserBuffMap.Find(CLogicConfig::GetBuffConfig().m_iMonthCardIndex);
    return (iter != pUserData->m_stUserBuffMap.End() && iter->second->GetEndTime() >= CLogicCommon::GetSec());
}


bool CLogicBuff::IsHaveFatMonthCard(user_data_table_ptr_type pUserData)
{
    auto iter = pUserData->m_stUserBuffMap.Find(CLogicConfig::GetBuffConfig().m_iFatMonthCardIndex);
    return (iter != pUserData->m_stUserBuffMap.End() && iter->second->GetEndTime() >= CLogicCommon::GetSec());
}

int32_t CLogicBuff::GetBuffValue(user_data_table_ptr_type pUserData, int iBuffType, int iPara)
{
    int32_t iValue = 0;
    for(auto iter = pUserData->m_stUserBuffMap.Begin(); iter != pUserData->m_stUserBuffMap.End(); ++iter)
    {
        if(iter->second->GetEndTime() < CLogicCommon::GetSec())
            continue;

        if(iter->second->GetBuffType() == iBuffType && iter->second->GetBuffPara() == iPara)
            iValue += iter->second->GetBuffParaEx();
    }

    return iValue;
}



void CLogicBuff::UpdateBuff(user_data_table_ptr_type pUserData)
{
    for(auto iterBuff = pUserData->m_stUserBuffMap.Begin(); iterBuff != pUserData->m_stUserBuffMap.End(); )
    {
        if((iterBuff->second->GetEndTime() <= CLogicCommon::GetSec() && iterBuff->second->GetEffectTimes() == 0) ||
            (iterBuff->second->GetEndTime() == 0 && iterBuff->second->GetEndTime() <= 0))
            pUserData->m_stUserBuffMap.Erase(iterBuff++);
        else
            ++iterBuff;
    }
}

bool CLogicBuff::HasTargetBuff(user_data_table_ptr_type pUserData,int32_t buffType,int32_t buffParam)
{
    bool ret = false;
    for(auto iterBuff = pUserData->m_stUserBuffMap.Begin(); iterBuff != pUserData->m_stUserBuffMap.End(); iterBuff++)
    {
        if (iterBuff->second->GetBuffType() != buffType)
            continue ;
        if (iterBuff->second->GetEffectTimes() == 0)
            continue ;
        if (buffParam == 0 || iterBuff->second->GetBuffPara() == buffParam)
        {
            ret = true;
        }
    }
    return ret;
}


void CLogicBuff::ReduceBuffTimes(user_data_table_ptr_type pUserData,int32_t buffType,int32_t buffParam)
{
    for(auto iterBuff = pUserData->m_stUserBuffMap.Begin(); iterBuff != pUserData->m_stUserBuffMap.End(); iterBuff++)
    {
        if (iterBuff->second->GetBuffType() != buffType)
            continue ;
        if (iterBuff->second->GetEffectTimes() == 0)
            continue ;
        if (buffParam == 0 || iterBuff->second->GetBuffPara() == buffParam)
        {
            if(iterBuff->second->GetEffectTimes() == 0) continue;
            iterBuff->second->SetEffectTimes(iterBuff->second->GetEffectTimes() - 1);
        }
    }
}

////////////////////////////////////////////////// CLogicBadgeManage ///////////////////////////////////////////////////
void CLogicBadgeManage::UpdateBadgeCond(user_data_table_ptr_type pUserData, int iType, int iPara, const CPackLevelFightPara& stParam)
{
    auto iterConfig = CLogicConfig::GetBadgeConfig().m_stCondTypeMap.lower_bound(iType);
    auto iterConfigEnd = CLogicConfig::GetBadgeConfig().m_stCondTypeMap.upper_bound(iType);
    for(; iterConfig != iterConfigEnd; ++iterConfig)
    {
        auto iterData = pUserData->m_stUserBadgeMap.Find(iterConfig->second->m_iBadgeID);
        if(iterData == pUserData->m_stUserBadgeMap.End())
        {
            TUserBadgeTableValueType stValue;
            stValue.m_iUid = pUserData->m_iUin;
            stValue.m_iGroupID = pUserData->m_iGroupID;
            stValue.m_iBadgeID = iterConfig->second->m_iBadgeID;
            stValue.m_cIsOwned = false;
            stValue.m_iProgress = 0;
            auto stRet = pUserData->m_stUserBadgeMap.Insert(std::make_pair(stValue.m_iBadgeID, stValue));
            if(!stRet.second)
            {
                LOGIC_LOG_TRACE_ERROR << "UserBadgeMap Insert Error!|UIN:" << pUserData->m_iUin << "|GROUP_ID:"
                                      << pUserData->m_iGroupID << "|BADGE_ID:" << stValue.m_iBadgeID << std::endl;
                continue;
            }

            iterData = stRet.first;
        }

        if(iterData->second->GetIsOwned())
            continue;

        bool bNotify = false;
        switch (iType)
        {
        case CLogicConfigDefine::EBCT_LevelFight:
            bNotify = CheckLevelFightCond(pUserData, iterData->second, *(iterConfig->second), iPara, stParam);
            break;
        case CLogicConfigDefine::EBCT_CollectFish:
        {
            auto stSet = iterData->second->GetProSet();
            if(stSet.find(iPara) == stSet.end())
            {
                stSet.insert(iPara);
                iterData->second->SetProSet(stSet);
                if(int(stSet.size()) >= iterConfig->second->m_iPara)
                    iterData->second->SetIsOwned(true);

                bNotify = true;
            }

            break;
        }
        case CLogicConfigDefine::EBCT_CookBookTimes:
        case CLogicConfigDefine::EBCT_LikeFriend:
        case CLogicConfigDefine::EBCT_BeLike:
        case CLogicConfigDefine::EBCT_CompletSpecExplore:
        case CLogicConfigDefine::EBCT_CollectRuneCard:
        case CLogicConfigDefine::EBCT_CollectSpecRuneCard:
        case CLogicConfigDefine::EBCT_CollectGold:
        {
            iterData->second->AddProgress(iPara);
            if(iterData->second->GetProgress() >= iterConfig->second->m_iPara)
                iterData->second->SetIsOwned(true);

            bNotify = true;

            break;
        }
        default:
            break;
        }

        if(iterData->second->GetIsOwned()) iterData->second->SetOwnedTime(CLogicCommon::GetSec());

        if(bNotify) pUserData->m_stOnlyInCacheData.m_stCacheBadgeIDSet.insert(iterData->first);
    }
}

bool CLogicBadgeManage::CheckLevelFightCond(user_data_table_ptr_type pUserData, TUserBadgeTable* pTable, const TLogicBadgeCondElem& stConfig, int iLevelID, const CPackLevelFightPara& stParam)
{
    bool bSave = false;
    auto stProSet = pTable->GetProSet();
    std::size_t iTarget = 0;

    if(stConfig.m_stParaSet.empty())
    {
        if(iLevelID != stConfig.m_iPara) return false;
    }
    else
    {
        if(stConfig.m_stParaSet.find(iLevelID) == stConfig.m_stParaSet.end())
            return false;

        if(stConfig.m_bIsAll)
        {
            // 该副本已达成
            if(stProSet.find(iLevelID) != stProSet.end()) return false;

            stProSet.insert(iLevelID);
            bSave = true;
            iTarget = stConfig.m_stParaSet.size();
        }
    }

    // 检查携带伙伴
    for(auto& card_id : stConfig.m_stTeamCard)
    {
        if(stParam.m_stCardStatus.find(card_id) == stParam.m_stCardStatus.end())
            return false;
    }

    // 检查携带BUFF
    if(stConfig.m_iBuffID > 0 && pUserData->m_stUserBuffMap.Find(stConfig.m_iBuffID) == pUserData->m_stUserBuffMap.End())
        return false;

    // 不被BOSS击中
    if(!stConfig.m_strMissBoss.empty())
    {
        auto iterBoss = stParam.m_stKillBossMap.find(stConfig.m_strMissBoss);
        if(iterBoss == stParam.m_stKillBossMap.end() || !iterBoss->second)
            return false;
    }

    // 击杀BOSS
    if(!stConfig.m_strKillBoss.empty() && stParam.m_stKillBossMap.find(stConfig.m_strKillBoss) == stParam.m_stKillBossMap.end())
        return false;

    // 吃到所有金币
    if(stConfig.m_bAllGold)
    {
        if(stParam.m_stAchieveTagVec.size() < CLogicConfigDefine::ELAT_AllGold || !stParam.m_stAchieveTagVec[CLogicConfigDefine::ELAT_AllGold - 1])
            return false;
    }

    // 没有伙伴阵亡
    if(stConfig.m_bNoDeath)
    {
        for(auto& iter : stParam.m_stCardStatus)
        {
            if(iter.first > 0 && iter.second <= 0)
                return false;
        }
    }

    // 不携带某种类型伙伴
    if(stConfig.m_iNoCardType > 0)
    {
        for(auto& iter : stParam.m_stCardStatus)
        {
            if(iter.first <= 0)
                continue;

            auto pCardConfig = CLogicConfig::GetHeroCardConfig().GetConfig(iter.first);
            if(pCardConfig == nullptr || pCardConfig->m_iCardType == stConfig.m_iNoCardType)
                return false;
        }
    }

    if(bSave) pTable->SetProSet(stProSet);
    if(stProSet.size() >= iTarget) pTable->SetIsOwned(true);

    return true;
}
