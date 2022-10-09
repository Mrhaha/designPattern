#include "server_errcode.h"
#include "server_cmd_logic_define.h"
#include "server_card_protocol.h"
#include "logic_link_common.h"
#include "logic_link_error.h"
#include "logic_config.h"
#include "logic_game_global.h"
#include "logic_game_cache_common.h"
#include "logic_game_cache_card_operation.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_game_cache_level_operation.h"
#include "logic_game_level_processor.h"
#include "logic_game_user_notify_system.h"
#include "logic_game_bill.h"
#include "logic_game_active.h"
#include "logic_game_common_publisher.h"
#include "server_cmd_cross_define.h"
#include "server_new_limit_boss_protocol.h"
#include "server_data_report_protocol.h"
#include "server_cmd_data_report_define.h"
#include "logic_config_base.h"

REGISTER_CMD_FACTORY(MSG_LOGIC_FIGHT_LEVEL, CLogicLevelProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_RETRY_FIGHT_LEVEL, CLogicLevelProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_FIGHT_LEVEL_GIFT, CLogicLevelProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_SWEEP_LEVEL, CLogicLevelProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_LEVEL_INFO, CLogicLevelProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_LEVEL_TIMES_INFO, CLogicLevelProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_CHAPTER_STAR_GIFT, CLogicLevelProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_CHALLENGE_UI_INFO, CLogicLevelProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_LEVEL_BONUS, CLogicLevelProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_EQUIP_LEVEL_AFFIX, CLogicLevelProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_ZHUHAI_CHANGE_LEVEL, CLogicLevelProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_ZHUHAI_GET_INFO, CLogicLevelProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_ZHUHAI_UPDATE_GRESS, CLogicLevelProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_ZHUHAI_ADD_BUFF, CLogicLevelProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_ZHUHAI_SET_BLESS, CLogicLevelProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_ZHUHAI_BUY_STORE, CLogicLevelProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_ZHUHAI_SAVE_TEAM, CLogicLevelProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_ZHUHAI_REFRESH_STORE, CLogicLevelProcessor)






CLogicLevelProcessor::CLogicLevelProcessor(uint16_t usCmd, const std::string &strCmdName)
        : CLogicProcessorBase(usCmd, strCmdName)
{
}

