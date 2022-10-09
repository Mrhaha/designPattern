//
// Created by hzl on 2019/6/6.
//

#include "common_datetime.h"
#include "logic_link_common.h"
#include "logic_game_global.h"
#include "logic_config.h"
#include "logic_game_cache_common.h"
#include "logic_game_day_refresh_system.h"
#include "logic_manager_limit_boss.h"

std::pair<std::time_t, std::time_t> CLogicGlobal::m_stEntireWarAvyTime = std::make_pair(0, 0);

/************************************************************************************************************/
/******************************************全  民  战  役*****************************************************/
/************************************************************************************************************/
int32_t CLogicGlobal::GetEntireWarThemeID()
{
    return GetGlobalCache()->m_stBasicTable.GetEntireWarThemeID();
}

int32_t CLogicGlobal::GetEntireWarLevel()
{
    return GetGlobalCache()->m_stBasicTable.GetEntireWarLevel();
}

CPackUserKey CLogicGlobal::GetEntireWarGodKey()
{
    CPackUserKey key;
    key.m_iUin = GetGlobalCache()->m_stBasicTable.GetEntireWarGodUid();
    key.m_iGroupID = GetGlobalCache()->m_stBasicTable.GetEntireWarGodGroupID();
    return key;
}

const std::vector<CPackEntireWarBuff>& CLogicGlobal::GetEntireWarBuff()
{
    return GetGlobalCache()->m_stBasicTable.GetEntireWarBuff().m_stBuffVec;
}

void CLogicGlobal::AddNewEntireWarBuff(int32_t iBuffType, int32_t iBuffPercentage, int32_t iBuffTime)
{
    CPackEntireWarBuff stBuff;
    stBuff.m_iType = iBuffType;
    stBuff.m_iPercentage = iBuffPercentage;
    stBuff.m_iEndTime = CLogicCommon::GetSec() + iBuffTime;

    auto buff = GetGlobalCache()->m_stBasicTable.GetEntireWarBuff();
    buff.m_stBuffVec.push_back(stBuff);
    GetGlobalCache()->m_stBasicTable.SetEntireWarBuff(buff);
}

bool CLogicGlobal::RefreshEntireWarNodeProgress(int32_t iWarID, uint64_t ulProgress, bool isSet/* = false*/)
{
    auto* pstWarTable = GetEntireWarMapTable(iWarID);
    if (pstWarTable)
    {
        if(pstWarTable->GetIsPassed())
            return false;

        if(isSet)
            pstWarTable->SetCurProgress(ulProgress);
        else
            pstWarTable->AddCurProgress(ulProgress);

        if(pstWarTable->GetCurProgress() >= pstWarTable->GetTargetProgress())
        {
            pstWarTable->SetIsPassed(1);
            return true;
        }
    }

    return false;
}

bool CLogicGlobal::IsEntireWarPass(int32_t iWarID)
{
    if(iWarID <= 0) return true;

    const auto* pstWarValue = GetEntireWarMapTable(iWarID);
    return (pstWarValue && pstWarValue->GetIsPassed());
}

void CLogicGlobal::RefreshEntireWarActivityTime()
{
    const std::time_t this_time = CLogicCommon::GetSec();
    const std::time_t monday_time = get_monday_time(this_time);
    const std::time_t open_svr_time = CLogicConfig::GetSystemConfig().GetOpenSvrTime();

    const int32_t diff_day = day_diff(open_svr_time, monday_time);
    if (diff_day < 4)
    {//本周周一前开服不足4天，活动不开启
        m_stEntireWarAvyTime = make_pair(0, 0);
        return;
    }

    if (diff_day < 8)
    {//本周周一前开服4-8天，活动开启一半
        const std::time_t start_time = get_day_begin_time(open_svr_time) + 7_day;
        const std::time_t end_time = get_day_begin_time(monday_time) + 7_day;
        m_stEntireWarAvyTime = make_pair(start_time, end_time);
        return;
    }

    const std::time_t start_time = get_day_begin_time(monday_time);
    const std::time_t end_time = start_time + 7_day;
    m_stEntireWarAvyTime = make_pair(start_time, end_time);
}

