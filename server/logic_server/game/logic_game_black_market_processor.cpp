#include "common_datetime.h"
#include "server_cmd_logic_define.h"
#include "server_errcode.h"
#include "server_item_protocol.h"
#include "logic_link_common.h"
#include "logic_config.h"
#include "logic_game_item.h"
#include "logic_game_cache_common.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_game_black_market_processor.h"
#include "logic_game_publisher.h"

REGISTER_CMD_FACTORY(MSG_LOGIC_TOUHOU_RANDOM_STORE_GET_INFO, CLogicBlackMarketProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_TOUHOU_RANDOM_STORE_REFRESH, CLogicBlackMarketProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_TOUHOU_RANDOM_STORE_BUY_ITEM, CLogicBlackMarketProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_TOUHOU_HEART_STORE_DIALOG, CLogicBlackMarketProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_TOUHOU_RANDOM_STORE_INFO_LIST, CLogicBlackMarketProcessor)

CLogicBlackMarketProcessor::CLogicBlackMarketProcessor(uint16_t usCmd, const std::string &strCmdName)
        : CLogicProcessorBase(usCmd, strCmdName)
{
}

bool CLogicBlackMarketProcessor::DoUserRun(const msgpack::unpacked& stFormatData)
{
    if((int)m_usCmd == MSG_LOGIC_TOUHOU_RANDOM_STORE_GET_INFO || (int)m_usCmd == MSG_LOGIC_TOUHOU_RANDOM_STORE_REFRESH)
    {
        CRequestRandomStoreGetInfo stReq;
        CResponseRandomStoreGetInfo stRsp;

        if(DecodeMsgPack(stFormatData, stReq) == false) return (false);

        const auto pConfig = CLogicConfig::GetRandomStoreConfig().GetStoreInfo(stReq.m_iStoreID);
        if(nullptr == pConfig)
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << COMMON_CONFIG_NOT_EXIST
                                << "|MSG:" << GET_ERRMSG_BY_ERRCODE(COMMON_CONFIG_NOT_EXIST)
                                << "|CMD:" << m_usCmd << std::endl;

            SEND_ERRCODE_AND_RET(COMMON_CONFIG_NOT_EXIST)
        }

        auto stRefreshData = m_pUserInfo->m_stIBusyData.GetRandomStoreRefreshData();
        auto& stStoreRefreshData = stRefreshData[stReq.m_iStoreID];

        // 刷新商店
        if (((int)m_usCmd == MSG_LOGIC_TOUHOU_RANDOM_STORE_GET_INFO && (stStoreRefreshData.first == 0 || is_pass_some_hour(stStoreRefreshData.first, CLogicCommon::GetSec(), pConfig->m_stFlashTime)))
            || (int)m_usCmd == MSG_LOGIC_TOUHOU_RANDOM_STORE_REFRESH)
        {
            if((int)m_usCmd == MSG_LOGIC_TOUHOU_RANDOM_STORE_REFRESH)
            {
                int iNowRefresheTimes = 1;
                if (!LOGIC_CHECK_DAY_REFRESH(stStoreRefreshData.first))
                    iNowRefresheTimes = stStoreRefreshData.second + 1;

                if(iNowRefresheTimes > pConfig->m_iRefreshMaxTimes || pConfig->m_stRefreshCost.empty())
                {
                    using namespace SERVER_ERRCODE;
                    PROCESSOR_LOG_ERROR << "ERRCODE:" << COUNT_NOT_ENOUGH
                                        << "|MSG:" << GET_ERRMSG_BY_ERRCODE(COUNT_NOT_ENOUGH)
                                        << "|CMD:" << m_usCmd << std::endl;

                    SEND_ERRCODE_AND_RET(COUNT_NOT_ENOUGH)
                }

                if(iNowRefresheTimes < 1) iNowRefresheTimes = 1;
                if (iNowRefresheTimes > (int)pConfig->m_stRefreshCost.size())
                    iNowRefresheTimes = pConfig->m_stRefreshCost.size();

                if (!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, pConfig->m_stRefreshCost[iNowRefresheTimes-1], m_usCmd))
                {
                    using namespace SERVER_ERRCODE;
                    PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                        << "|MSG:" << CLogicError::GetErrMsg()
                                        << std::endl;

                    SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
                }

                stStoreRefreshData.second = iNowRefresheTimes;
            }

            stStoreRefreshData.first = CLogicCommon::GetSec();

            m_pUserInfo->m_stIBusyData.SetRandomStoreRefreshData(stRefreshData);

            int32_t iLevel = m_pUserInfo->m_stUserInfoTable.GetLevel();
            int32_t iHeartLevel = m_pUserInfo->m_stIBusyData.GetTotalHeartLevel();
            auto stItemVec = pConfig->GetCommodityElem(iLevel, iHeartLevel);

            // 好感度商店随机隐性折扣位置
            std::vector<int> stVirtualIndex;
            if(pConfig->m_iStoreType == CLogicConfigDefine::ERST_HeartStore)
            {
                for(int i = 1; i <= int(stItemVec.size()); ++i)
                {
                    stVirtualIndex.push_back(i);
                }

                std::random_shuffle(stVirtualIndex.begin(), stVirtualIndex.end());
            }

            for (size_t i = 0; i < stItemVec.size(); ++i)
            {
                CPackMarket stElem;
                stElem.m_stCommodity = stItemVec[i].m_stCommodityItem;
                stElem.m_stConsume = stItemVec[i].m_stConsumeItem;
                stElem.m_iDiscount = stItemVec[i].m_iDiscount;

                // 好感度商店折扣
                if(pConfig->m_iStoreType == CLogicConfigDefine::ERST_HeartStore && i < stVirtualIndex.size())
                {
                    stElem.m_iDiscount = stVirtualIndex[i];
                }

                int iCommodityID = i + 1;

                auto& stRandomStoreMap = m_pUserInfo->m_stRandomStoreMap[stReq.m_iStoreID];
                auto stIter = stRandomStoreMap.Find(iCommodityID);
                if (stIter == stRandomStoreMap.End())
                {
                    TUserRandomStoreTableValueType stVal;
                    stVal.m_iUid = m_pUserInfo->m_stUserInfoTable.m_stData.m_iUid;
                    stVal.m_iGroupID = m_pUserInfo->m_stUserInfoTable.m_stData.m_iGroupID;
                    stVal.m_iStoreID = stReq.m_iStoreID;
                    stVal.m_iCommodityID = iCommodityID;
                    stVal.m_iCommodityItemType = stElem.m_stCommodity.m_iItemType;
                    stVal.m_iCommodityItemID = stElem.m_stCommodity.m_iCardID;
                    stVal.m_iCommodityItemNum = stElem.m_stCommodity.m_iNum;

                    stVal.m_iConsumeItemType = stElem.m_stConsume.m_iItemType;
                    stVal.m_iConsumeItemID = stElem.m_stConsume.m_iCardID;
                    stVal.m_iConsumeItemNum = stElem.m_stConsume.m_iNum;
                    stVal.m_iDiscount = stElem.m_iDiscount;
                    stVal.m_iLastBuyTime = 0;

                    stRandomStoreMap.Insert(std::make_pair(iCommodityID, stVal));
                }
                else
                {
                    stIter->second->SetCommodityItemType(stElem.m_stCommodity.m_iItemType);
                    stIter->second->SetCommodityItemID(stElem.m_stCommodity.m_iCardID);
                    stIter->second->SetCommodityItemNum(stElem.m_stCommodity.m_iNum);

                    stIter->second->SetConsumeItemType(stElem.m_stConsume.m_iItemType);
                    stIter->second->SetConsumeItemID(stElem.m_stConsume.m_iCardID);
                    stIter->second->SetConsumeItemNum(stElem.m_stConsume.m_iNum);

                    stIter->second->SetDiscount(stElem.m_iDiscount);
                    stIter->second->SetLastBuyTime(0);
                }

                stRsp.m_stStoreInfo.push_back(stElem);
            }
        }
        else
        {
            auto stIterBeg = m_pUserInfo->m_stRandomStoreMap[stReq.m_iStoreID].Begin();
            auto stIterEnd = m_pUserInfo->m_stRandomStoreMap[stReq.m_iStoreID].End();
            for ( ; stIterBeg != stIterEnd; ++stIterBeg)
            {
                CPackMarket stElem;
                stElem.m_stCommodity.m_iItemType = stIterBeg->second->GetCommodityItemType();
                stElem.m_stCommodity.m_iCardID = stIterBeg->second->GetCommodityItemID();
                stElem.m_stCommodity.m_iNum = stIterBeg->second->GetCommodityItemNum();
                stElem.m_stConsume.m_iItemType = stIterBeg->second->GetConsumeItemType();
                stElem.m_stConsume.m_iCardID = stIterBeg->second->GetConsumeItemID();
                stElem.m_stConsume.m_iNum = stIterBeg->second->GetConsumeItemNum();
                stElem.m_iDiscount = stIterBeg->second->GetDiscount();

                if (stIterBeg->second->GetLastBuyTime() != 0)
                    stElem.m_bCanBuy = false;

                stRsp.m_stStoreInfo.push_back(stElem);
            }
        }

        stRsp.m_iStoreID = stReq.m_iStoreID;
        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS" << std::endl;
    }
    else if((int)m_usCmd == MSG_LOGIC_TOUHOU_RANDOM_STORE_BUY_ITEM)
    {
        CRequestRandomStoreBuyItem stParam;
        CResponseRandomStoreBuyItem stRsp;

        if(DecodeMsgPack(stFormatData, stParam) == false) return (false);

        auto iter = m_pUserInfo->m_stRandomStoreMap.find(stParam.m_iStoreID);
        if(iter == m_pUserInfo->m_stRandomStoreMap.end())
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << PARAMETER_ERROR
                                << "|MSG:" << GET_ERRMSG_BY_ERRCODE(PARAMETER_ERROR)
                                << "|CMD:" << m_usCmd << std::endl;

            SEND_ERRCODE_AND_RET(PARAMETER_ERROR)
        }

        auto stIter = iter->second.Find(stParam.m_iCommodityID);
        if (stIter == iter->second.End())
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << UNKNOW_COMMODITY_ID << "|MSG:" <<  GET_ERRMSG_BY_ERRCODE(INTERN_ERROR)
                                << "|STORE_ID:" << stParam.m_iStoreID << "|COMMODITY_ID:" << stParam.m_iCommodityID << std::endl;

            SEND_ERRCODE_AND_RET(UNKNOW_COMMODITY_ID)
        }

        if (stIter->second->GetLastBuyTime() != 0)
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << NOT_ENOUGH_BUY_ITEM_TIMES << "|MSG:" <<  GET_ERRMSG_BY_ERRCODE(NOT_ENOUGH_BUY_ITEM_TIMES)
                                << "|STORE_ID:" << stParam.m_iStoreID << "|COMMODITY_ID:" << stParam.m_iCommodityID << std::endl;

            SEND_ERRCODE_AND_RET(NOT_ENOUGH_BUY_ITEM_TIMES)
        }

        const auto pConfig = CLogicConfig::GetRandomStoreConfig().GetStoreInfo(stParam.m_iStoreID);
        if(nullptr == pConfig)
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << COMMON_CONFIG_NOT_EXIST
                                << "|MSG:" << GET_ERRMSG_BY_ERRCODE(COMMON_CONFIG_NOT_EXIST)
                                << "|CMD:" << m_usCmd << std::endl;

            SEND_ERRCODE_AND_RET(COMMON_CONFIG_NOT_EXIST)
        }

        stRsp.m_stConsume.m_iItemType = stIter->second->GetConsumeItemType();
        stRsp.m_stConsume.m_iCardID = stIter->second->GetConsumeItemID();
        stRsp.m_stConsume.m_iNum = stIter->second->GetConsumeItemNum();

        // 如果是好感度商店，计算折扣
        if(pConfig->m_iStoreType == CLogicConfigDefine::ERST_HeartStore)
        {
            const auto pHeartStoreDiscount = CLogicConfig::GetHeartStoreDiscountConfig().GetConfig(m_pUserInfo->m_stIBusyData.GetTotalHeartLevel());
            if(nullptr == pHeartStoreDiscount)
            {
                using namespace SERVER_ERRCODE;
                PROCESSOR_LOG_ERROR << "ERRCODE:" << INTERN_ERROR
                                    << "|MSG:" << GET_ERRMSG_BY_ERRCODE(INTERN_ERROR)
                                    << "|CMD:" << m_usCmd << std::endl;

                SEND_ERRCODE_AND_RET(INTERN_ERROR)
            }

            auto iterDiscount = pHeartStoreDiscount->m_stDiscount.find(stParam.m_iStoreID);
            if(iterDiscount != pHeartStoreDiscount->m_stDiscount.end() &&
                stIter->second->GetDiscount() > 0 && stIter->second->GetDiscount() <= int(iterDiscount->second.size()))
            {
                int discount = iterDiscount->second[stIter->second->GetDiscount() - 1];
                stRsp.m_stConsume.m_iNum = std::floor(discount / 10.0 * stRsp.m_stConsume.m_iNum);
            }
        }

        if (false == CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, stRsp.m_stConsume, m_usCmd))
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode() << "|MSG:" << CLogicError::GetErrMsg()
                                << "|STORE_ID:" << stParam.m_iStoreID << "|COMMODITY_ID:" << stParam.m_iCommodityID << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        static std::string strBonus;

        CPackGameItem stBonusItem;

        stBonusItem.m_iItemType = stIter->second->GetCommodityItemType();
        stBonusItem.m_iCardID = stIter->second->GetCommodityItemID();
        stBonusItem.m_iNum = stIter->second->GetCommodityItemNum();

        stRsp.m_iStoreID = stParam.m_iStoreID;
        stRsp.m_iCommodityID = stParam.m_iCommodityID;
        stRsp.m_stCommodity = stBonusItem;

        CLogicCacheCommon::AddGameItem(m_pUserInfo, stBonusItem, strBonus, m_usCmd);

        stIter->second->SetLastBuyTime(CLogicCommon::GetSec());

        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|STORE_ID:" << stParam.m_iStoreID << "|COMMODITY_ID:" << stParam.m_iCommodityID << "|BONUS_ITEM:" << strBonus << std::endl;

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    }
    else if((int)m_usCmd == MSG_LOGIC_TOUHOU_RANDOM_STORE_INFO_LIST)
    {
        CRequestRandomStoreInfoList stReq;
        CResponseRandomStoreInfoList stRsp;

        if(DecodeMsgPack(stFormatData, stReq) == false) return (false);

        for(auto iID : stReq.m_stStoreIDList)
        {
            CRandomStoreInfoPack stInfo;
            if(false == GetRandomStoreInfo(iID, stInfo))
            {
                using namespace SERVER_ERRCODE;
                PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode() << "|MSG:" << CLogicError::GetErrMsg() << "|STORE_ID:" << iID << std::endl;

                SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
            }

            stRsp.m_stInfoList.push_back(stInfo);
        }

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    }
    else if((int)m_usCmd == MSG_LOGIC_TOUHOU_HEART_STORE_DIALOG)
    {
        /*
        CResponseHeartStoreDialog stRsp;

        stRsp.m_iHeroCardID = CLogicCacheCommon::GetMainRoleCardID(m_pUserInfo);
        auto iterMainCard = m_pUserInfo->m_stHeroCardTableMap.Find(stRsp.m_iHeroCardID);
        if(iterMainCard == m_pUserInfo->m_stHeroCardTableMap.End())
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << INTERN_ERROR
                                << "|MSG:" << GET_ERRMSG_BY_ERRCODE(INTERN_ERROR)
                                << "|CMD:" << m_usCmd << std::endl;

            SEND_ERRCODE_AND_RET(INTERN_ERROR)
        }

        int32_t iCurLevel = iterMainCard->second->GetHeartLevel();
        int32_t iCurExp = iterMainCard->second->GetHeartExp();

        // 先更新每日送礼物次数
        if(!is_same_day(CLogicCommon::GetSec(), iterMainCard->second->GetLastGiftTime()))
        {
            iterMainCard->second->SetLastGiftTime(CLogicCommon::GetSec());
            iterMainCard->second->SetDailyGiftNum(0);
        }

        if(iterMainCard->second->GetDailyGiftNum() >= CLogicConfig::GetHeartStoreDiscountConfig().m_iDailyDialogExpTimes)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COUNT_NOT_ENOUGH);
        }

        // 升级
        int32_t iNewLevel = iCurLevel;
        int32_t iNewExp = iCurExp + CLogicConfig::GetHeartStoreDiscountConfig().m_iDialogExp;
        CLogicConfig::GetHeartStoreDiscountConfig().ReCalcHeartLevelExp(iNewLevel, iNewExp);
        iterMainCard->second->SetHeartLevel(iNewLevel);
        iterMainCard->second->SetHeartExp(iNewExp);
        iterMainCard->second->AddDailyGiftNum(1);

        stRsp.m_iHeartLevel = iNewLevel;
        stRsp.m_iHeartExp = iNewExp;
        stRsp.m_iDailyGiftNum = iterMainCard->second->GetDailyGiftNum();
        stRsp.m_iLastGiftTime = iterMainCard->second->GetLastGiftTime();

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);

        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|HERO_CARD_ID:" << stRsp.m_iHeroCardID << "|BEFORE_HERO_HEART_LEVEL:" << iCurLevel
                            << "|BEFORE_HERO_HEART_EXP:" << iCurExp << "|AFTER_HERO_HEART_LEVEL:" << iNewLevel
                            << "|AFTER_HERO_HEART_EXP:" << iNewExp << std::endl;

         */
    }

    return (true);
}

