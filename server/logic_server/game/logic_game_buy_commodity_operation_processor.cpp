#include "server_cmd_logic_define.h"
#include "server_errcode.h"
#include "server_active_protocol.h"
#include "server_game_item_protocol.h"
#include "server_item_protocol.h"
#include "logic_link_common.h"
#include "logic_config.h"
#include "logic_game_item.h"
#include "logic_game_cache_common.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_game_buy_commodity_operation_processor.h"
#include "logic_game_publisher.h"
#include "logic_game_item_factory.h"
#include "logic_game_global.h"
#include "logic_game_common_publisher.h"
#include "logic_game_active.h"

REGISTER_CMD_FACTORY(MSG_LOGIC_BUY_VIP_BAG, CLogicBuyCommodityOperationProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_SELL_GAME_ITEM, CLogicBuyCommodityOperationProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_CHANGE_VARIABLE_SOUL, CLogicBuyCommodityOperationProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_COMMODITY_INFO, CLogicBuyCommodityOperationProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_BUY_COMMODITY, CLogicBuyCommodityOperationProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_COMMODITY_PT_EXCHANGE, CLogicBuyCommodityOperationProcessor)

CLogicBuyCommodityOperationProcessor::CLogicBuyCommodityOperationProcessor(uint16_t usCmd, const std::string &strCmdName)
        : CLogicProcessorBase(usCmd, strCmdName)
{
}


std::vector<TLogicRandBonusItem> CLogicBuyCommodityOperationProcessor::GetFixedCardRand(int32_t iCommodityID,bool useTenBaodi) const
{
    //先根据保底次数来计算随机出的星级，然后在根据包里的相对权重
    std::vector<TLogicRandBonusItem> cardItem;
    auto commodityConfig = CLogicConfig::GetMallConfig().GetConfig(iCommodityID);
    if (commodityConfig == nullptr)
    {
        return cardItem;
    }

    auto baodiTimesIter = m_pUserInfo->m_stUserBuyCommodityMap.Find(iCommodityID);
    if (baodiTimesIter == m_pUserInfo->m_stUserBuyCommodityMap.End())
    {
        return cardItem;
    }
    auto baodiTimes = baodiTimesIter->second->GetBaodiTime();
    //初始化权重对
    std::vector<std::pair<int32_t,int32_t>> gradeWeightCont;

    double iGradeTotalWeight = 0;
    std::unordered_map<int32_t,int32_t> useCardBag;
    if (useTenBaodi)
    {
        useCardBag = commodityConfig->m_initTenCardWeight;
    } else
    {
        useCardBag = commodityConfig->m_initCardWeight;
    }
    for(auto cardConfig: useCardBag)
    {
        iGradeTotalWeight += cardConfig.second;
    }

    for(auto cardConfig: useCardBag)
    {
        //开始权重修正
        auto fixedWeight = cardConfig.second;
        if (cardConfig.first == CLogicConfigDefine::ECGT_SSR)
        {
            fixedWeight += commodityConfig->m_iPerFixedWeight * (baodiTimes - commodityConfig->m_iStartFixed);
        } else
        {
            fixedWeight -= int32_t (commodityConfig->m_iPerFixedWeight * (baodiTimes - commodityConfig->m_iStartFixed) * cardConfig.second / iGradeTotalWeight);
        }
        PROCESSOR_LOG_DEBUG << "cardConfig.first: " << cardConfig.first
                            << "cardConfig.second: " << cardConfig.second
                            << std::endl;

        gradeWeightCont.emplace_back(std::make_pair(cardConfig.first,fixedWeight));
    }
    //获取星级
    int32_t chooseGrade = CLogicCacheCommon::RandStlCont(gradeWeightCont);
    PROCESSOR_LOG_DEBUG << "chooseGrade: " << chooseGrade
                        << std::endl;
    //再从星级中获取具体的card
    std::unordered_map<int32_t,TLogicCardbagConfigElem> stGradeConfig;
    if (useTenBaodi)
    {
        stGradeConfig = commodityConfig->m_iInitTenGradeCard;
    } else
    {
        stGradeConfig = commodityConfig->m_iInitGradeCard;
    }
    auto iterGradeCard = stGradeConfig.find(chooseGrade);
    if (iterGradeCard == stGradeConfig.end())
    {
        return cardItem;
    }
    int32_t iTotalWeight = iterGradeCard->second.m_iTotalWeight;
    PROCESSOR_LOG_DEBUG << "iTotalWeight: " << iTotalWeight
                        << std::endl;
    int iRandWeight = CLogicCommon::GetRandNum() % iTotalWeight;
    for(auto& stBagElem : iterGradeCard->second.m_stCardbagElem)
    {
        if (stBagElem.m_iWeight <= 0)
            continue;

        if (iRandWeight < stBagElem.m_iWeight)
        {
            cardItem.push_back(stBagElem);
            break;
        }
        iRandWeight -= stBagElem.m_iWeight;
    }

    return cardItem;
}