void CLogicGlobal::BuildEntireWarExtraData(int32_t iWarID)
{
    const auto* pstWarConfig = CLogicConfig::GetEntireWarMapConfig().GetConfig(GetEntireWarThemeID(), iWarID);
    if(nullptr == pstWarConfig) return;

    if(!GetGlobalCache()->m_stEntireWarExtra[iWarID].Empty()) return;

    if (pstWarConfig->m_iNodeType == CLogicConfigDefine::EEWNT_Market)
    {
        BuildEntireWarMarketExtra(iWarID);
    }
    if (pstWarConfig->m_iNodeType == CLogicConfigDefine::EEWNT_Treasure)
    {
        BuildEntireWarTreasureExtra(iWarID);
    }
}

std::vector<CPackMarket> CLogicGlobal::GetEntireWarMarketPack(int32_t iWarID)
{
    BuildEntireWarExtraData(iWarID);

    std::vector<CPackMarket> stMarketVec;
    auto iterWar = GetGlobalCache()->m_stEntireWarExtra.find(iWarID);
    if (iterWar != GetGlobalCache()->m_stEntireWarExtra.end())
    {
        for (auto iterExtra = iterWar->second.Begin(); iterExtra != iterWar->second.End(); ++iterExtra)
        {
            CPackMarket stElem;
            stElem.m_iDiscount = iterExtra->second->GetDiscount();
            stElem.m_bCanBuy = false;
            stElem.m_stCommodity = { iterExtra->second->GetCommodityItemType(), iterExtra->second->GetCommodityItemID(), iterExtra->second->GetCommodityItemNum() };
            stElem.m_stConsume = { iterExtra->second->GetConsumeItemType(), iterExtra->second->GetConsumeItemID(), iterExtra->second->GetConsumeItemNum() };
            stMarketVec.push_back(stElem);
        }
    }
    return stMarketVec;
}

const TGlobalEntireMapTableValueType* CLogicGlobal::GetEntireWarMapInfo(int32_t iWarID)
{
    const auto* pstWarTable = GetEntireWarMapTable(iWarID);
    return pstWarTable ? &pstWarTable->m_stData : nullptr;
}

const TGlobalEntireExtraTableValueType* CLogicGlobal::GetEntireWarExtraInfo(int32_t iWarID, int32_t iExtraID)
{
    const auto* pstExtraTable = GetEntireWarExtraTable(iWarID, iExtraID);
    return pstExtraTable ? &pstExtraTable->m_stData : nullptr;
}

void CLogicGlobal::RefreshEntireWar()
{
    RefreshWorldEntireWar();

    RefreshEntireWarGodData();
}

void CLogicGlobal::RefreshWorldEntireWar()
{
    const time_t this_time = CLogicCommon::GetSec();

    const auto& rstEntireWarTable = GetGlobalCache()->m_stBasicTable;
    time_t last_time = rstEntireWarTable.GetEntireLastUpdateTime();
    if (this_time >= m_stEntireWarAvyTime.first && this_time < m_stEntireWarAvyTime.second)
    {
        if (last_time < m_stEntireWarAvyTime.first || last_time >= m_stEntireWarAvyTime.second)
        {
            const auto& entire_config = CLogicConfig::GetEntireWarConfig();

            AwardEntireWarRankList();
            BuildNewEntireWar();
            last_time = rstEntireWarTable.GetEntireLastUpdateTime();

            CLogicGlobal::AddGlobalMail(CLogicCommon::GetSec(), m_stEntireWarAvyTime.second, 0, entire_config.m_strStartMailSender, entire_config.m_strStartMailTitle, entire_config.m_strStartMailContent, {});

            //先清空日志
            CLogicGlobal::ClearGlobalFightReport(CLogicConfigDefine::FIGHT_REPORT_ENTIRE_WAR);
            //添加全民战役日志
            const auto pstWarGod = CLogicCacheCommon::LoadUserData(rstEntireWarTable.GetEntireWarGodGroupID(), rstEntireWarTable.GetEntireWarGodUid());
            if (nullptr != pstWarGod)
            {
                for (auto reportIter = CLogicConfig::GetFightReportConfig().m_stConfigMap.lower_bound("entirewar"); reportIter != CLogicConfig::GetFightReportConfig().m_stConfigMap.upper_bound("entirewar"); ++reportIter)
                {
                    if (reportIter->second.m_iTriggerValue == CLogicConfigDefine::ENTIRE_WAR_FIGHT_REPORT_GOD_REPORT_TRIGGER_VALUE)
                    {
                        CPackFightReportItem reportItem;
                        reportItem.m_iReportType = CLogicConfigDefine::FIGHT_REPORT_ENTIRE_WAR;
                        reportItem.m_iReportID = CLogicCommon::GetSec();
                        std::map<std::string, std::string> stParamMap;
                        stParamMap["nick"] = pstWarGod->m_strNick;
                        reportItem.m_strReportContent = reportIter->second.m_stContentObj.Replace(stParamMap);
                        reportItem.m_cFightResult = 0;
                        reportItem.m_stParams.push_back(CLogicConfigDefine::ENTIRE_WAR_FIGHT_REPORT_GOD_REPORT_TRIGGER_VALUE);
                        CLogicGlobal::AddGlobalFightReport(reportItem);

                        break;
                    }
                }
            }

            // 存数据库
            LOGIC_RECORD_CACHE_INS->ExecSQL(0, 0, "LOGIC_BUILD_NEW_ENTIRE_WAR");

            // 清空限时BOSS
            CLogicManagerLimitBoss::ClearBoss();
        }

        // 合服
        const time_t merge_time = CLogicConfig::GetSystemConfig().m_stServerConfig.m_iMergeTime;
        if (last_time < merge_time && this_time >= merge_time)
        {
            BuildMergedEntireWar();

            CLogicGlobal::ClearGlobalFightReport(CLogicConfigDefine::FIGHT_REPORT_ENTIRE_WAR);
        }
    }
}

