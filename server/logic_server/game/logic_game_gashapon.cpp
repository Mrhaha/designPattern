//
// Created by czy on 2022/8/8.
//

#include "server_errcode.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_config.h"
#include "logic_game_active.h"
#include "logic_config_base.h"
#include "logic_game_gashapon.h"
#include "server_gashapon_protocol.h"
#include "logic_game_cache_common.h"
#include "logic_link_error.h"

REGISTER_CMD_FACTORY(MSG_LOGIC_GET_GASHAPON_INFO, CLogicGashaponProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_PLAY_GASHAPON, CLogicGashaponProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_CRUSH_BOSS_INFO, CLogicGashaponProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_PLAY_CRUSH_BOSS, CLogicGashaponProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_END_CRUSH_BOSS, CLogicGashaponProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GIVE_UP_CRUSH_BOSS, CLogicGashaponProcessor)




CLogicGashaponProcessor::CLogicGashaponProcessor(uint16_t usCmd, const std::string &strCmdName)
    : CLogicProcessorBase(usCmd, strCmdName)
{
}

bool CLogicGashaponProcessor::DoUserRun(const msgpack::unpacked& stFormatData)
{
    bool bRet = false;

#if LOGIC_BUILD_PLATFORM != BUILD_PLATFORM_LOCAL
    if (!CLogicActive::IsGashaponOpen(m_pUserInfo))
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_IS_NOT_OPEN);
    }
#endif

    switch (m_usCmd)
    {
    case MSG_LOGIC_GET_GASHAPON_INFO:
        bRet = DoUserRunGetGashaponInfo(stFormatData);
        break;
    case MSG_LOGIC_PLAY_GASHAPON:
        bRet = DoUserRunPlayGashapon(stFormatData);
        break;
    case MSG_LOGIC_GET_CRUSH_BOSS_INFO:
        bRet = DoUserRunGetCrushBossInfo(stFormatData);
        break;
    case MSG_LOGIC_PLAY_CRUSH_BOSS:
        bRet = DoUserRunPlayCrushBoss(stFormatData);
        break;
    case MSG_LOGIC_END_CRUSH_BOSS:
        bRet = DoUserRunEndCrushBoss(stFormatData);
        break;
    case MSG_LOGIC_GIVE_UP_CRUSH_BOSS:
        bRet = DoUserRunGiveUpCrushBoss(stFormatData);
        break;
    default:
        break;
    }
    return bRet;
}