bool CLogicLevelProcessor::DoUserRun(const msgpack::unpacked& stFormatData)
{
    if ((int)m_usCmd == MSG_LOGIC_FIGHT_LEVEL ||
        (int)m_usCmd == MSG_LOGIC_RETRY_FIGHT_LEVEL)
    {
        CResponseFightLevel stRsp;
        CRequestFightLevel stParam;

        if(!DecodeMsgPack(stFormatData, stParam)) return (false);

        auto pstConfigElem = CLogicConfig::GetLevelConfig().GetLevelConfigElem(stParam.m_iLevelID);
        if (nullptr == pstConfigElem)
        {
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg()
                                << "|LEVEL_ID:" << stParam.m_iLevelID << std::endl;
            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        CLogicCacheLevelOperation stOperation(m_pUserInfo);
        auto pLevelData = stOperation.GetOrCreateLevelInfo(stParam.m_iLevelID);
        if (nullptr == pLevelData)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR)
        }

        // 检查阵容
        auto stTeam = CLogicCacheCommon::GetTeam(m_pUserInfo, pstConfigElem->m_iTeamType);
        if(stTeam.empty() || std::find_if(stTeam.begin(), stTeam.end(), [](auto& id){return id > 0;}) == stTeam.end())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::TEAM_MEMBER_CAN_NOT_EMPTY)
        }

        if(!stOperation.CheckLevelCommonLimit(pLevelData, pstConfigElem, stParam.m_iTodayMaxLevelID))
        {
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg()
                                << "|LEVEL_ID:" << stParam.m_iLevelID << std::endl;
            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        // 竹海BOSS掉落缓存
        if(pstConfigElem->m_iLevelType == CLogicConfigDefine::ELT_ZhuHaiLevel)
        {
            auto pZhuHaiConfig = CLogicConfig::GetZhuHaiConfig().GetConfig(stParam.m_iLevelID);
            if(!pZhuHaiConfig)
            {
                SEND_ERRCODE_AND_RET1(SERVER_ERRCODE::CONFIG_LOGIC_ERROR, stParam.m_iLevelID)
            }

            m_pUserInfo->m_stCacheZhuHaiDropBonus.clear();
            for(auto& stReward : *pZhuHaiConfig)
            {
                m_pUserInfo->m_stCacheZhuHaiDropBonus.push_back(CLogicConfigZhuHaiParser::GetRewards(stReward));
            }
        }

        m_pUserInfo->m_stUserInfoTable.SetFightLevelID(stParam.m_iLevelID);
        if (stParam.m_iTodayMaxLevelID != 0) m_pUserInfo->m_stUserInfoTable.SetGuildExtraLevel(stParam.m_iTodayMaxLevelID);
        m_pUserInfo->m_stCacheFightLevelDropBonus.clear();
        //这里需要过滤下不能在战斗中展示的道具
        CPackItemVec tmpDropBonus;
        CLogicConfig::GetLevelDropConfig().GetRandBonusByName(pstConfigElem->m_strBonusCardBag, m_pUserInfo->m_stCacheFightLevelDropBonus);
        for(auto &dropItem: m_pUserInfo->m_stCacheFightLevelDropBonus)
        {
            auto itemConfig = CLogicConfig::GetItemConfig().GetConfig(dropItem.m_iCardID);
            if (nullptr == itemConfig || itemConfig->m_bIsNotShowInFight)
            {
                continue;
            }

            tmpDropBonus.emplace_back(dropItem);
        }

        stRsp.m_iPhyPower = m_pUserInfo->m_stUserInfoTable.GetPhyPower();
        stRsp.m_iLastAutoAddEnergyTime = m_pUserInfo->m_stUserInfoTable.GetLastAutoAddTime();
        stRsp.m_stBonusList = tmpDropBonus;
        stRsp.m_stZhuHaiDrop = m_pUserInfo->m_stCacheZhuHaiDropBonus;

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

        if (pLevelData->GetTotalVictoryTimes() <= 0)
        {
            CLogicBill::SaveFirstFightBill(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID, m_pUserInfo->m_iChannelID, m_usCmd,
                                           stParam.m_iLevelID, 0, m_pUserInfo->m_stUserInfoTable.GetRoleCombat());
        }
        //buff次数减少
        CLogicBuff::ReduceBuffTimes(m_pUserInfo,CLogicConfigDefine::EBT_AddAttr);
        CLogicUserNotifySystem::NotifyUserBuffData(m_pUserInfo);

        //记录最近一个关卡的时间戳
        m_pUserInfo->m_stIBusyData.SetStartLevelTime(CLogicCommon::GetSec());
        return (true);
    }
    else if ((int)m_usCmd == MSG_LOGIC_GET_FIGHT_LEVEL_GIFT)
    {
        CResponseGetFightLevelGift stRsp;
        CRequestGetFightLevelGift stParam;

        if(!DecodeMsgPack(stFormatData, stParam)) return (false);

        if (stParam.m_iLevelID <= 0 || m_pUserInfo->m_stUserInfoTable.GetFightLevelID() != stParam.m_iLevelID)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::FIGHT_LEVEL_MISMATCH)
        }

        auto pstConfigElem = CLogicConfig::GetLevelConfig().GetLevelConfigElem(stParam.m_iLevelID);
        if (nullptr == pstConfigElem)
        {
            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        if(pstConfigElem->m_iLevelType == CLogicConfigDefine::ELT_Normal)
        {
            stParam.m_stFightPara.m_iGetCoinNum = 0;
        }

        if (pstConfigElem->m_iLevelType != CLogicConfigDefine::ELT_GoldLevel && stParam.m_stFightPara.m_iGetCoinNum > COIN_LIMIT)
        {
            stParam.m_stFightPara.m_iGetCoinNum = COIN_LIMIT;
        }

        auto stLevelInfoIT = m_pUserInfo->m_stLevelInfoTableMap.Find(stParam.m_iLevelID);
        if (stLevelInfoIT == m_pUserInfo->m_stLevelInfoTableMap.End())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR)
        }

        CLogicCacheLevelOperation stOperation(m_pUserInfo);
        if(!stOperation.CheckLevelFightBefore(stLevelInfoIT->second, pstConfigElem, stParam.m_stFightPara))
        {
            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        if(!stOperation.UpdateLevelTimes(stLevelInfoIT->second, pstConfigElem))
        {
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg()
                                << "|LEVEL_ID:" << stParam.m_iLevelID << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        //战力验证
        int64_t lDps = 0;
        int64_t lMaxDps = 0;
        bool isCheating = AntiCheatingCheck(pstConfigElem,stParam.m_stFightPara,&lDps,&lMaxDps);
        auto passLevelTime = CLogicCommon::GetSec() - m_pUserInfo->m_stIBusyData.GetStartLevelTime();
        auto stTeam = CLogicCacheCommon::GetTeam(m_pUserInfo, pstConfigElem->m_iTeamType);
        if (isCheating || passLevelTime <= CLogicConfig::GetAwardConfig().iPassLevelTime)
        {
            CLogicBill::SaveAntiCheatingBill(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID, m_pUserInfo->m_iChannelID, m_usCmd, stParam.m_iLevelID,lDps, lMaxDps, stTeam, "",passLevelTime);
        }
        stRsp.m_cLevelStar = stOperation.CalcLevelStar(pstConfigElem, stParam.m_stFightPara);

        std::vector<CPackGameItem> stRewards;
        std::vector<CPackGameItem> stExtraRewards;

        stRewards = m_pUserInfo->m_stCacheFightLevelDropBonus;
        CLogicConfig::GetLevelDropBagConfig().GetRandReward(pstConfigElem->m_strDropBag, stRewards);

        int iCoinNum = std::min(stParam.m_stFightPara.m_iGetCoinNum, pstConfigElem->m_iMaxCoinNum);


        stRewards.emplace_back(CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_GoldID, iCoinNum);
        //完美奖励
        if (CLogicConfigDefine::ESD_LevelFullStarFlag == stRsp.m_cLevelStar)
        {
            CLogicConfig::GetLevelDropConfig().GetRandBonusByName(pstConfigElem->m_strPerfectCardBag, stRewards);
        }

        //是否开启双倍
        auto levelType = CLogicConfig::GetDoubleRewardConfig().GetDoubleRewardConfig(pstConfigElem->m_iLevelType);
        if (levelType != nullptr)
        {
            for(auto avy_time : levelType->m_iAvy_time)
            {
                if(avy_time > 0 && !CLogicActive::IsAvyOpen(avy_time, m_pUserInfo->m_stUserInfoTable.GetCreateTime()))
                {
                    continue;
                }
                stRewards.insert(stRewards.end(),stRewards.begin(),stRewards.end());
            }
        }

        // 首通奖励
        if (stLevelInfoIT->second->GetTotalVictoryTimes() == 1 && !pstConfigElem->m_stFirstBonusVec.empty())
        {
            stRewards.insert(stRewards.end(), pstConfigElem->m_stFirstBonusVec.begin(), pstConfigElem->m_stFirstBonusVec.end());
            for(auto& item : pstConfigElem->m_stFirstBonusVec)
            {
                stRsp.m_stFirstBonusMap[item.m_iCardID] = item.m_iItemType;
            }
        }
        stRewards.emplace_back(CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_RoleExpID, pstConfigElem->m_iBonusTeamExp);

        // 其他类型副本奖励
        auto extraLevelID = m_pUserInfo->m_stUserInfoTable.GetGuildExtraLevel();
        stOperation.GetOtherLevelReward(stParam.m_iLevelID, pstConfigElem, stParam.m_stFightPara.m_ulPara,extraLevelID,stRewards,stExtraRewards);

        // 更新副本通关时间
        if(stLevelInfoIT->second->GetCostTime() <= 0 || stParam.m_stFightPara.m_iCostTime < stLevelInfoIT->second->GetCostTime())
            stLevelInfoIT->second->SetCostTime(stParam.m_stFightPara.m_iCostTime);

        //检查公会buff
        auto guildBuffIter = m_pUserInfo->m_stOnlyInCacheData.m_mapGuildBuff.find(EnumGuildBuffType::GuildBuffGoldAdd);
        if (guildBuffIter != m_pUserInfo->m_stOnlyInCacheData.m_mapGuildBuff.end())
        {
            for(auto& item : stRewards)
            {
                if (item.m_iItemType == CLogicConfigDefine::CURRENCY && item.m_iCardID == CLogicConfigDefine::ECIT_GoldID)
                {
                    item.m_iNum = (int32_t) ((float)item.m_iNum * (1 + guildBuffIter->second.m_iBuffParam[0]/100));
                }
            }
        }


        //////////////////// Add User Game Item
        static std::string strBonusItem;
        CLogicCacheCommon::AddGameItem(m_pUserInfo, stRewards, strBonusItem, m_usCmd, {{"level_id", std::to_string(stParam.m_iLevelID)}});

        static std::string strExtraBonusItem;
        CLogicCacheCommon::AddGameItem(m_pUserInfo, stExtraRewards, strExtraBonusItem, m_usCmd, {{"level_id", std::to_string(stParam.m_iLevelID)}});

        //////////////////// Upgrade Card Exp
        CLogicCacheCardOperation stCardOP(m_pUserInfo);
        stCardOP.UpgradeTeamCardLevelExp(pstConfigElem->m_iBonusExp, stRsp.m_stTeamLevelVec, pstConfigElem->m_iTeamType);

        // 星数更新
        stLevelInfoIT->second->SetLevelStar(stLevelInfoIT->second->GetLevelStar() | stRsp.m_cLevelStar);

        m_pUserInfo->m_stUserInfoTable.SetFightLevelID(0);
        m_pUserInfo->m_stUserInfoTable.SetLastLeaveGuildTime(0);

        // 更新副本成就
        if(stLevelInfoIT->second->GetAchieveTag() != CLogicConfigDefine::ESD_LevelFullStarFlag)
        {
            char cFlag = stLevelInfoIT->second->GetAchieveTag();
            char cFullFlag = 0;
            for(std::size_t i = 0; i < pstConfigElem->m_stAchieveBonus.size(); ++i)
            {
                if(pstConfigElem->m_stAchieveBonus[i])
                    cFullFlag |= (1 << i);

                if(i < stParam.m_stFightPara.m_stAchieveTagVec.size() && stParam.m_stFightPara.m_stAchieveTagVec[i])
                    cFlag |= (1 << i);
            }

            if(((cFullFlag & stLevelInfoIT->second->GetAchieveTag()) != cFullFlag) && ((cFullFlag & cFlag) == cFullFlag))
            {
                // 通用事件
                CLogicEventData stEventData;
                stEventData.m_eType = CLogicConfigDefine::ECET_ChapterAchieveCompleteNum;
                stEventData.m_iPara = pstConfigElem->m_iChapterID;
                stEventData.m_iCmd = m_usCmd;
                LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);
            }
            stLevelInfoIT->second->SetAchieveTag(cFlag);
        }

        stRsp.m_stItemData = m_pUserInfo->m_stCacheBonusData;
        stRsp.m_stLevelInfo = CLogicCacheCommon::GetUserLevelInfo(stLevelInfoIT->second->m_stData);
        //添加关卡额外掉落展示
        stOperation.CalcExtraLevelReward(stParam.m_iLevelID, pstConfigElem,stExtraRewards,&stRsp.m_stExtraItemData);

        // 是否刷新限时Boss，这里要记录下如果已经存在了就不要再刷新了
        if (!m_pUserInfo->m_stOnlyInCacheData.m_bHasLimitBoss)
        {
            stRsp.m_stLevelInfo.m_bLimitBoss = stOperation.IsRefreshLimitBoss(stParam.m_iLevelID);
        }

        if (0 == m_pUserInfo->m_stIBusyData.GetCrushBossLevel())
        {
            CPackCrushBossInfo crushBossInfo;
            stRsp.m_stLevelInfo.m_bCrushBoss = IsRefreshCrushBoss(stParam.m_iLevelID,crushBossInfo);
            m_pUserInfo->m_stIBusyData.SetCrushBossLevel(crushBossInfo.m_iLevelID);

            if (0 != crushBossInfo.m_iLevelID)
            {
                //记录boss属性
                m_pUserInfo->m_stIBusyData.SetCrushBossInfo(crushBossInfo);
            }
        }

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|LEVEL_ID:" << stParam.m_iLevelID << "|LEVEL_STAR:" << stRsp.m_cLevelStar
                            << "|BONUS_CARD_EXP:" << pstConfigElem->m_iBonusExp << "|LEVEL_TOTAL_BONUS:" << strBonusItem
                            << "|EXTRA_BONUS: " << strExtraBonusItem
                            << std::endl;
        //刷新限时boss告诉跨服
        if (stRsp.m_stLevelInfo.m_bLimitBoss)
        {
            CCrossNotifyNewLimitBoss stCrossBody;
            CPackNewLimitBossInfo bossInfo;
            bossInfo.m_iAppearLeveID = stParam.m_iLevelID;  //记录当前出现的levelID
            stOperation.InitLimitBoss(&bossInfo);

            auto hasFoundLimitBoss = m_pUserInfo->m_stUserInfoTable.GetFindLimitBoss();
            auto hasFound = hasFoundLimitBoss.find(bossInfo.m_iLevelID);
            if (hasFound == hasFoundLimitBoss.end())
            {
                hasFoundLimitBoss.insert(std::make_pair(bossInfo.m_iLevelID,1));
                m_pUserInfo->m_stUserInfoTable.SetFindLimitBoss(hasFoundLimitBoss);
            }

            stCrossBody.m_stBossInfo = bossInfo;
            stCrossBody.m_strFinderName = m_pUserInfo->m_strNick;
            stCrossBody.m_iIconHead = m_pUserInfo->m_stUserInfoTable.GetAvatarID();
            stCrossBody.m_iIconImage = m_pUserInfo->m_stUserInfoTable.GetAvatarBorder();
            SendToCrossServer(enmCmdType_request, MSG_CROSS_NEW_LIMIT_BOSS_NOTIFY, stCrossBody);
        }

        if (stLevelInfoIT->second->GetTotalVictoryTimes() == 1)
        {
            CLogicBill::SaveFirstFightBill(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID, m_pUserInfo->m_iChannelID, m_usCmd, stParam.m_iLevelID, 1, m_pUserInfo->m_stUserInfoTable.GetRoleCombat());

            // 通用事件
            CLogicEventData stEventData;
            stEventData.m_eType = CLogicConfigDefine::ECET_PvELevelFirstFight;
            stEventData.m_iPara = stParam.m_iLevelID;
            stEventData.m_iCmd = m_usCmd;
            LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);
        }

        CLogicBill::SaveFightPassBill(m_pUserInfo->m_iUin, m_pUserInfo->m_iGroupID, m_pUserInfo->m_iChannelID, m_usCmd, stParam.m_iLevelID,
                                      m_pUserInfo->m_stUserInfoTable.GetRoleCombat(), stTeam, "");

        // 文文日报
        std::vector<std::string> stWenWenParam;
        stWenWenParam.push_back(m_pUserInfo->m_strNick);
        stWenWenParam.push_back(to_string(stParam.m_iLevelID));
        CLogicGlobal::UpdateWenWenNewsEvent(m_iUin, m_iGroupID, CLogicConfigDefine::EWWNT_PVE_Level, stWenWenParam);

        // 文文日报
        stWenWenParam.push_back(to_string(stParam.m_stFightPara.m_iCostTime));
        CLogicGlobal::UpdateWenWenNewsEvent(m_iUin, m_iGroupID, CLogicConfigDefine::EWWNT_PVE_Time, stWenWenParam);

        // 通用事件
        CLogicEventData stEventData;
        stEventData.m_eType = CLogicConfigDefine::ECET_LevelTypeTimes;
        stEventData.m_iPara = pstConfigElem->m_iLevelType;
        stEventData.m_iCmd = m_usCmd;
        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);
        // 通用事件
        CLogicEventData stEventWipe;
        stEventWipe.m_eType = CLogicConfigDefine::ECET_WipeFightTotalNum;
        stEventWipe.m_iCount = stParam.m_stFightPara.m_iWipeFightNum;
        stEventWipe.m_iCmd = m_usCmd;
        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventWipe);

        stEventWipe.m_eType = CLogicConfigDefine::ECET_WipeFightOnceNum;
        stEventWipe.m_iCmd = m_usCmd;
        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventWipe);

        stEventWipe.m_eType = CLogicConfigDefine::ECET_TotalMonster;
        auto totalMonster = 0;
        for (auto iter : stParam.m_stFightPara.m_stKillMonsterMap)
        {
            totalMonster += iter.second;
        }
        stEventWipe.m_iCount = totalMonster;
        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventWipe);

        // 后续处理
        stOperation.HandleLevelFightAfter(stLevelInfoIT->second, pstConfigElem, stParam.m_stFightPara);

        //更新释放奥义记录
        for(auto cardUserSkill : stParam.m_stFightPara.m_stCardUseSkill)
        {
            auto iterCard = m_pUserInfo->m_stHeroCardTableMap.Find(cardUserSkill.first);
            if (iterCard == m_pUserInfo->m_stHeroCardTableMap.End())
            {
                continue;
            }
            iterCard->second->AddUseSkillCount(cardUserSkill.second);
            if (stParam.m_stFightPara.m_stCardStatus[cardUserSkill.first] > 0)
            {
                auto passLevel = iterCard->second->GetPassLevel();
                passLevel.insert(std::make_pair(stParam.m_iLevelID,true));
                iterCard->second->SetPassLevel(passLevel);
            }
        }

        // 徽章更新
        CLogicBadgeManage::UpdateBadgeCond(m_pUserInfo, CLogicConfigDefine::EBCT_LevelFight, stParam.m_iLevelID, stParam.m_stFightPara);

        // 关卡排行榜更新
        CLogicGlobal::UpdateOrInsertCommonRank(stParam.m_iLevelID, m_iUin, m_iGroupID, stParam.m_stFightPara.m_iCostTime);
        return (true);
    }
    else if ((int)m_usCmd == MSG_LOGIC_SWEEP_LEVEL)
    {
        CResponseSweepLevel stRsp;
        CRequestSweepLevel stParam;

        if (DecodeMsgPack(stFormatData, stParam) == false) return (false);

        if (stParam.m_iTimes <= 0 || stParam.m_iTimes >= 99)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
        }

        if (m_pUserInfo->m_stUserInfoTable.GetLevel() < CLogicConfig::GetGlobalConfig().m_iSweepLevelLimit)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ROLE_LEVEL)
        }

        auto pstConfigElem = CLogicConfig::GetLevelConfig().GetLevelConfigElem(stParam.m_iLevelID);
        if (nullptr == pstConfigElem)
        {
            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        auto stUserLevelIter = m_pUserInfo->m_stLevelInfoTableMap.Find(stParam.m_iLevelID);
        if (stUserLevelIter == m_pUserInfo->m_stLevelInfoTableMap.End())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::SWEEP_LEVEL_NOT_THREE_STAR)
        }

        // 星数判断
        if(CLogicConfigDefine::ESD_LevelFullStarFlag != stUserLevelIter->second->GetLevelStar())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::SWEEP_LEVEL_NOT_THREE_STAR)
        }

        CLogicCacheLevelOperation stOperation(m_pUserInfo);
        std::vector<CPackGameItem> stBonusVec;
        std::vector<CPackGameItem> stExtraBonusVec;
        std::string strBonusItem;
        stRsp.m_iLevelID = stParam.m_iLevelID;

        bool bFlagLimitBoss = false;
        while (stParam.m_iTimes--)
        {
            ++stRsp.m_iTimes;
            stBonusVec.clear();
            m_pUserInfo->m_stCacheBonusData = {};

            if(!stOperation.CheckLevelCommonLimit(stUserLevelIter->second, pstConfigElem))
                break;

            // 扫荡消耗
            if(pstConfigElem->m_stSweepItem.m_iItemType > 0)
            {
                if(CLogicCacheCommon::GetItemNum(m_pUserInfo, pstConfigElem->m_stSweepItem.m_iItemType, pstConfigElem->m_stSweepItem.m_iCardID) < pstConfigElem->m_stSweepItem.m_iNum)
                {
                    SET_ERR_INFO(SERVER_ERRCODE::NOT_ENOUGH_ITEM);
                    break;
                }
            }

            // 消耗次数
            stOperation.UpdateLevelTimes(stUserLevelIter->second, pstConfigElem);

            // 扫荡消耗
            if(pstConfigElem->m_stSweepItem.m_iItemType > 0)
            {
                CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, pstConfigElem->m_stSweepItem, m_usCmd);
            }

            // Total Sweep Bonus
            CLogicConfig::GetLevelDropConfig().GetRandBonusByName(pstConfigElem->m_strBonusCardBag, stBonusVec);
            CLogicConfig::GetLevelDropConfig().GetRandBonusByName(pstConfigElem->m_strPerfectCardBag, stBonusVec);
            CLogicConfig::GetLevelDropBagConfig().GetRandReward(pstConfigElem->m_strDropBag, stBonusVec);

            //是否开启双倍
            auto levelType = CLogicConfig::GetDoubleRewardConfig().GetDoubleRewardConfig(pstConfigElem->m_iLevelType);
            if (levelType != nullptr)
            {
                for(auto avy_time : levelType->m_iAvy_time)
                {
                    if(avy_time > 0 && !CLogicActive::IsAvyOpen(avy_time, m_pUserInfo->m_stUserInfoTable.GetCreateTime()))
                    {
                        continue ;
                    }
                    stBonusVec.insert(stBonusVec.end(),stBonusVec.begin(),stBonusVec.end());
                }
            }

            // 其他类型副本奖励
            stOperation.GetOtherLevelReward(stParam.m_iLevelID, pstConfigElem, 0, 0,stBonusVec,stExtraBonusVec);

            stBonusVec.emplace_back(CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_RoleExpID, pstConfigElem->m_iBonusTeamExp);

            //检查公会buff
            auto guildBuffIter = m_pUserInfo->m_stOnlyInCacheData.m_mapGuildBuff.find(EnumGuildBuffType::GuildBuffGoldAdd);
            if (guildBuffIter != m_pUserInfo->m_stOnlyInCacheData.m_mapGuildBuff.end())
            {
                for(auto& item : stBonusVec)
                {
                    if (item.m_iItemType == CLogicConfigDefine::CURRENCY && item.m_iCardID == CLogicConfigDefine::ECIT_GoldID)
                    {
                        item.m_iNum = (int32_t) ((float)item.m_iNum * (1 + guildBuffIter->second.m_iBuffParam[0]/100));
                    }
                }
            }

            static std::string strBonus;
            CLogicCacheCommon::AddGameItem(m_pUserInfo, stBonusVec, strBonus, m_usCmd,
                                           {{"level_id", std::to_string(stParam.m_iLevelID)}});

            static std::string strExtraBonus;
            CLogicCacheCommon::AddGameItem(m_pUserInfo, stExtraBonusVec, strExtraBonus, m_usCmd,
                                           {{"level_id", std::to_string(stParam.m_iLevelID)}});
            strBonusItem += strBonus;
            strBonusItem += strExtraBonus;

            stRsp.m_stItemDataVec.push_back(m_pUserInfo->m_stCacheBonusData);
            //添加额外掉落展示
            stOperation.CalcExtraLevelReward(stParam.m_iLevelID, pstConfigElem,stExtraBonusVec,&stRsp.m_stExtraItemData);
            // 检查需要的道具
            if(stParam.m_iNeedItemType > 0)
            {
                for(auto& item : stBonusVec)
                {
                    if(item.m_iItemType == stParam.m_iNeedItemType && item.m_iCardID == stParam.m_iNeedItemID)
                        stParam.m_iNeedNum -= item.m_iNum;
                }

                if(stParam.m_iNeedNum <= 0)
                    break;
            }

            //检查刷新限时boss
            if (!m_pUserInfo->m_stOnlyInCacheData.m_bHasLimitBoss && !bFlagLimitBoss)
            {
                stRsp.m_bLimitBoss = stOperation.IsRefreshLimitBoss(stParam.m_iLevelID);
            }

            if (0 == m_pUserInfo->m_stIBusyData.GetCrushBossLevel())
            {
                CPackCrushBossInfo crushBossInfo;
                stRsp.m_bCrushBoss = IsRefreshCrushBoss(stParam.m_iLevelID,crushBossInfo);
                m_pUserInfo->m_stIBusyData.SetCrushBossLevel(crushBossInfo.m_iLevelID);

                if (0 != crushBossInfo.m_iLevelID)
                {
                    //记录boss属性
                    m_pUserInfo->m_stIBusyData.SetCrushBossInfo(crushBossInfo);
                }
            }

            if (stRsp.m_bLimitBoss && !bFlagLimitBoss)
            {
                bFlagLimitBoss = true;
                CCrossNotifyNewLimitBoss stCrossBody;
                CPackNewLimitBossInfo bossInfo;
                bossInfo.m_iAppearLeveID = stParam.m_iLevelID;  //记录当前出现的levelID
                stOperation.InitLimitBoss(&bossInfo);

                auto hasFoundLimitBoss = m_pUserInfo->m_stUserInfoTable.GetFindLimitBoss();
                auto hasFound = hasFoundLimitBoss.find(bossInfo.m_iLevelID);
                if (hasFound == hasFoundLimitBoss.end())
                {
                    hasFoundLimitBoss.insert(std::make_pair(bossInfo.m_iLevelID,1));
                    m_pUserInfo->m_stUserInfoTable.SetFindLimitBoss(hasFoundLimitBoss);
                }

                stCrossBody.m_stBossInfo = bossInfo;
                stCrossBody.m_strFinderName = m_pUserInfo->m_strNick;
                stCrossBody.m_iIconHead = m_pUserInfo->m_stUserInfoTable.GetAvatarID();
                stCrossBody.m_iIconImage = m_pUserInfo->m_stUserInfoTable.GetAvatarBorder();
                SendToCrossServer(enmCmdType_request, MSG_CROSS_NEW_LIMIT_BOSS_NOTIFY, stCrossBody);
                PROCESSOR_LOG_DEBUG << "PROCESS LIMIT_BOSS|LEVEL_ID:" << bossInfo.m_iLevelID << "LIMIT_BOSS_HP" << bossInfo.m_ulMaxHp << std::endl;
            }
        }

        if(stRsp.m_stItemDataVec.empty())
        {
            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        //////////////////// Upgrade Card Exp
        CLogicCacheCardOperation stCardOP(m_pUserInfo);
        stCardOP.UpgradeTeamCardLevelExp(pstConfigElem->m_iBonusExp * stRsp.m_iTimes, stRsp.m_stTeamLevelVec, pstConfigElem->m_iTeamType);

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|LEVEL_ID:" << stParam.m_iLevelID << "|SWEEP_TIMES:" << stRsp.m_iTimes
                            << "|BONUS_CARD_EXP:" << pstConfigElem->m_iBonusExp * stRsp.m_iTimes
                            << "|LEVEL_TOTAL_BONUS:" << strBonusItem << std::endl;

        // 通用事件
        CLogicEventData stEventData;
        stEventData.m_eType = CLogicConfigDefine::ECET_LevelTypeTimes;
        stEventData.m_iPara = pstConfigElem->m_iLevelType;
        stEventData.m_iTimes = stRsp.m_iTimes;
        stEventData.m_iCmd = m_usCmd;
        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);

        return (true);
    }
    else if ((int)m_usCmd == MSG_LOGIC_GET_LEVEL_INFO)
    {
        CResponseGetLevelInfo stRsp;

        auto iterLevelBeg = m_pUserInfo->m_stLevelInfoTableMap.Begin();
        auto iterLevelEnd = m_pUserInfo->m_stLevelInfoTableMap.End();

        for ( ; iterLevelBeg != iterLevelEnd; ++iterLevelBeg)
        {
            stRsp.m_stLevelInfoVec.push_back(CLogicCacheCommon::GetUserLevelInfo(iterLevelBeg->second->m_stData));
        }

        stRsp.m_strNormalChapterBonus1 = int2bin(m_pUserInfo->m_stUserInfoTable.GetNormalChapterBonus1());
        stRsp.m_strNormalChapterBonus2 = int2bin(m_pUserInfo->m_stUserInfoTable.GetNormalChapterBonus2());
        stRsp.m_strNormalChapterBonus3 = int2bin(m_pUserInfo->m_stUserInfoTable.GetNormalChapterBonus3());
        stRsp.m_strEliteChapterBonus1 = int2bin(m_pUserInfo->m_stUserInfoTable.GetEliteChapterBonus1());
        stRsp.m_strEliteChapterBonus2 = int2bin(m_pUserInfo->m_stUserInfoTable.GetEliteChapterBonus2());
        stRsp.m_strEliteChapterBonus3 = int2bin(m_pUserInfo->m_stUserInfoTable.GetEliteChapterBonus3());

        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS" << std::endl;

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    }
    else if ((int)m_usCmd == MSG_LOGIC_GET_LEVEL_TIMES_INFO)
    {
        // 获取指定副本ID的次数记录详情
        CRequestGetLevelTimesInfo stReq;
        CResponseGetLevelTimesInfo stRsp;

        if (DecodeMsgPack(stFormatData, stReq) == false) return (false);

        if(stReq.m_stLevelIds.size() > 200)
        {
            using namespace SERVER_ERRCODE;
            SET_ERR_INFO(PARAMETER_ERROR);
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode() << "|MSG:" << CLogicError::GetErrMsg() << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        for (const auto& levelID : stReq.m_stLevelIds)
        {
            auto stLevelIter = m_pUserInfo->m_stLevelInfoTableMap.Find(levelID);
            if (stLevelIter != m_pUserInfo->m_stLevelInfoTableMap.End())
            {
                stRsp.m_stLeveTimesInfoMap.push_back(CLogicCacheCommon::GetUserLevelInfo(stLevelIter->second->m_stData));
            }
        }

        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS" << std::endl;

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    }
    else if ((int)m_usCmd == MSG_LOGIC_GET_CHAPTER_STAR_GIFT)
    {
        CResponseChapterStarGift stRsp;
        CRequestChapterStarGift stParam;

        if (DecodeMsgPack(stFormatData, stParam) == false) return (false);

        auto pstConfigElem = CLogicConfig::GetChapterConfig().GetChapterConfigElem(stParam.m_iChapterID);
        if (nullptr == pstConfigElem)
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg()
                                << "|CHAPTER_ID:" << stParam.m_iChapterID << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        auto stCheckRet = CLogicCacheLevelOperation::CheckGetChapterBonus(m_pUserInfo, pstConfigElem, stParam.m_iChapterID, stParam.m_iPosition);
        if (false == stCheckRet.second)
        {
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg()
                                << "|CHAPTER_ID:" << stParam.m_iChapterID 
                                << "|POSITION:" << stParam.m_iPosition << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        auto pstBonusItemVec = CLogicCacheLevelOperation::GetChapterBonus(pstConfigElem, stParam.m_iPosition);
        if (nullptr == pstBonusItemVec)
        {
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg()
                                << "|CHAPTER_ID:" << stParam.m_iChapterID 
                                << "|POSITION:" << stParam.m_iPosition << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        CLogicCacheLevelOperation::SetChapterBonusBitmap(m_pUserInfo, stParam.m_iChapterID, stParam.m_iPosition, stCheckRet.first);

        static std::string strBonusItem;
        CLogicCacheCommon::AddGameItem(m_pUserInfo, *pstBonusItemVec, strBonusItem, m_usCmd);

        stRsp.m_iChapterID = stParam.m_iChapterID;
        stRsp.m_iPosition = stParam.m_iPosition;
        stRsp.m_stBonusListVec = *pstBonusItemVec;
        stRsp.m_strChapterBonus = int2bin(stCheckRet.first);

        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|CHAPTER_ID:" << stParam.m_iChapterID
                            << "|POSITION:" << stParam.m_iPosition
                            << "|BONUS_LIST:" << strBonusItem
                            << "|CHAPTER_BONUS_BITMAP:" << stRsp.m_strChapterBonus << std::endl;

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    }
    else if ((int)m_usCmd == MSG_LOGIC_GET_CHALLENGE_UI_INFO)
    {
//        CResponseChallengeUiInfo stRsp;
//        stRsp.m_stPVPInfo.m_iLeftTimes = CLogicConfig::GetPVPConfig().m_iDailyChallengeTimes - m_pUserInfo->m_stUserInfoTable.GetPvpFightTimes();
//        if(stRsp.m_stPVPInfo.m_iLeftTimes < 0) stRsp.m_stPVPInfo.m_iLeftTimes = 0;
//
//        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
//
//        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS" << std::endl;
        return (true);
    }
    else if((int)m_usCmd == MSG_LOGIC_GET_EQUIP_LEVEL_AFFIX)
    {
        CResponseGetEquipLevelAffix stRsp;
        stRsp.m_stAffixVec = CLogicGlobal::GetGlobalCache()->m_stBasicTable.GetAffixVec();
        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
        return (true);
    }
    else if(m_usCmd == MSG_LOGIC_ZHUHAI_GET_INFO)
    {
        CResponseZhuHaiGetInfo stRsp;
        stRsp.m_iLevelID = m_pUserInfo->m_stIBusyData.GetZhuHaiLevel();
        stRsp.m_iGress = m_pUserInfo->m_stIBusyData.GetZhuHaiGress();
        stRsp.m_stBuffVec = m_pUserInfo->m_stIBusyData.GetZhuHaiBuff();
        stRsp.m_stCardHp = m_pUserInfo->m_stIBusyData.GetZhuHaiCardHp();
        stRsp.m_stBless = m_pUserInfo->m_stIBusyData.GetZhuHaiBlessGroup();
        stRsp.m_stRoleBless = m_pUserInfo->m_stIBusyData.GetZhuHaiRoleBuff();
        stRsp.m_bSaveTeam = m_pUserInfo->m_stIBusyData.GetZhuHaiSaveTeamFlag();
        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
        return (true);
    }
    else if(m_usCmd == MSG_LOGIC_ZHUHAI_SAVE_TEAM)
    {
        m_pUserInfo->m_stIBusyData.SetZhuHaiSaveTeamFlag(1);
        SendToClient(SERVER_ERRCODE::SUCCESS);
        return (true);
    }
    else if(m_usCmd == MSG_LOGIC_ZHUHAI_CHANGE_LEVEL)
    {
        CRequestZhuHaiChangeLevel stReq;
        CResponseZhuHaiGetInfo stRsp;
        if(DecodeMsgPack(stFormatData, stReq) == false) return (false);

#if LOGIC_BUILD_PLATFORM != BUILD_PLATFORM_LOCAL
        // 已选定难度
        if(m_pUserInfo->m_stIBusyData.GetZhuHaiLevel() > 0)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
        }

        auto pLevelConfig = CLogicConfig::GetLevelConfig().GetLevelConfigElem(stReq.m_iLevelID);
        if(nullptr == pLevelConfig || pLevelConfig->m_iLevelType != CLogicConfigDefine::ELT_ZhuHaiLevel)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
        }
#endif

        m_pUserInfo->m_stIBusyData.SetZhuHaiLevel(stReq.m_iLevelID);
        m_pUserInfo->m_stIBusyData.SetZhuHaiGress(0);
        m_pUserInfo->m_stIBusyData.SetZhuHaiBuff({});
        m_pUserInfo->m_stIBusyData.SetZhuHaiCardHp({});
        m_pUserInfo->m_stIBusyData.SetZhuHaiBlessGroup({});
        m_pUserInfo->m_stIBusyData.SetZhuHaiRoleBuff({});
        m_pUserInfo->m_stUserInfoTable.SetZhuhaiScore(0);

        // 重新发送信息
        stRsp.m_iLevelID = m_pUserInfo->m_stIBusyData.GetZhuHaiLevel();
        stRsp.m_iGress = m_pUserInfo->m_stIBusyData.GetZhuHaiGress();
        stRsp.m_stBuffVec = m_pUserInfo->m_stIBusyData.GetZhuHaiBuff();
        stRsp.m_stCardHp = m_pUserInfo->m_stIBusyData.GetZhuHaiCardHp();
        stRsp.m_stBless = m_pUserInfo->m_stIBusyData.GetZhuHaiBlessGroup();
        stRsp.m_stRoleBless = m_pUserInfo->m_stIBusyData.GetZhuHaiRoleBuff();
        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
        return (true);
    }
    else if(m_usCmd == MSG_LOGIC_ZHUHAI_UPDATE_GRESS)
    {
        CRequestZhuHaiUpdateGress stReq;
        if(DecodeMsgPack(stFormatData, stReq) == false) return (false);

        // 判断当前战斗中的副本
        if(m_pUserInfo->m_stUserInfoTable.GetFightLevelID() <= 0 ||
            m_pUserInfo->m_stUserInfoTable.GetFightLevelID() != m_pUserInfo->m_stIBusyData.GetZhuHaiLevel())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ZHUHAI_LEVEL_ERROR)
        }

        if(m_pUserInfo->m_stIBusyData.GetZhuHaiGress() >= stReq.m_iGress)
        {
            SendToClient(SERVER_ERRCODE::SUCCESS);
            return (true);
        }

        if(stReq.m_iGress < 1 || stReq.m_iGress > int(m_pUserInfo->m_stCacheZhuHaiDropBonus.size()))
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
        }

        // 更新血量
        auto stCardHp = m_pUserInfo->m_stIBusyData.GetZhuHaiCardHp();
        for(auto& stData : stReq.m_stCardHp)
        {
            stCardHp[stData.first] = stData.second;
        }

        if(stCardHp.size() > CLogicConfigDefine::ESD_TeamMaxCardNum)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
        }

        std::vector<CPackGameItem> stRewards;
        for(int i = m_pUserInfo->m_stIBusyData.GetZhuHaiGress() + 1; i <= stReq.m_iGress; ++i)
        {
            auto& stItemVec = m_pUserInfo->m_stCacheZhuHaiDropBonus[i - 1];
            stRewards.insert(stRewards.end(), stItemVec.begin(), stItemVec.end());
        }

        m_pUserInfo->m_stIBusyData.SetZhuHaiGress(stReq.m_iGress);
        m_pUserInfo->m_stIBusyData.SetZhuHaiCardHp(stCardHp);

        static std::string strBonus;
        CLogicCacheCommon::AddGameItem(m_pUserInfo, stRewards, strBonus, m_usCmd);
        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|LEVEL_ID:" << m_pUserInfo->m_stIBusyData.GetZhuHaiLevel()
                            << "|GRESS:" << stReq.m_iGress << "|BONUS_ITEM:" << strBonus << std::endl;

        SendSuccessCacheItemToClient();
        return (true);
    }
    else if(m_usCmd == MSG_LOGIC_ZHUHAI_ADD_BUFF)
    {
        CRequestZhuHaiAddBuff stReq;
        if(DecodeMsgPack(stFormatData, stReq) == false) return (false);

        if(stReq.m_iBuffID <= 0)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
        }

        // 判断当前战斗中的副本
        if(m_pUserInfo->m_stUserInfoTable.GetFightLevelID() <= 0 ||
           m_pUserInfo->m_stUserInfoTable.GetFightLevelID() != m_pUserInfo->m_stIBusyData.GetZhuHaiLevel())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ZHUHAI_LEVEL_ERROR)
        }

        auto stData = m_pUserInfo->m_stIBusyData.GetZhuHaiBuff();
        if(stReq.m_iReplaceBuffID > 0)
        {
            auto iter = std::find(stData.begin(), stData.end(), stReq.m_iReplaceBuffID);
            if(iter != stData.end()) *iter = stReq.m_iBuffID;
            else stData.push_back(stReq.m_iBuffID);
        } else
        {
            stData.push_back(stReq.m_iBuffID);
        }

        if(stData.size() > CLogicConfigZhuHaiParser::E_MaxBuffSize)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
        }

        m_pUserInfo->m_stIBusyData.SetZhuHaiBuff(stData);

        SendToClient(SERVER_ERRCODE::SUCCESS);
        return (true);
    } else if ((int)m_usCmd == MSG_LOGIC_ZHUHAI_REFRESH_STORE)
    {
        CRequestZhuHaiRefreshStore stReq;
        if(DecodeMsgPack(stFormatData, stReq) == false) return (false);

        CResponseZhuHaiRefreshStore stRsp;
        if (stReq.m_bReset)
        {
            m_pUserInfo->m_stIBusyData.SetZhuHaiRefreshStore(0);
        } else
        {
            int32_t iRefreshCount = m_pUserInfo->m_stIBusyData.GetZhuHaiRefreshStore();
            auto refreshCountConfig = CLogicConfig::GetZhuHaiConfig().GetRefreshStore(iRefreshCount+1);
            if (refreshCountConfig == nullptr)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ZHUHAI_REFRESH_COUNT_LIMIT)
            }
            if (!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, refreshCountConfig->m_iConsumeItem, m_usCmd))
            {
                PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                    << "|MSG:" << CLogicError::GetErrMsg()
                                    << std::endl;

                SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
            }
            m_pUserInfo->m_stIBusyData.AddZhuHaiRefreshStore(1);
            stRsp.m_iCurRefreshCount = m_pUserInfo->m_stIBusyData.GetZhuHaiRefreshStore();
        }
        SendToClient(stRsp,SERVER_ERRCODE::SUCCESS);
    } else if ((int)m_usCmd == MSG_LOGIC_ZHUHAI_BUY_STORE)
    {
        CRequestZhuHaiBuyStore stReq;
        if(DecodeMsgPack(stFormatData, stReq) == false) return (false);

        //先扣钱之后根据不同类型操作
        auto storeConfig = CLogicConfig::GetZhuHaiConfig().GetStoreItem(stReq.m_iIndex,stReq.m_iStoreID);
        if (storeConfig == nullptr)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
        }
        if (!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, storeConfig->m_stCostItem, m_usCmd))
        {
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg() << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        if(storeConfig->m_iStoreType == EnumZhuHaiStore::ZhuHaiBuffBless)
        {
            if(stReq.m_iStoreID <= 0 || stReq.m_iReplaceBlessPos > CLogicConfigZhuHaiParser::E_MaxBlessGroup)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ZHUHAI_SET_BLESS_ERROR)
            }
            //替换祝福
            auto curBlessGroup = m_pUserInfo->m_stIBusyData.GetZhuHaiBlessGroup();
            //这里根据商品id获取到祝福id
            auto blessGroupInfo = curBlessGroup.find(stReq.m_iReplaceBlessPos);
            if (blessGroupInfo != curBlessGroup.end())
            {
                blessGroupInfo->second.m_stBlessGroup[1] = storeConfig->m_iGetBuffID;
            }
        } else if (storeConfig->m_iStoreType == EnumZhuHaiStore::ZhuHaiBuffBuff)
        {
            auto stData = m_pUserInfo->m_stIBusyData.GetZhuHaiBuff();

            if(stReq.m_iReplaceBuffID > 0)
            {
                auto iter = std::find(stData.begin(), stData.end(), stReq.m_iReplaceBuffID);
                if(iter != stData.end()) *iter = storeConfig->m_iGetBuffID;
                else stData.push_back(storeConfig->m_iGetBuffID);
            } else
            {
                stData.push_back(storeConfig->m_iGetBuffID);
            }

            if(stData.size() > CLogicConfigZhuHaiParser::E_MaxBuffSize)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
            }

            m_pUserInfo->m_stIBusyData.SetZhuHaiBuff(stData);

        } else if (storeConfig->m_iStoreType == EnumZhuHaiStore::ZhuHaiBuffRoleBuff)
        {
            auto roleBuffData = m_pUserInfo->m_stIBusyData.GetZhuHaiRoleBuff();

            auto iterCard = m_pUserInfo->m_stHeroCardTableMap.Find(stReq.m_iCardID);
            if (iterCard == m_pUserInfo->m_stHeroCardTableMap.End())
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HERO_CARD_IS_NOT_OWNED);
            }

            if(roleBuffData.find(stReq.m_iCardID) != roleBuffData.end())
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ZHUHAI_SET_BLESS_ERROR);
            }

            roleBuffData[stReq.m_iCardID].push_back(storeConfig->m_iGetBuffID);
            m_pUserInfo->m_stIBusyData.SetZhuHaiRoleBuff(roleBuffData);
        }

        CResponseZhuHaiBuyStore stRsp;
        stRsp.m_iStoreID = stReq.m_iStoreID;
        stRsp.m_iCardID = stReq.m_iCardID;
        stRsp.m_iIndex = stReq.m_iIndex;
        stRsp.m_iReplaceBlessPos = stReq.m_iReplaceBlessPos;
        stRsp.m_iReplaceBuffID = stReq.m_iReplaceBuffID;
        stRsp.m_stItemData.m_stConsume.m_stItems = storeConfig->m_stCostItem;

        SendToClient(stRsp,SERVER_ERRCODE::SUCCESS);
    }
    else if ((int)m_usCmd == MSG_LOGIC_ZHUHAI_SET_BLESS)
    {
        CRequestZhuHaiSetBless stReq;
        if(DecodeMsgPack(stFormatData, stReq) == false) return (false);

        if(stReq.m_iBlessID <= 0)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
        }

        auto stData = m_pUserInfo->m_stIBusyData.GetZhuHaiBlessGroup();
        if(stReq.m_iBlessID <= 0 || stReq.m_iPos > CLogicConfigZhuHaiParser::E_MaxBlessGroup)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ZHUHAI_SET_BLESS_ERROR)
        }

        auto iterSet = stData.find(stReq.m_iPos);
        if (iterSet == stData.end())
        {
            CPackZhuHaiBlessInfo blessInfo;
            blessInfo.m_stBlessGroup.push_back(stReq.m_iBlessID);
            blessInfo.m_stBlessGroup.push_back(stReq.m_iBackBlessID);
            stData[stReq.m_iPos] = blessInfo;
        } else {
            iterSet->second.m_stBlessGroup[0] = stReq.m_iBlessID;
            iterSet->second.m_stBlessGroup[1] = stReq.m_iBackBlessID;
        }

        m_pUserInfo->m_stIBusyData.SetZhuHaiBlessGroup(stData);
        SendToClient(SERVER_ERRCODE::SUCCESS);
        return (true);
    }
    else if ((int)m_usCmd == MSG_LOGIC_GET_LEVEL_BONUS)
    {
        CResponseGetLevelBonus stRsp;
        CRequestGetLevelBonus stParam;

        if(DecodeMsgPack(stFormatData, stParam) == false) return (false);

        auto stLevelIter = m_pUserInfo->m_stLevelInfoTableMap.Find(stParam.m_iLevelID);
        if (stLevelIter == m_pUserInfo->m_stLevelInfoTableMap.End() || stLevelIter->second->GetTotalVictoryTimes() <= 0)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::LEVEL_NOT_PASSED)
        }

        auto pConfig = CLogicConfig::GetLevelConfig().GetLevelConfigElem(stParam.m_iLevelID);
        if (nullptr == pConfig)
        {
            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        char cMin = 0;
        char cMax = char(pConfig->m_stAchieveBonus.size()) - 1;
        if(stParam.m_cIndex >= 0)
        {
            cMin = cMax = stParam.m_cIndex;
        }

        if(cMax >= char(pConfig->m_stAchieveBonus.size()))
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
        }

        int iNum = 0;
        auto cBonusTag = stLevelIter->second->GetBonusTag();
        for(; cMin <= cMax; ++cMin)
        {
            char cFlag = 1 << cMin;
            if(cBonusTag & cFlag)
            {
                SET_ERR_INFO(SERVER_ERRCODE::LEVEL_BONUS_HAS_RECEIVED);
                continue;
            }

            if(!(stLevelIter->second->GetAchieveTag() & cFlag))
            {
                SET_ERR_INFO(SERVER_ERRCODE::DAILY_TASK_NOT_COMPLETED);
                continue;
            }

            iNum += pConfig->m_stAchieveBonus[cMin];
            cBonusTag |= cFlag;
        }

        if(iNum <= 0)
        {
            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        CLogicCacheCommon::AddGameItem(m_pUserInfo, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_YuanBaoID, iNum, m_usCmd);

        stLevelIter->second->SetBonusTag(cBonusTag);

        stRsp.m_iLevelID = stParam.m_iLevelID;
        stRsp.m_cBonusTag = stLevelIter->second->GetBonusTag();
        stRsp.m_stItemData = m_pUserInfo->m_stCacheBonusData;
        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

        return (true);
    }

    return (true);
}