void CLogicGlobal::RefreshEntireWarGodData()
{
    static int32_t last_check_time = 0;
    if (LOGIC_CHECK_DAY_REFRESH(last_check_time))
    {
        auto pstWarGod = CLogicCacheCommon::LoadUserData(GetGlobalCache()->m_stBasicTable.GetEntireWarGodGroupID(), GetGlobalCache()->m_stBasicTable.GetEntireWarGodUid());
        if (nullptr != pstWarGod)
        {
            CLogicDayRefreshSystem::CheckUserDataRefresh(pstWarGod);
        }
        last_check_time = CLogicCommon::GetSec() + 60;
    }
}


void CLogicGlobal::BuildEntireWarMarketExtra(int32_t iWarID)
{
    const auto* pstConfig = CLogicConfig::GetEntireWarMapConfig().GetConfig(GetEntireWarThemeID(), iWarID);
    if (nullptr == pstConfig || pstConfig->m_iNodeType != CLogicConfigDefine::EEWNT_Market)
    {
        return;
    }

    const auto * pstMarket = CLogicConfig::GetEntireWarConfig().GetEntireWarMarketConfig(pstConfig->m_iNodePara1, GetEntireWarLevel());
    if (nullptr == pstMarket)
    {
        return;
    }

    for (size_t i = 1; i <= pstMarket->size(); ++i)
    {
        const auto bag = CLogicConfig::GetEntireWarConfig().RandEntireWarMarketBag(pstConfig->m_iNodePara1, GetEntireWarLevel(), i);
        const auto commodity = CLogicConfig::GetMallBagConfig().GetCommodityElem(bag);

        TGlobalEntireExtraTableValueType stVal;
        stVal.m_iWarID = iWarID;
        stVal.m_iExtraID = i;
        stVal.m_iServerID = CLogicConfig::GetSystemConfig().m_stServerConfig.m_iServerID;
        stVal.m_iCommodityItemType = commodity.m_stCommodityItem.m_iItemType;
        stVal.m_iCommodityItemID = commodity.m_stCommodityItem.m_iCardID;
        stVal.m_iCommodityItemNum = commodity.m_stCommodityItem.m_iNum;
        stVal.m_iConsumeItemType = commodity.m_stConsumeItem.m_iItemType;
        stVal.m_iConsumeItemID = commodity.m_stConsumeItem.m_iCardID;
        stVal.m_iConsumeItemNum = commodity.m_stConsumeItem.m_iNum;
        stVal.m_iDiscount = commodity.m_iDiscount;

        GetGlobalCache()->m_stEntireWarExtra[stVal.m_iWarID].Insert(std::make_pair(stVal.m_iExtraID, stVal));
    }
}