bool CLogicBuyCommodityOperationProcessor::DoUserRun(const msgpack::unpacked& stFormatData)
{
    if ((int)m_usCmd == MSG_LOGIC_GET_COMMODITY_INFO)
    {
        CResponseGetCommodityInfo stRsp;
        stRsp.m_iMallBaodiTimes = m_pUserInfo->m_stIBusyData.GetMallBaodiTimes();
        for(auto iter = m_pUserInfo->m_stUserBuyCommodityMap.Begin(); iter != m_pUserInfo->m_stUserBuyCommodityMap.End(); ++iter)
        {
            CPackCommodityInfo stInfo;
            stInfo.m_iCommodityID = iter->first;
            stInfo.m_iDailyLimitPayTimes = iter->second->GetDailyLimitPayTimes();
            stInfo.m_iLastPayTime = iter->second->GetLastPayTime();
            stInfo.m_iPt = iter->second->GetPt();
            stInfo.m_iTotalTimes = iter->second->GetTotalTimes();
            stInfo.m_iBaoDiTimes = iter->second->GetBaodiTime();
            stRsp.m_stInfoVec.push_back(stInfo);
        }

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
        return (true);
    }
    else if ((int)m_usCmd == MSG_LOGIC_BUY_COMMODITY)
    {
        CRequestBuyCommodity stReq;
        CResponseBuyCommodity stRsp;

        if(!DecodeMsgPack(stFormatData, stReq)) return (false);

        auto pConfig = CLogicConfig::GetMallConfig().GetConfig(stReq.m_iCommodityID);
        if(nullptr == pConfig)
        {
            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        if(pConfig->m_iAvyID > 0 && !CLogicActive::IsAvyOpen(pConfig->m_iAvyID, m_pUserInfo->m_stUserInfoTable.GetCreateTime()))
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_IS_NOT_OPEN)
        }

        auto iterUserData = m_pUserInfo->m_stUserBuyCommodityMap.Find(stReq.m_iCommodityID);
        if (iterUserData == m_pUserInfo->m_stUserBuyCommodityMap.End())
        {
            TUserBuyCommodityTableValueType stVal;
            stVal.m_iCommodityID = stReq.m_iCommodityID;
            stVal.m_iGroupID = m_iGroupID;
            stVal.m_iUid = m_iUin;

            auto stRet = m_pUserInfo->m_stUserBuyCommodityMap.Insert(std::make_pair(stReq.m_iCommodityID, stVal));
            if (!stRet.second)
            {
                using namespace SERVER_ERRCODE;
                PROCESSOR_LOG_ERROR << "ERROR:" << INTERN_ERROR
                                    << "|MSG:" << GET_ERRMSG_BY_ERRCODE(INTERN_ERROR)
                                    << "|COMMODITY_ID:" << stReq.m_iCommodityID
                                    << "|INSERT_INTO_CACHE_FAILD " << std::endl;

                SEND_ERRCODE_AND_RET(INTERN_ERROR)
            }

            iterUserData = stRet.first;
        }

        int iTimes = stReq.m_iBuyType == CRequestBuyCommodity::E_Ten ? CLogicConfigMallParser::E_TenMallNum : 1;
        // 总次数限制
        if(pConfig->m_iTotalTimes > 0 && iterUserData->second->GetTotalTimes() + iTimes > pConfig->m_iTotalTimes)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COUNT_NOT_ENOUGH)
        }

        // 如果首次免费
        if(iterUserData->second->GetTotalTimes() == 0 && pConfig->m_bFirstFree && stReq.m_iBuyType != CRequestBuyCommodity::E_Single)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
        }

        std::string strCardBag = pConfig->m_strCardBag;
        CPackGameItem stConsume;
        if(stReq.m_iBuyType == CRequestBuyCommodity::E_LimitPay)
        {
            if(pConfig->m_iDailyLimitPayRmb <= 0)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::SYSTEM_NOT_OPEN)
            }

            if(LOGIC_CHECK_DAY_REFRESH(iterUserData->second->GetLastPayTime()))
            {
                iterUserData->second->SetDailyLimitPayTimes(0);
                iterUserData->second->SetLastPayTime(CLogicCommon::GetSec());
            }

            if(iterUserData->second->GetDailyLimitPayTimes() >= pConfig->m_iDailyLimitPayTimes)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COUNT_NOT_ENOUGH)
            }

            stConsume.m_iItemType = CLogicConfigDefine::CURRENCY;
            stConsume.m_iCardID = CLogicConfigDefine::ECIT_PayYuanBaoID;
            stConsume.m_iNum = pConfig->m_iDailyLimitPayRmb;
        }
        else if(stReq.m_iBuyType == CRequestBuyCommodity::E_Single)
        {
            if(iterUserData->second->GetTotalTimes() == 0 && pConfig->m_bFirstFree)
            {
                if(!pConfig->m_strFirstBag.empty())
                    strCardBag = pConfig->m_strFirstBag;
            }
            else
            {
                if(stReq.m_bExchangeItem)
                {
                    if(pConfig->m_stSingleItem.m_iItemType <= 0)
                    {
                        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::SYSTEM_NOT_OPEN)
                    }

                    stConsume = pConfig->m_stSingleItem;
                }
                else
                {
                    if(pConfig->m_iSinglePrice <= 0)
                    {
                        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::SYSTEM_NOT_OPEN)
                    }

                    stConsume.m_iItemType = CLogicConfigDefine::CURRENCY;
                    if(pConfig->m_bIsOnlyPayRmb)
                    {
                        stConsume.m_iCardID = CLogicConfigDefine::ECIT_PayYuanBaoID;
                    }
                    else
                    {
                        stConsume.m_iCardID = CLogicConfigDefine::ECIT_YuanBaoID;
                    }

                    stConsume.m_iNum = pConfig->m_iSinglePrice;
                }
            }
        }
        else if(stReq.m_iBuyType == CRequestBuyCommodity::E_Ten)
        {
            if(stReq.m_bExchangeItem)
            {
                if(pConfig->m_stTenItem.size() <= 0)
                {
                    SEND_ERRCODE_AND_RET(SERVER_ERRCODE::SYSTEM_NOT_OPEN)
                }

                if (stReq.m_iConsumeIndex >= int32_t(pConfig->m_stTenItem.size()))
                {
                    SEND_ERRCODE_AND_RET(SERVER_ERRCODE::POS_ERROR)
                }
                stConsume = pConfig->m_stTenItem[stReq.m_iConsumeIndex];
            }
            else
            {
                if(pConfig->m_iTenPrice <= 0)
                {
                    SEND_ERRCODE_AND_RET(SERVER_ERRCODE::SYSTEM_NOT_OPEN)
                }

                stConsume.m_iItemType = CLogicConfigDefine::CURRENCY;
                if(pConfig->m_bIsOnlyPayRmb)
                {
                    stConsume.m_iCardID = CLogicConfigDefine::ECIT_PayYuanBaoID;
                }
                else
                {
                    stConsume.m_iCardID = CLogicConfigDefine::ECIT_YuanBaoID;
                }

                stConsume.m_iNum = pConfig->m_iTenPrice;
            }
        }
        else
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
        }

        if(stConsume.m_iNum > 0 && !CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, stConsume, m_usCmd))
        {
            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        auto& stMallConfig = CLogicConfig::GetMallConfig();
        int iHandleNum = iTimes;
        int iGlobalTimes = m_pUserInfo->m_stIBusyData.GetMallBaodiTimes();
        bool bTenReach = false;
        bool bUseTenBaodi = false;
        bool bGlobalReach = (iGlobalTimes >= stMallConfig.m_iBaodiTimes);
        std::map<int32_t, int32_t> stCardGradeNum;
        std::map<int32_t, int32_t> stCardSameNum;

        while (iHandleNum--)
        {
            PROCESSOR_LOG_DEBUG << "start commodity: " << strCardBag
                                << std::endl;
            // 十连保底
            if(iHandleNum == 0 && stReq.m_iBuyType == CRequestBuyCommodity::E_Ten && !bTenReach)
            {
                strCardBag = pConfig->m_strTenBaodiBag;
                bUseTenBaodi = true;
                PROCESSOR_LOG_DEBUG << "ten baodi!|bag:" << strCardBag << std::endl;
            }

            // 全局保底
            if(!bGlobalReach && stReq.m_iCommodityID == stMallConfig.m_iExcludeID)
            {
                ++iGlobalTimes;
                if(iGlobalTimes == stMallConfig.m_iBaodiTimes)
                {
                    strCardBag = stMallConfig.m_strBaodiBag;
                    bGlobalReach = true;

                    PROCESSOR_LOG_DEBUG << "global baodi!|bag:" << strCardBag << "|global_times:" << iGlobalTimes << "|handle_num:" << iHandleNum << std::endl;
                }
            }

            // 抽卡(一种是不需要修正的卡池，一种是需要动态调整概率的卡池)
            bool hasSSR = false;
            std::vector<TLogicRandBonusItem> stRandBonusVec;
            if (pConfig->m_iStartFixed != 0 && iterUserData->second->GetBaodiTime() >= pConfig->m_iStartFixed)
            {
                stRandBonusVec = GetFixedCardRand(stReq.m_iCommodityID,bUseTenBaodi);
            }
            else
            {
                stRandBonusVec = CLogicConfig::GetCardbagConfig().GetRandBonusByName(strCardBag);
            }
            for (auto rand : stRandBonusVec)
            {
                stRsp.m_stReward.emplace_back(rand.m_iItemType, rand.m_iCardID, rand.m_iNum);

                if(rand.m_iItemType != CLogicConfigDefine::HERO_CARD)
                    continue;

                auto pCardConfig = CLogicConfig::GetHeroCardConfig().GetConfig(rand.m_iCardID);
                if(nullptr == pCardConfig)
                {
                    PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                        << "|MSG:" << CLogicError::GetErrMsg()
                                        << "|CARD_ID:" << rand.m_iCardID << std::endl;

                    continue;
                }

                // 十连保底记录
                if(pCardConfig->m_iInitGrade >= pConfig->m_iRecordGrade)
                    bTenReach = true;

                // 全局保底记录
                if(!bGlobalReach && stReq.m_iCommodityID == stMallConfig.m_iExcludeID)
                {
                    if(pCardConfig->m_iInitGrade >= stMallConfig.m_iRecordGrade)
                        bGlobalReach = true;
                }

                if(pCardConfig->m_iInitGrade >= CLogicConfigDefine::ECGT_SR)
                {
                    ++stCardGradeNum[pCardConfig->m_iInitGrade];
                }
                //是否有SSR(三星角色)
                if (pCardConfig->m_iInitStar == CLogicConfigDefine::ECGT_SSR)
                {
                    hasSSR = true;
                }

                ++stCardSameNum[pCardConfig->m_iHeroCardID];
            }

            strCardBag = pConfig->m_strCardBag;
            if (pConfig->m_iStartFixed != 0)
            {
                if(hasSSR)
                {
                    iterUserData->second->SetBaodiTime(0);

                } else
                {
                    iterUserData->second->AddBaodiTime(1);
                }
            }
        }

        // 保存全局保底进度
        if(bGlobalReach)
            m_pUserInfo->m_stIBusyData.SetMallBaodiTimes(stMallConfig.m_iBaodiTimes);
        else
            m_pUserInfo->m_stIBusyData.SetMallBaodiTimes(iGlobalTimes);

        static std::string strBonusItem;
        CLogicCacheCommon::AddGameItem(m_pUserInfo, stRsp.m_stReward, strBonusItem, m_usCmd);

        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|COMMODITY_ID:" << stReq.m_iCommodityID << "|BONUS_ITEM:" << strBonusItem << std::endl;

        if(stReq.m_iBuyType == CRequestBuyCommodity::E_LimitPay)
        {
            iterUserData->second->AddDailyLimitPayTimes(1);
        }

        iterUserData->second->AddTotalTimes(iTimes);
        //没有配置pt点兑换就不需要加pt点数
        if(!pConfig->m_stPtCardExchangeMap.empty())
        {
            iterUserData->second->AddPt(iTimes);
        }

        //记录最近一次抽的限时卡池
        if(pConfig->m_iAvyID > 0 && CLogicActive::IsAvyOpen(pConfig->m_iAvyID, m_pUserInfo->m_stUserInfoTable.GetCreateTime()))
        {
            PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|COMMODITY_ID:" << stReq.m_iCommodityID << "|BONUS_ITEM:" << strBonusItem << std::endl;
            auto iLastID = m_pUserInfo->m_stUserInfoTable.GetLastLimitCommodityID();
            if (iLastID == 0 || iLastID != stReq.m_iCommodityID)
            {
                m_pUserInfo->m_stUserInfoTable.SetLastLimitCommodityID(stReq.m_iCommodityID);
            }
        }

        stRsp.m_stCommodityInfo.m_iCommodityID = stReq.m_iCommodityID;
        stRsp.m_stCommodityInfo.m_iDailyLimitPayTimes = iterUserData->second->GetDailyLimitPayTimes();
        stRsp.m_stCommodityInfo.m_iLastPayTime = iterUserData->second->GetLastPayTime();
        stRsp.m_stCommodityInfo.m_iPt = iterUserData->second->GetPt();
        stRsp.m_stCommodityInfo.m_iBaoDiTimes = iterUserData->second->GetBaodiTime();
        stRsp.m_stCommodityInfo.m_iTotalTimes = iterUserData->second->GetTotalTimes();
        stRsp.m_stItemData = m_pUserInfo->m_stCacheBonusData;
        stRsp.m_iMallBaodiTimes = m_pUserInfo->m_stIBusyData.GetMallBaodiTimes();
        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

        // 广播
        CLogicGlobal::PublishBuyMallBroadcast(stReq.m_iCommodityID, m_pUserInfo->m_strNick, stRsp.m_stReward);

        for(auto& iter : stCardGradeNum)
        {
            // 通用事件
            CLogicEventData stEventNoGrade;
            stEventNoGrade.m_eType = CLogicConfigDefine::ECET_UninterruptedCardCommodityNoGrade;
            stEventNoGrade.m_iPara = iter.first;
            stEventNoGrade.m_iCount = iter.second;
            stEventNoGrade.m_iCmd = m_usCmd;
            LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventNoGrade);

            if(iter.second < 2) continue;

            // 通用事件
            CLogicEventData stEventData;
            stEventData.m_eType = CLogicConfigDefine::ECET_CardCommodityGradeNum;
            stEventData.m_iPara = iter.first;
            stEventData.m_iCount = iter.second;
            stEventData.m_iCmd = m_usCmd;
            LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);
        }

        for(auto& iter : stCardSameNum)
        {
            if(iter.second < 3) continue;

            // 通用事件
            CLogicEventData stEventData;
            stEventData.m_eType = CLogicConfigDefine::ECET_CardCommoditySameCardNum;
            stEventData.m_iCount = iter.second;
            stEventData.m_iCmd = m_usCmd;
            LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);
        }

        // 通用事件
        CLogicEventData stEventNumData;
        stEventNumData.m_eType = CLogicConfigDefine::ECET_CardCommodityNum;
        stEventNumData.m_iTimes = iTimes;
        stEventNumData.m_iCmd = m_usCmd;
        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventNumData);

        return (true);
    }
    else if ((int)m_usCmd == MSG_LOGIC_COMMODITY_PT_EXCHANGE)
    {
        CRequestCommodityPtExchange stReq;
        CResponseCommodityPtExchange stRsp;

        if(!DecodeMsgPack(stFormatData, stReq)) return (false);

        auto pConfig = CLogicConfig::GetMallConfig().GetConfig(stReq.m_iCommodityID);
        if(nullptr == pConfig)
        {
            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode());
        }

        if(pConfig->m_iAvyID > 0 && !CLogicActive::IsAvyOpen(pConfig->m_iAvyID, m_pUserInfo->m_stUserInfoTable.GetCreateTime()))
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_IS_NOT_OPEN);
        }

        auto iterPtConfig = pConfig->m_stPtCardExchangeMap.find(stReq.m_iCardID);
        if(iterPtConfig == pConfig->m_stPtCardExchangeMap.end())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
        }

        auto iterUserData = m_pUserInfo->m_stUserBuyCommodityMap.Find(stReq.m_iCommodityID);
        if (iterUserData == m_pUserInfo->m_stUserBuyCommodityMap.End() || iterUserData->second->GetPt() < iterPtConfig->second)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ITEM);
        }

        stRsp.m_stReward.m_iItemType = CLogicConfigDefine::HERO_CARD;
        stRsp.m_stReward.m_iCardID = stReq.m_iCardID;
        stRsp.m_stReward.m_iNum = 1;

        iterUserData->second->AddPt(-iterPtConfig->second);

        static std::string strBonusItem;
        CLogicCacheCommon::AddGameItem(m_pUserInfo, stRsp.m_stReward, strBonusItem, m_usCmd);

        stRsp.m_iCommodityID = stReq.m_iCommodityID;
        stRsp.m_iPt = iterUserData->second->GetPt();
        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|COMMODITY_ID:" << stReq.m_iCommodityID << "|BONUS_ITEM:" << strBonusItem << std::endl;

        return (true);
    }
    else if ((int)m_usCmd == MSG_LOGIC_BUY_VIP_BAG)
    {
        CResponseBuyVipBag stRsp;
        CRequestBuyVipBag stParam;

        if(DecodeMsgPack(stFormatData, stParam) == false) return (false);
        int iVipLevel = m_pUserInfo->m_stUserInfoTable.GetVipLevel();
        if (iVipLevel < stParam.m_iVipLevel)
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << NOT_ENOUGH_VIP_LEVEL
                                << "|MSG:" << GET_ERRMSG_BY_ERRCODE(NOT_ENOUGH_VIP_LEVEL)
                                << "|NEED_VIP:" << stParam.m_iVipLevel 
                                << "|CURRENT_VIP:" << iVipLevel << std::endl;

            SEND_ERRCODE_AND_RET(NOT_ENOUGH_VIP_LEVEL)
        }

        int iVipLevelBuyBonus = m_pUserInfo->m_stUserInfoTable.GetVipLevelBuyBonus();
        if (iVipLevelBuyBonus & (1 << stParam.m_iVipLevel))
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << VIP_BAG_HAS_BEEN_BUY
                                << "|MSG:" << GET_ERRMSG_BY_ERRCODE(VIP_BAG_HAS_BEEN_BUY)
                                << std::endl;

            SEND_ERRCODE_AND_RET(VIP_BAG_HAS_BEEN_BUY)
        }

        auto pstVipBagElem = CLogicConfig::GetVipBagConfig().GetVipBagConfigElem(stParam.m_iVipLevel);
        if (!pstVipBagElem)
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg()
                                << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        auto stConsume = LOGIC_GAME_ITEM_FACTORY_INS->CreateGameItem(
                (CLogicConfigDefine::GAME_ITEM_COLLECTION) CLogicConfigDefine::CURRENCY,
                                                             CLogicConfigDefine::ECIT_YuanBaoID,
                                                             pstVipBagElem->m_iPrice,
															m_usCmd,
                                                             m_pUserInfo);

        if (!stConsume.get() || !stConsume->CanConsume())
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg()
                                << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        stConsume->Consume();
        iVipLevelBuyBonus |= (1 << stParam.m_iVipLevel);
        m_pUserInfo->m_stUserInfoTable.SetVipLevelBuyBonus(iVipLevelBuyBonus);

        static std::string strDailyTaskBonus;
        CLogicCacheCommon::AddGameItem(m_pUserInfo, pstVipBagElem->m_stBonusList, strDailyTaskBonus, m_usCmd);

        stRsp.m_strVipBagBitmap = int2bin(iVipLevelBuyBonus);
        stRsp.m_stItemList = pstVipBagElem->m_stBonusList;

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
        return (true);
    }
    else if ((int)m_usCmd == MSG_LOGIC_SELL_GAME_ITEM)
    {
        CResponseSellGameItem stRsp;
        CRequestSellGameItem stParam;

        if (DecodeMsgPack(stFormatData, stParam) == false) return (false);

        if (stParam.m_stItem.m_iNum <= 0 && stParam.m_stFishIndex.empty())
        {
            using namespace SERVER_ERRCODE;
            SEND_ERRCODE_AND_RET(PARAMETER_ERROR)
        }

        if(stParam.m_stItem.m_iNum > 0)
        {
            if (!CLogicCacheCommon::SellGameItem(m_pUserInfo,
                                                 stParam.m_stItem.m_iItemType, stParam.m_stItem.m_iCardID,
                                                 stParam.m_stItem.m_iNum, m_usCmd))
            {
                PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                    << "|MSG:" << CLogicError::GetErrMsg()
                                    << std::endl;

                SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
            }
        }
        else
        {
            if(stParam.m_stFishIndex.size() > m_pUserInfo->m_stFishItemMap.Size())
            {
                using namespace SERVER_ERRCODE;
                SEND_ERRCODE_AND_RET(PARAMETER_ERROR)
            }

            int iGoldNum = 0;
            for(auto fish_index : stParam.m_stFishIndex)
            {
                auto iterFish = m_pUserInfo->m_stFishItemMap.Find(fish_index);
                if(iterFish == m_pUserInfo->m_stFishItemMap.End())
                    continue;

                int iPrice = CLogicConfig::GetHomeFuncConfig().GetFishSellPrice(iterFish->second->GetFishID(), iterFish->second->GetGrade());
                if(iPrice <= 0)
                    continue;

                iGoldNum += iPrice;
                m_pUserInfo->m_stFishItemMap.Erase(iterFish);
                stRsp.m_stFishIndex.push_back(fish_index);
            }

            CLogicCacheCommon::AddGameItem(m_pUserInfo, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_GoldID, iGoldNum, m_usCmd);
        }

        stRsp.m_stItemData = m_pUserInfo->m_stCacheBonusData;
        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

        return (true);
    }
    else if ((int)m_usCmd == MSG_LOGIC_CHANGE_VARIABLE_SOUL)
    {
        CResponseChangeVariableSoul stRsp;
        CRequestChangeVariableSoul stParam;

        if (DecodeMsgPack(stFormatData, stParam) == false) return (false);

        if (stParam.m_iNum <= 0)
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << PARAMETER_ERROR
                                << "|MSG:" << GET_ERRMSG_BY_ERRCODE(PARAMETER_ERROR)
                                << "|NUM:" << stParam.m_iNum 
                                << std::endl;

            SEND_ERRCODE_AND_RET(PARAMETER_ERROR)
        }

        auto stCardIter = m_pUserInfo->m_stHeroCardTableMap.Find(stParam.m_iCardID);
        if (stCardIter == m_pUserInfo->m_stHeroCardTableMap.End())
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << HERO_CARD_IS_NOT_OWNED
                                << "|MSG:" << GET_ERRMSG_BY_ERRCODE(HERO_CARD_IS_NOT_OWNED)
                                << "|CARD_ID:" << stParam.m_iCardID << std::endl;
        
            SEND_ERRCODE_AND_RET(HERO_CARD_IS_NOT_OWNED)
        }

        auto pConfig = CLogicConfig::GetHeroCardConfig().GetConfig(stParam.m_iCardID);
        if(nullptr == pConfig)
        {
            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        if (stCardIter->second->GetCardLevel() <= 0)
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << HERO_CARD_IS_NOT_OWNED
                                << "|MSG:" << GET_ERRMSG_BY_ERRCODE(HERO_CARD_IS_NOT_OWNED)
                                << "|CARD_ID:" << stParam.m_iCardID << std::endl;
        
            SEND_ERRCODE_AND_RET(HERO_CARD_IS_NOT_OWNED)
        }

        // auto stVariableSoul = LOGIC_GAME_ITEM_FACTORY_CREATE_VARIABLE_SOUL(stParam.m_iNum, m_usCmd, m_pUserInfo);
        // if (!stVariableSoul.get() || !stVariableSoul->CanConsume())
        {
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg()
                                << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }
        // stVariableSoul->Consume();

        CPackGameItem stCardSoul;
        stCardSoul.m_iItemType = CLogicConfigDefine::EXCHANGE_ITEM;
        stCardSoul.m_iCardID = pConfig->m_iSoulID;
        stCardSoul.m_iNum = stParam.m_iNum;

        CLogicCacheCommon::AddGameItem(m_pUserInfo, stCardSoul, m_usCmd);

        stRsp.m_iCardID = stParam.m_iCardID;
        stRsp.m_iNum = stParam.m_iNum;

        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|CARD_ID:" << stParam.m_iCardID 
                            << "|NUM:" << stParam.m_iNum << std::endl;

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

        return (true);
    }

    return (true);
}