bool CLogicBlackMarketProcessor::GetRandomStoreInfo(int32_t iStoreID, CRandomStoreInfoPack& stMarketInfo)
{
    const auto pConfig = CLogicConfig::GetRandomStoreConfig().GetStoreInfo(iStoreID);
    if(nullptr == pConfig)
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(COMMON_CONFIG_NOT_EXIST);
        return false;
    }

    stMarketInfo.m_iStoreID = iStoreID;
    stMarketInfo.m_bSoldOut = true;

    auto stRefreshData = m_pUserInfo->m_stIBusyData.GetRandomStoreRefreshData();
    auto& stStoreRefreshData = stRefreshData[iStoreID];

    // 刷新商店
    if (stStoreRefreshData.first == 0 || is_pass_some_hour(stStoreRefreshData.first, CLogicCommon::GetSec(), pConfig->m_stFlashTime))
    {
        stStoreRefreshData.first = CLogicCommon::GetSec();

        m_pUserInfo->m_stIBusyData.SetRandomStoreRefreshData(stRefreshData);

        int32_t iLevel = m_pUserInfo->m_stUserInfoTable.GetLevel();
        int32_t iHeartLevel = m_pUserInfo->m_stIBusyData.GetTotalHeartLevel();
        auto stItemVec = pConfig->GetCommodityElem(iLevel, iHeartLevel);

        // 好感度商店随机隐性折扣位置
        std::vector<int> stVirtualIndex;
        if(pConfig->m_iStoreType == CLogicConfigDefine::ERST_HeartStore)
        {
            for(int i = 1; i <= int(stItemVec.size()); ++i)
            {
                stVirtualIndex.push_back(i);
            }

            std::random_shuffle(stVirtualIndex.begin(), stVirtualIndex.end());
        }

        for (size_t i = 0; i < stItemVec.size(); ++i)
        {
            CPackMarket stMarket;
            stMarket.m_stCommodity = stItemVec[i].m_stCommodityItem;
            stMarket.m_stConsume = stItemVec[i].m_stConsumeItem;
            stMarket.m_iDiscount = stItemVec[i].m_iDiscount;

            // 好感度商店折扣
            if(pConfig->m_iStoreType == CLogicConfigDefine::ERST_HeartStore && i < stVirtualIndex.size())
            {
                stMarket.m_iDiscount = stVirtualIndex[i];
            }

            int iCommodityID = i + 1;

            auto& stRandomStoreMap = m_pUserInfo->m_stRandomStoreMap[iStoreID];
            auto stIter = stRandomStoreMap.Find(iCommodityID);
            if (stIter == stRandomStoreMap.End())
            {
                TUserRandomStoreTableValueType stVal;
                stVal.m_iUid = m_pUserInfo->m_stUserInfoTable.m_stData.m_iUid;
                stVal.m_iGroupID = m_pUserInfo->m_stUserInfoTable.m_stData.m_iGroupID;
                stVal.m_iStoreID = iStoreID;
                stVal.m_iCommodityID = iCommodityID;
                stVal.m_iCommodityItemType = stMarket.m_stCommodity.m_iItemType;
                stVal.m_iCommodityItemID = stMarket.m_stCommodity.m_iCardID;
                stVal.m_iCommodityItemNum = stMarket.m_stCommodity.m_iNum;

                stVal.m_iConsumeItemType = stMarket.m_stConsume.m_iItemType;
                stVal.m_iConsumeItemID = stMarket.m_stConsume.m_iCardID;
                stVal.m_iConsumeItemNum = stMarket.m_stConsume.m_iNum;
                stVal.m_iDiscount = stMarket.m_iDiscount;
                stVal.m_iLastBuyTime = 0;

                stRandomStoreMap.Insert(std::make_pair(iCommodityID, stVal));
            }
            else
            {
                stIter->second->SetCommodityItemType(stMarket.m_stCommodity.m_iItemType);
                stIter->second->SetCommodityItemID(stMarket.m_stCommodity.m_iCardID);
                stIter->second->SetCommodityItemNum(stMarket.m_stCommodity.m_iNum);

                stIter->second->SetConsumeItemType(stMarket.m_stConsume.m_iItemType);
                stIter->second->SetConsumeItemID(stMarket.m_stConsume.m_iCardID);
                stIter->second->SetConsumeItemNum(stMarket.m_stConsume.m_iNum);

                stIter->second->SetDiscount(stMarket.m_iDiscount);
                stIter->second->SetLastBuyTime(0);
            }

            stMarketInfo.m_stStoreInfo.push_back(stMarket);
        }

        stMarketInfo.m_bSoldOut = false;
    }
    else
    {
        auto stIterBeg = m_pUserInfo->m_stRandomStoreMap[iStoreID].Begin();
        auto stIterEnd = m_pUserInfo->m_stRandomStoreMap[iStoreID].End();
        for ( ; stIterBeg != stIterEnd; ++stIterBeg)
        {
            CPackMarket stMarket;
            stMarket.m_stCommodity.m_iItemType = stIterBeg->second->GetCommodityItemType();
            stMarket.m_stCommodity.m_iCardID = stIterBeg->second->GetCommodityItemID();
            stMarket.m_stCommodity.m_iNum = stIterBeg->second->GetCommodityItemNum();
            stMarket.m_stConsume.m_iItemType = stIterBeg->second->GetConsumeItemType();
            stMarket.m_stConsume.m_iCardID = stIterBeg->second->GetConsumeItemID();
            stMarket.m_stConsume.m_iNum = stIterBeg->second->GetConsumeItemNum();
            stMarket.m_iDiscount = stIterBeg->second->GetDiscount();

            if (stIterBeg->second->GetLastBuyTime() != 0)
                stMarket.m_bCanBuy = false;
            else
                stMarketInfo.m_bSoldOut = false;

            stMarketInfo.m_stStoreInfo.push_back(stMarket);
        }
    }

    return true;
}