void CLogicGlobal::BuildEntireWarTreasureExtra(int32_t iWarID)
{
    const auto* pstConfig = CLogicConfig::GetEntireWarMapConfig().GetConfig(GetEntireWarThemeID(), iWarID);
    if (nullptr == pstConfig || pstConfig->m_iNodeType != CLogicConfigDefine::EEWNT_Treasure)
    {
        return;
    }

    const auto bag = CLogicConfig::GetEntireWarConfig().RandEntireWarTreasureBag(pstConfig->m_iNodePara1, GetEntireWarLevel());
    const auto iterBag = CLogicConfig::GetCardbagConfig().m_stCardbagConfig.find(bag);
    if (iterBag == CLogicConfig::GetCardbagConfig().m_stCardbagConfig.end())
    {
        return;
    }

    int32_t iExtra = 0;
    for (const auto& item : iterBag->second.m_stCardbagElem)
    {
        TGlobalEntireExtraTableValueType stVal;
        stVal.m_iWarID = iWarID;
        stVal.m_iExtraID = ++iExtra;
        stVal.m_iServerID = CLogicConfig::GetSystemConfig().m_stServerConfig.m_iServerID;
        stVal.m_iCommodityItemType = item.m_iItemType;
        stVal.m_iCommodityItemID = item.m_iCardID;
        stVal.m_iCommodityItemNum = item.m_iNum;
        stVal.m_iDiscount = item.m_iWeight;

        GetGlobalCache()->m_stEntireWarExtra[stVal.m_iWarID].Insert(std::make_pair(stVal.m_iExtraID, stVal));
    }
}

void CLogicGlobal::AwardEntireWarRankList()
{
    int32_t iRank = 1;
    for (auto iter = LOGIC_ENTIRE_WAR_RANK_INS.begin(); iter != LOGIC_ENTIRE_WAR_RANK_INS.end(); ++iter, ++iRank)
    {
        const user_info_key_t stKey(iter->first);
        if (1 == iRank)
        {
            GetGlobalCache()->m_stBasicTable.SetEntireWarGodUid(stKey.m_stRoleIDDetail.m_iUin);
            GetGlobalCache()->m_stBasicTable.SetEntireWarGodGroupID(stKey.m_stRoleIDDetail.m_iGroupID);
        }

        const auto* pstConfig = CLogicConfig::GetEntireWarConfig().GetEntireRankAwardConfig(iRank);
        if (nullptr != pstConfig)
        {
            CLogicGlobal::AddAsyncUserMail(stKey.m_stRoleIDDetail.m_iUin, stKey.m_stRoleIDDetail.m_iGroupID,
                                                     pstConfig->m_strMailSender, pstConfig->m_strMailTitle,
                                                     pstConfig->m_strMailContent, pstConfig->m_stBonusItemVec,
                                                     "SEND_ENTIRE_WAR_RANK_AWARD");
        }
    }

    LOGIC_ENTIRE_WAR_RANK_INS.clear();
}

void CLogicGlobal::BuildNewEntireWar()
{
    auto* pstGlobalCache = GetGlobalCache();
    pstGlobalCache->m_stBasicTable.SetEntireLastUpdateTime(CLogicCommon::GetSec());

    int32_t iTotalLevel = 0, iTotalCount = 0;
    for (auto iter = LOGIC_FIGHT_POWER_RANK_INS.begin(); iter != LOGIC_FIGHT_POWER_RANK_INS.end(); ++iter)
    {
        if (iTotalCount >= 10)
        {
            break;
        }
        const user_info_key_t stKey(iter->first);
        const auto pstDstUser = CLogicCacheCommon::LoadUserData(stKey.m_stRoleIDDetail.m_iGroupID, stKey.m_stRoleIDDetail.m_iUin);
        if (nullptr != pstDstUser)
        {
            iTotalLevel += pstDstUser->m_stUserInfoTable.GetLevel();
            ++iTotalCount;
        }
    }

    pstGlobalCache->m_stBasicTable.SetEntireWarThemeID(CLogicConfig::GetEntireWarMapConfig().GetNextThemeID(pstGlobalCache->m_stBasicTable.GetEntireWarThemeID()));
    pstGlobalCache->m_stBasicTable.SetEntireWarLevel(iTotalCount ? std::ceil(1.0f * iTotalLevel / iTotalCount) - 5 : 0);
    pstGlobalCache->m_stBasicTable.SetEntireWarBuff({});

    while (!pstGlobalCache->m_stEntireWarMap.Empty())
    {
        pstGlobalCache->m_stEntireWarMap.Erase(pstGlobalCache->m_stEntireWarMap.Begin());
    }

    for (auto& extra : pstGlobalCache->m_stEntireWarExtra)
    {
        while (!extra.second.Empty())
        {
            extra.second.Erase(extra.second.Begin());
        }
    }
    pstGlobalCache->m_stEntireWarExtra.clear();
}