bool CLogicLevelProcessor::AntiCheatingCheck(const TLogicLevelElem* pConfig,const CPackLevelFightPara& stParam,int64_t* lPassDps,int64_t* lMaxDps)
{
    std::unordered_map<int32_t,int64_t> unorderedMapCardFight;
    for(const auto& cardStatus: stParam.m_stCardStatus)
    {
        auto cardConfig = CLogicConfig::GetHeroCardConfig().GetConfig(cardStatus.first);
        auto cardInfo = m_pUserInfo->m_stHeroCardTableMap.Find(cardStatus.first);
        if (cardConfig != nullptr && cardInfo != m_pUserInfo->m_stHeroCardTableMap.End())
        {
            int64_t lCardFight = 0;
            for(const auto& attr : cardConfig->m_stStarUpAttr[cardInfo->second->GetCardStar()].m_stAttrVec)
            {
                if (attr.first == CLogicConfigDefine::ECAT_Attack)
                {
                    lCardFight += attr.second;
                }
            }
            PROCESSOR_LOG_DEBUG << "Card_ID: " << cardStatus.first
                                << "Card_fight: " << lCardFight
                                << std::endl;
            unorderedMapCardFight.insert(make_pair(cardStatus.first,lCardFight));
        }
    }

    int32_t bossTime = 1;
    if (pConfig->m_iTimeLimit != 0 && pConfig->m_iTimeLimit < stParam.m_iBossCostTime)
    {
        bossTime = pConfig->m_iTimeLimit;
    } else if (stParam.m_iBossCostTime != 0 && stParam.m_iBossCostTime < pConfig->m_iTimeLimit)
    {
        bossTime = stParam.m_iBossCostTime;
    }


    int64_t lDps = stParam.m_ulPara == 0 ? pConfig->m_ulBossHp / bossTime : stParam.m_ulPara / bossTime;
    int64_t totalFight = 0;
    for(const auto& cardFight : unorderedMapCardFight)
    {
        totalFight += cardFight.second;
    }

    int64_t lMaxTotalFight = totalFight * CLogicConfig::GetAwardConfig().iAntiCheatingRatio;
    PROCESSOR_LOG_DEBUG << "lDps: " << lDps
                        << "bossTime: " << bossTime
                        << "lMaxTotalFight: " << lMaxTotalFight
                        << "clientTime: " << stParam.m_iBossCostTime
                        << "configLimitTime: " << pConfig->m_iTimeLimit
                        << "pConfigBossHp: " << pConfig->m_ulBossHp
                        << "clientDamage: " << stParam.m_ulPara
                        << std::endl;
    *lPassDps = lDps;
    *lMaxDps = lMaxTotalFight;

    if (lDps == 0) return false;
    return lDps > lMaxTotalFight;
}