bool CLogicGashaponProcessor::DoUserRunGetGashaponInfo(const msgpack::unpacked& stFormatData)
{
    CResponseGashapon stRsp;
    stRsp.m_mapGashaponInfo = m_pUserInfo->m_stIBusyData.GetGashaponInfo();

    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGashaponProcessor::DoUserRunPlayGashapon(const msgpack::unpacked& stFormatData)
{
    CRequestPlayGashapon stReq;
    if (!DecodeMsgPack(stFormatData, stReq)) return false;

    if (stReq.m_iGashID <= 0 || stReq.m_iGashID > (int32_t)CLogicConfig::GetGashaponConfig().m_uMapGashInfo.size())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    if (stReq.m_iCount <= 0 || stReq.m_iCount > 10)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    const auto gashaponConfig = CLogicConfig::GetGashaponConfig().m_uMapGashInfo.find(stReq.m_iGashID);
    if (gashaponConfig == CLogicConfig::GetGashaponConfig().m_uMapGashInfo.end())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    //计算总抽取次数，递增消耗
    int32_t totalCount = 0;
    auto gashaponInfo = m_pUserInfo->m_stIBusyData.GetGashaponInfo();
    if (stReq.m_iGashID == GashaponTypeAdv)
    {
        for(auto gashapon : gashaponInfo)
        {
            if (gashapon.first != stReq.m_iGashID)
            {
                continue;
            }
            for(auto gashItem : gashapon.second)
            {
                totalCount += gashItem.second.m_iHasBuyCount;
            }
        }
    } else
    {
        //检查之前卡池A赏抽取完毕
        if (stReq.m_iGashID > GashaponTypeNormal)
        {
            auto iterGashaponIndex = gashaponInfo.find(stReq.m_iGashID-1);
            if (gashaponInfo.end() == iterGashaponIndex)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
            }
            auto rewardIndex = CLogicConfig::GetGashaponConfig().m_uMapGashapon2Index.find(stReq.m_iGashID-1);
            for (auto& index : rewardIndex->second)
            {
                if (iterGashaponIndex->second.find(index) == iterGashaponIndex->second.end())
                {
                    SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
                }
            }
        }
    }

    CPackGameItem consumItem = gashaponConfig->second.m_vecConsumeItem[0];
    CResponsePlayGashaponGameItem stRsp;
    // 消耗道具
    while (stReq.m_iCount--)
    {

        if (stReq.m_iGashID == GashaponTypeAdv && totalCount >= 0 && totalCount < (int32_t)gashaponConfig->second.m_vecConsumeItem.size())
        {
            consumItem = gashaponConfig->second.m_vecConsumeItem[totalCount];
        }

        if(!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, consumItem, m_usCmd))
        {
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode() << "|MSG:" << CLogicError::GetErrMsg() << std::endl;
            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        //构建权重对,这里需要过滤掉已经全部抽完的商品
        std::vector<std::pair<int32_t,int32_t>> pairWeight;
        for(const auto& gashaponInfoConfig: gashaponConfig->second.m_mapGashRewardInfo)
        {
            auto iterGashaponInfo = gashaponInfo.find(stReq.m_iGashID);
            if (iterGashaponInfo != gashaponInfo.end())
            {
                auto itemIter = iterGashaponInfo->second.find(gashaponInfoConfig.first);
                if (itemIter != iterGashaponInfo->second.end())
                {
                    if (itemIter->second.m_iHasBuyCount >= gashaponInfoConfig.second.m_iTotalNum)
                    {
                        continue;
                    }
                }
            }
            pairWeight.emplace_back(gashaponInfoConfig.first,gashaponInfoConfig.second.m_iWeight);
        }

        int32_t chooseIndex = CLogicCacheCommon::RandStlCont(pairWeight);
        auto gashaponItem = gashaponConfig->second.m_mapGashRewardInfo.find(chooseIndex);
        if (gashaponItem == gashaponConfig->second.m_mapGashRewardInfo.end())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::GASHAPON_IS_EMPTY);
        }
        //抽奖记录
        auto iterGashaponInfo = gashaponInfo.find(stReq.m_iGashID);
        if (iterGashaponInfo == gashaponInfo.end())
        {
            std::map<int32_t,CPackGashaponInfo> mapGashItemInfo;
            CPackGashaponInfo itemGashapon;
            itemGashapon.m_iIndex = gashaponItem->first;
            itemGashapon.m_iHasBuyCount = 1;
            mapGashItemInfo[chooseIndex] = itemGashapon;
            gashaponInfo.insert(std::make_pair(stReq.m_iGashID,mapGashItemInfo));
        } else
        {
            auto iterGashapon = iterGashaponInfo->second.find(chooseIndex);
            if (iterGashapon == iterGashaponInfo->second.end())
            {
                CPackGashaponInfo itemGashapon;
                itemGashapon.m_iIndex = gashaponItem->first;
                itemGashapon.m_iHasBuyCount = 1;
                iterGashaponInfo->second.insert(std::make_pair(chooseIndex,itemGashapon));
            } else
            {
                iterGashapon->second.m_iHasBuyCount += 1;
            }
        }

        //发送道具
        std::string strAddItem;
        CLogicCacheCommon::AddGameItem(m_pUserInfo, gashaponItem->second.m_stItem, strAddItem, m_usCmd);
        stRsp.m_vecIndex.push_back(chooseIndex);
        m_pUserInfo->m_stIBusyData.SetGashaponInfo(gashaponInfo);
    }

    stRsp.m_iGashID = stReq.m_iGashID;
    stRsp.m_stRewardItem = m_pUserInfo->m_stCacheBonusData;
    SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGashaponProcessor::DoUserRunGetCrushBossInfo(const msgpack::unpacked& stFormatData)
{
    CResponseGetCrushBossInfo stRsp;
    stRsp.m_stBossInfo = m_pUserInfo->m_stIBusyData.GetCrushBossInfo();
    stRsp.m_iCrushBossCount = m_pUserInfo->m_stIBusyData.GetCrushBossCount();
    SendToClient(stRsp,SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGashaponProcessor::DoUserRunPlayCrushBoss(const msgpack::unpacked& stFormatData)
{
    CRequestPlayCrushBoss stReq;
    if (!DecodeMsgPack(stFormatData, stReq)) return false;

    if (!CLogicActive::IsGashaponChapterOpen(m_pUserInfo))
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_IS_NOT_OPEN);
    }

    if (stReq.m_iCrushBossLevelID <= 0)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    if (0 == m_pUserInfo->m_stIBusyData.GetCrushBossLevel() || stReq.m_iCrushBossLevelID != m_pUserInfo->m_stIBusyData.GetCrushBossLevel())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    m_pUserInfo->m_stUserInfoTable.SetFightLevelID(stReq.m_iCrushBossLevelID);
    SendToClient(SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGashaponProcessor::DoUserRunEndCrushBoss(const msgpack::unpacked& stFormatData)
{
    CRequestEndCrushBoss stReq;
    if (!DecodeMsgPack(stFormatData, stReq)) return false;

    if (!CLogicActive::IsGashaponChapterOpen(m_pUserInfo))
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_IS_NOT_OPEN);
    }

    if (stReq.m_iLevelID <= 0 || stReq.m_iLevelID != m_pUserInfo->m_stUserInfoTable.GetFightLevelID())
    {
        SendToClient(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    if (stReq.m_ulDamage < 0 || stReq.m_ulDamage > std::numeric_limits<uint64_t>::max())
    {
        SendToClient(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    auto curBossInfo = m_pUserInfo->m_stIBusyData.GetCrushBossInfo();
    if (curBossInfo.m_ulCurHurt >= curBossInfo.m_ulMaxHp)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::CRUSH_BOSS_HAS_DEAD);
    }

    if (stReq.m_ulDamage > curBossInfo.m_ulMaxHp - curBossInfo.m_ulCurHurt)
    {
        stReq.m_ulDamage = curBossInfo.m_ulMaxHp - curBossInfo.m_ulCurHurt;
    }

    curBossInfo.m_ulCurHurt += stReq.m_ulDamage;

    //crushBoss已死亡
    CResponseEndCrushBoss stRsp;
    if (curBossInfo.m_ulCurHurt >= curBossInfo.m_ulMaxHp)
    {
        //发送奖励
        auto crushBossRewardConfig = CLogicConfig::GetGashaponConfig().m_uMapBossReward;
        auto crushBossReward = crushBossRewardConfig.find(stReq.m_iLevelID);
        if (crushBossReward == crushBossRewardConfig.end())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
        }

        std::string strCombineItem;
        for (auto &randItem : crushBossReward->second)
        {
            int32_t  iRandomNum = randItem.m_iNumRange[0];
            if (randItem.m_iNumRange.size() == 2)
            {
                iRandomNum = CLogicCommon::GetRandNum(randItem.m_iNumRange[0],randItem.m_iNumRange[1]);
            }
            randItem.m_stGameItem.m_iNum = iRandomNum;
            CLogicCacheCommon::AddGameItem(m_pUserInfo, randItem.m_stGameItem, strCombineItem, m_usCmd);
        }
        stRsp.m_stCrushReward = m_pUserInfo->m_stCacheBonusData;

        //清除遭遇boss记录
        m_pUserInfo->m_stIBusyData.SetCrushBossInfo({});
        m_pUserInfo->m_stIBusyData.SetCrushBossLevel(0);
    } else
    {
        m_pUserInfo->m_stIBusyData.SetCrushBossInfo(curBossInfo);
    }

    m_pUserInfo->m_stUserInfoTable.SetFightLevelID(0);
    SendToClient(stRsp,SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGashaponProcessor::DoUserRunGiveUpCrushBoss(const msgpack::unpacked& stFormatData)
{
    CRequestGiveUpCrushBoss stReq;
    if (!DecodeMsgPack(stFormatData, stReq)) return false;

    if (stReq.m_iLevelID <= 0 || stReq.m_iLevelID != m_pUserInfo->m_stIBusyData.GetCrushBossLevel())
    {
        SendToClient(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    auto iBeforeGarde = m_pUserInfo->m_stIBusyData.GetCrushBossInfo().m_iGrade;

    auto& gashaponConfig = CLogicConfig::GetGashaponConfig();
    auto bossLevelConfig = gashaponConfig.m_uMapBossInfo;

    if (bossLevelConfig.empty())
    {
        SendToClient(SERVER_ERRCODE::CRUSH_BOSS_NOT_EXIST);
    }

    //重置遭遇boss,并直接随机一个原来星级更低的遭遇boss
    m_pUserInfo->m_stIBusyData.SetCrushBossLevel(0);
    m_pUserInfo->m_stIBusyData.SetCrushBossInfo({});

    int32_t iGrade = iBeforeGarde-1 <= CrushBossGradeThreeStar ? CrushBossGradeThreeStar : iBeforeGarde-1;
    auto iterBossLevel = bossLevelConfig.find(iGrade);
    if (iterBossLevel == bossLevelConfig.end())
    {
        SendToClient(SERVER_ERRCODE::PARAMETER_ERROR);
    }

    //确定最终boss关卡
    int32_t ret = CLogicCommon::GetRandNum(0,(int32_t)iterBossLevel->second.size());
    auto crushBossInfoConfig = iterBossLevel->second[ret];

    CPackCrushBossInfo bossInfo;
    bossInfo.m_iLevelID = crushBossInfoConfig.m_iLevelID;
    bossInfo.m_ulMaxHp = crushBossInfoConfig.m_iBossHp;
    bossInfo.m_iGrade = crushBossInfoConfig.m_iGrade;

    CResponseGiveUpCrushBoss stRsp;
    stRsp.m_stBossInfo = bossInfo;
    m_pUserInfo->m_stIBusyData.SetCrushBossLevel(crushBossInfoConfig.m_iLevelID);
    m_pUserInfo->m_stIBusyData.SetCrushBossInfo(bossInfo);
    SendToClient(stRsp,SERVER_ERRCODE::SUCCESS);
    return true;
}