void CLogicGlobal::BuildMergedEntireWar()
{
    BuildNewEntireWar();

    GetGlobalCache()->m_stBasicTable.SetEntireWarGodUid(0);
    GetGlobalCache()->m_stBasicTable.SetEntireWarGodGroupID(0);
}

TGlobalEntireMapTable* CLogicGlobal::GetEntireWarMapTable(int32_t iWarID)
{
    auto* pstGlobalCache = GetGlobalCache();
    auto iter = pstGlobalCache->m_stEntireWarMap.Find(iWarID);
    if (iter == pstGlobalCache->m_stEntireWarMap.End())
    {
        const auto* pstWarConfig = CLogicConfig::GetEntireWarMapConfig().GetConfig(GetEntireWarThemeID(), iWarID);
        if (nullptr == pstWarConfig)
        {
            return nullptr;
        }

        TGlobalEntireMapTableValueType value;
        value.m_iServerID = CLogicConfig::GetSystemConfig().m_stServerConfig.m_iServerID;
        value.m_iWarID = iWarID;
        value.m_ulTargetProgress = 0;
        value.m_ulCurProgress = 0;
        value.m_iIsPassed = 0;

        switch (pstWarConfig->m_iNodeType)
        {
        case CLogicConfigDefine::EEWNT_PVE:
        case CLogicConfigDefine::EEWNT_PVP:
        case CLogicConfigDefine::EEWNT_Question:
            value.m_ulTargetProgress = CLogicConfig::GetEntireWarConfig().GetWinCount(pstWarConfig->m_iWinCountID, GetEntireWarLevel());

            if(value.m_ulTargetProgress <= 0) return nullptr;

            break;
        case CLogicConfigDefine::EEWNT_Boss:
            value.m_ulTargetProgress = CLogicConfig::GetLevelConfig().GetLevelBossHp(pstWarConfig->m_iNodePara1)
                * CLogicConfig::GetEntireWarConfig().GetWorldLevelRatio(GetEntireWarLevel());

            if(value.m_ulTargetProgress <= 0) return nullptr;

            break;
        default:
            break;
        }

        if(value.m_ulCurProgress >= value.m_ulTargetProgress) value.m_iIsPassed = 1;

        const auto result = pstGlobalCache->m_stEntireWarMap.Insert(std::make_pair(value.m_iWarID, value));
        if (result.second)
        {
            iter = result.first;
        }
    }

    return iter == pstGlobalCache->m_stEntireWarMap.End() ? nullptr : iter->second;
}

TGlobalEntireExtraTable* CLogicGlobal::GetEntireWarExtraTable(int32_t iWarID, int32_t iExtraID)
{
    auto* pstGlobalCache = GetGlobalCache();
    auto iterWar = pstGlobalCache->m_stEntireWarExtra.find(iWarID);
    if (iterWar == pstGlobalCache->m_stEntireWarExtra.end())
    {
        return nullptr;
    }

    auto iterExtra = iterWar->second.Find(iExtraID);
    return iterExtra == iterWar->second.End() ? nullptr : iterExtra->second;
}

/************************************************************************************************************/
/******************************************全  服  战  报*****************************************************/
/************************************************************************************************************/
bool CLogicGlobal::AddGlobalFightReport(CPackFightReportItem &stReportItem)
{
    auto &typeReport = LOGIC_GLOBAL_CACHE_INS->m_stFightReportMap[stReportItem.m_iReportType];

    TFightReportTableValueType newReport;
    newReport.m_iUid = 0;
    newReport.m_iGroupID = CLogicConfig::GetSystemConfig().m_stServerConfig.m_iServerID;
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

bool CLogicGlobal::ClearGlobalFightReport(int32_t iFightReportType)
{
    auto findIter = LOGIC_GLOBAL_CACHE_INS->m_stFightReportMap.find(iFightReportType);
    if (findIter != LOGIC_GLOBAL_CACHE_INS->m_stFightReportMap.end())
    {
        auto &typeReport = findIter->second;

        while (typeReport.Size() > 0)
        {
            typeReport.Erase(typeReport.Begin());
        }
    }

    return true;
}