bool CLogicLevelProcessor::IsRefreshCrushBoss(int32_t iLevelID,CPackCrushBossInfo& crushBossInfo)
{
    auto& gashaponConfig = CLogicConfig::GetGashaponConfig();
#if LOGIC_BUILD_PLATFORM != BUILD_PLATFORM_LOCAL
    if (!CLogicActive::IsGashaponOpen(m_pUserInfo))
    {
        return false;
    }
#endif

    //先根据当前的关卡出现概率决定是否出现，再决定出现的怪物星级。
    auto iterLevel = gashaponConfig.m_mapLeveID2Weight.find(iLevelID);
    if (gashaponConfig.m_mapLeveID2Weight.end() == iterLevel)
    {
        return false;
    }

    //决定是否刷新boss
    std::vector<std::pair<int32_t,int32_t>> pairWeight;
    pairWeight.emplace_back(1,iterLevel->second.m_iCrushBossPro);
    pairWeight.emplace_back(0,CLogicConfigDefine::ESD_RandomWeight - iterLevel->second.m_iCrushBossPro);

    int32_t isRefreshCrush = CLogicCacheCommon::RandStlCont(pairWeight);
    int32_t iTotalCount = m_pUserInfo->m_stIBusyData.GetCrushBossCount();
    if (0 == isRefreshCrush && iTotalCount < gashaponConfig.m_iCrushBossCount)
    {
        //没有刷新,记录次数保底
        m_pUserInfo->m_stIBusyData.AddCrushBossCount(1);
        return false;
    }

    //boss刷新品质
    pairWeight.clear();
    for(auto gradeWeight: iterLevel->second.m_uMapGradeWeight)
    {
        pairWeight.emplace_back(gradeWeight.first,gradeWeight.second);
    }

    int32_t iCrushBossGrade = CLogicCacheCommon::RandStlCont(pairWeight);

    auto bossLevelConfig = gashaponConfig.m_uMapBossInfo;
    auto iterBossLevel = bossLevelConfig.find(iCrushBossGrade);

    if (iterBossLevel == bossLevelConfig.end())
    {
        return false;
    }

    //确定最终boss关卡
    int32_t ret = CLogicCommon::GetRandNum(0,(int32_t)iterBossLevel->second.size());
    auto crushBossInfoConfig = iterBossLevel->second[ret];
    crushBossInfo.m_iLevelID = crushBossInfoConfig.m_iLevelID;
    crushBossInfo.m_ulMaxHp = crushBossInfoConfig.m_iBossHp;
    crushBossInfo.m_iGrade = crushBossInfoConfig.m_iGrade;

    //清空保底次数
    m_pUserInfo->m_stIBusyData.SetCrushBossCount(0);
    return true;
}


