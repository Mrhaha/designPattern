#include "server_cmd_logic_define.h"
#include "server_errcode.h"
#include "server_game_item_protocol.h"
#include "server_item_protocol.h"
#include "server_role_protocol.h"
#include "logic_link_common.h"
#include "logic_game_buy_resource_processor.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_config.h"
#include "logic_game_item.h"
#include "logic_game_cache_common.h"
#include "logic_game_cache_level_operation.h"
#include "logic_game_user_stat.h"
#include "logic_game_user_notify_system.h"
#include "logic_game_active.h"
#include "logic_game_common_publisher.h"
#include "logic_record_global_store_reward_table.h"
#include "logic_game_global.h"
#include "common_datetime.h"

REGISTER_CMD_FACTORY(MSG_LOGIC_BUY_GOLD, CLogicBuyResourceProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_BUY_ENERGY, CLogicBuyResourceProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_BUY_VITALITY, CLogicBuyResourceProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_SEQUENTIAL_BUY_GOLD, CLogicBuyResourceProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_REFRESH_LEVEL_TIMES, CLogicBuyResourceProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_REFRESH_PVP_FIGHT_TIMES, CLogicBuyResourceProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_REFRESH_CROSS_FIGHT_RESET_TIMES, CLogicBuyResourceProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_USER_DEPOSIT_INFO, CLogicBuyResourceProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_BUY_REFRESH_TOKEN, CLogicBuyResourceProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_SHOP_ITEM, CLogicBuyResourceProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_REFRESH_CONSTEL_SHOP, CLogicBuyResourceProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_GET_CONSTEL_SHOP_INFO, CLogicBuyResourceProcessor)






CLogicBuyResourceProcessor::CLogicBuyResourceProcessor(uint16_t usCmd, const std::string &strCmdName)
        : CLogicProcessorBase(usCmd, strCmdName)
{
}


bool 
CLogicBuyResourceProcessor::DoUserRun(const msgpack::unpacked& stFormatData)
{
    if (false == CheckVipLevel(m_usCmd))
    {
        using namespace SERVER_ERRCODE;
        PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                            << "|MSG:" << CLogicError::GetErrMsg() << std::endl;

        SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        return false;
    }

    if ((int)m_usCmd == MSG_LOGIC_BUY_GOLD ||
        (int)m_usCmd == MSG_LOGIC_BUY_ENERGY ||
        (int)m_usCmd == MSG_LOGIC_BUY_VITALITY)
    {
        int iVip = m_pUserInfo->m_stUserInfoTable.GetVipLevel();
        auto pstVipInfo = CLogicConfig::GetVipConfig().GetVipInfo(iVip);
        if (!pstVipInfo)
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg()
                                << "|VIP:" << iVip << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        int iBuyTimes = 1;
        int iCanBuyTimes = 0;
        const TLogicBuyResourceElem* pstBuyResourceElem = nullptr;
        if ((int)m_usCmd == MSG_LOGIC_BUY_GOLD)
        {
            iBuyTimes = m_pUserInfo->m_stUserInfoTable.GetBuyGoldTimes() + 1;

            pstBuyResourceElem = CLogicConfig::GetBuyResourceConfig().GetBuyGoldConfig(iBuyTimes);

            iCanBuyTimes = pstVipInfo->m_iCanBuyGoldTimes;
        }
        else if ((int)m_usCmd == MSG_LOGIC_BUY_ENERGY)
        {
            iBuyTimes = m_pUserInfo->m_stUserInfoTable.GetBuyEnergyTimes() + 1;

            pstBuyResourceElem = CLogicConfig::GetBuyResourceConfig().GetBuyEnergyConfig(iBuyTimes);

            iCanBuyTimes = pstVipInfo->m_iCanBuyEnergyTimes;
        }
        else if ((int)m_usCmd == MSG_LOGIC_BUY_VITALITY)
        {
            if (!LOGIC_CHECK_DAY_REFRESH(m_pUserInfo->m_stUserInfoTable.GetLastBuyVitalityTime()))
                iBuyTimes = m_pUserInfo->m_stUserInfoTable.GetBuyVitalityTimes() + 1;

            pstBuyResourceElem = CLogicConfig::GetBuyResourceConfig().GetBuyVitalityConfig(iBuyTimes);

            iCanBuyTimes = pstVipInfo->m_iCanBuyVitalityTimes;
        }

        if (nullptr == pstBuyResourceElem)
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg()
                                << "|BUY_TIMES:" << iBuyTimes << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        if (iBuyTimes > iCanBuyTimes)
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << NOT_ENOUGH_BUY_RESOURCE_TIMES
                                << "|MSG:" << GET_ERRMSG_BY_ERRCODE(NOT_ENOUGH_BUY_RESOURCE_TIMES)
                                << "|CAN_BUY_TIMES:" << iCanBuyTimes << "|BUY_TIMES:" << iBuyTimes
                                << std::endl;

            SEND_ERRCODE_AND_RET(NOT_ENOUGH_BUY_RESOURCE_TIMES);
        }

        if ((int)m_usCmd == MSG_LOGIC_BUY_GOLD)
        {
            int iRate = CLogicConfig::GetVipConfig().GetShakeGoldRate(iVip);
            if (-1 == iRate)
            {
                using namespace SERVER_ERRCODE;
                PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                    << "|MSG:" << CLogicError::GetErrMsg() << "|VIP:" << iVip << std::endl;

                SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
            }

            int iBaseGold = CLogicConfig::GetMoneyTreeExpConfig().GetBaseMoney(m_pUserInfo->m_stUserInfoTable.GetBuyGoldTotalTimes() * 10);
            if (!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, pstBuyResourceElem->m_stConsumeItem.m_iItemType, pstBuyResourceElem->m_stConsumeItem.m_iCardID, pstBuyResourceElem->m_stConsumeItem.m_iNum, m_usCmd))
            {
                using namespace SERVER_ERRCODE;
                PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                    << "|MSG:" << CLogicError::GetErrMsg() << std::endl;

                SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
            }

			if (iRate < 2)
			{
				iRate = 2;
			}

            CResponseBuyGold stRsp;

            stRsp.m_iRate = iRate;
            stRsp.m_stConsumeItem = m_pUserInfo->m_stCacheBonusData.m_stConsume.m_stItems[0];
            stRsp.m_stGetItem.m_iItemType = CLogicConfigDefine::CURRENCY;
            stRsp.m_stGetItem.m_iCardID = CLogicConfigDefine::ECIT_GoldID;
            stRsp.m_stGetItem.m_iNum = iBaseGold * iRate;

            std::string strAddItem;
            CLogicCacheCommon::AddGameItem(m_pUserInfo, stRsp.m_stGetItem, strAddItem, m_usCmd);

            m_pUserInfo->m_stUserInfoTable.SetBuyGoldTimes(iBuyTimes);
            m_pUserInfo->m_stUserInfoTable.AddBuyGoldTotalTimes(1);

            logic_game_user_action_channel_data_t stData;
            stData.m_iTimes = 1;
            stData.m_iAction = CLogicGameChannelEnum::ACTION_USE_SHAKE_GOLD;
            LOGIC_PUBLISHER_INS->PublishGameUserActionInfo(m_pUserInfo, stData);

            PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|BUY_TIMES:" << iBuyTimes << "|VIP:" << iVip 
                                << "|CONSUME_ITEM:[" << stRsp.m_stConsumeItem.m_iItemType << "," << stRsp.m_stConsumeItem.m_iCardID
                                << "," << stRsp.m_stConsumeItem.m_iNum << "]|RATE:" << iRate << "|GET_ITEM:" << strAddItem << std::endl;

            SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
        }
        else
        {
            CResponseBuyResource stRsp;
            stRsp.m_stConsumeItem = pstBuyResourceElem->m_stConsumeItem;
            if (!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, stRsp.m_stConsumeItem, m_usCmd))
            {
                using namespace SERVER_ERRCODE;
                PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                    << "|MSG:" << CLogicError::GetErrMsg() << std::endl;

                SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
            }

            stRsp.m_stGetItem = pstBuyResourceElem->m_stGetItem;
            stRsp.m_stConsumeItem = m_pUserInfo->m_stCacheBonusData.m_stConsume.m_stItems[0];
            std::string strAddItem;
            CLogicCacheCommon::AddGameItem(m_pUserInfo, stRsp.m_stGetItem, strAddItem, m_usCmd);

            if ((int)m_usCmd == MSG_LOGIC_BUY_ENERGY)
            {
                m_pUserInfo->m_stUserInfoTable.SetBuyEnergyTimes(iBuyTimes);

                CLogicGameUserActionChannel::data_type stChannelData;
                stChannelData.m_iTimes = 1;
                stChannelData.m_iAction = CLogicGameChannelEnum::ACTION_BUY_ENERGY;
                LOGIC_PUBLISHER_INS->PublishGameUserActionInfo(m_pUserInfo, stChannelData);

                // 通用事件
                CLogicEventData stEventData;
                stEventData.m_eType = CLogicConfigDefine::ECET_BuyEnergyTimes;
                stEventData.m_iCmd = m_usCmd;
                LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventData);
            }
            else if ((int)m_usCmd == MSG_LOGIC_BUY_VITALITY)
            {
                m_pUserInfo->m_stUserInfoTable.SetBuyVitalityTimes(iBuyTimes);
                m_pUserInfo->m_stUserInfoTable.SetLastBuyVitalityTime(CLogicCommon::GetSec());

                CLogicGameUserActionChannel::data_type stChannelData;
                stChannelData.m_iTimes = 1;
                stChannelData.m_iAction = CLogicGameChannelEnum::ACTION_BUY_VITALITY;
                LOGIC_PUBLISHER_INS->PublishGameUserActionInfo(m_pUserInfo, stChannelData);
            }

            PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|BUY_TIMES:" << iBuyTimes << "|VIP:" << iVip 
                                << "|CONSUME_ITEM:[" << stRsp.m_stConsumeItem.m_iItemType
                                << "," << stRsp.m_stConsumeItem.m_iCardID
                                << "," << stRsp.m_stConsumeItem.m_iNum 
                                << "]|GET_ITEM:" << strAddItem << std::endl;

            SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
        }
    }
    else if ((int)m_usCmd == MSG_LOGIC_SEQUENTIAL_BUY_GOLD)
    {
        int iVip = m_pUserInfo->m_stUserInfoTable.GetVipLevel();
        auto pstVipInfo = CLogicConfig::GetVipConfig().GetVipInfo(iVip);
        if (!pstVipInfo)
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg()
                                << "|VIP:" << iVip << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        int iBuyTimes = m_pUserInfo->m_stUserInfoTable.GetBuyGoldTimes() + 1;
        int iCanBuyTimes = std::min(pstVipInfo->m_iCanBuyGoldTimes, iBuyTimes + 4);
        if (iCanBuyTimes < iBuyTimes)
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << NOT_ENOUGH_BUY_RESOURCE_TIMES
                                << "|MSG:" << GET_ERRMSG_BY_ERRCODE(NOT_ENOUGH_BUY_RESOURCE_TIMES)
                                << "|BUY_TIMES:" << iBuyTimes << std::endl;

            SEND_ERRCODE_AND_RET(NOT_ENOUGH_BUY_RESOURCE_TIMES);
        }

        auto pstBuyResourceElem = CLogicConfig::GetBuyResourceConfig().GetBuyGoldConfig(iBuyTimes);
        if (!pstBuyResourceElem)
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg()
                                << "|BUY_TIMES:" << iBuyTimes << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        CResponseSequentialBuyGold stRsp;

        int iUseTimes = 0;
        int iTotalTimes = m_pUserInfo->m_stUserInfoTable.GetBuyGoldTotalTimes();
        stRsp.m_stConsumeItem = pstBuyResourceElem->m_stConsumeItem;
        stRsp.m_stConsumeItem.m_iNum = 0;
        while (iBuyTimes <= iCanBuyTimes)
        {
            auto pstElem = CLogicConfig::GetBuyResourceConfig().GetBuyGoldConfig(iBuyTimes);
            if (nullptr == pstElem)
                break;

            int iBaseGold = CLogicConfig::GetMoneyTreeExpConfig().GetBaseMoney(iTotalTimes * 10);

			CPackBuyGold stElem;
			stElem.m_iRate = CLogicConfig::GetVipConfig().GetShakeGoldRate(iVip);
			if (stElem.m_iRate < 2)
			{
				stElem.m_iRate = 2;
			}
			stElem.m_stGetItem.m_iItemType = CLogicConfigDefine::CURRENCY;
			stElem.m_stGetItem.m_iCardID = CLogicConfigDefine::ECIT_GoldID;
			stElem.m_stGetItem.m_iNum = iBaseGold * stElem.m_iRate;

			stRsp.m_stSequentialBuyGoldRsp.push_back(stElem);
			stRsp.m_stConsumeItem.m_iNum += pstElem->m_stConsumeItem.m_iNum;

			++iUseTimes;
			++iBuyTimes;
			++iTotalTimes;
        }
        --iBuyTimes;

        if (!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, stRsp.m_stConsumeItem, m_usCmd))
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg()
                                << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        std::string strAddItem;
        std::string strTotalItem;
        for (size_t i = 0; i < stRsp.m_stSequentialBuyGoldRsp.size(); ++i)
        {
            CLogicCacheCommon::AddGameItem(m_pUserInfo, stRsp.m_stSequentialBuyGoldRsp[i].m_stGetItem, strAddItem, m_usCmd);
            strTotalItem = strTotalItem + "," + strAddItem;
        }

        m_pUserInfo->m_stUserInfoTable.SetBuyGoldTimes(iBuyTimes);
        m_pUserInfo->m_stUserInfoTable.SetBuyGoldTotalTimes(iTotalTimes);

        logic_game_user_action_channel_data_t stData;
        stData.m_iTimes = iUseTimes;
        stData.m_iAction = CLogicGameChannelEnum::ACTION_USE_SHAKE_GOLD;
        LOGIC_PUBLISHER_INS->PublishGameUserActionInfo(m_pUserInfo, stData);

        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|BUY_TIMES:" << iBuyTimes << "|VIP:" << iVip 
                            << "|BONUS_ITEM:" << strTotalItem
                            << std::endl;

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    }
    else if ((int)m_usCmd == MSG_LOGIC_REFRESH_LEVEL_TIMES)
    {
        CRequestRefreshLevelTimes stParam;
        if(!DecodeMsgPack(stFormatData, stParam)) return (false);

        auto pLevelConfig = CLogicConfig::GetLevelConfig().GetLevelConfigElem(stParam.m_iLevelID);
        if(!pLevelConfig)
        {
            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        if(pLevelConfig->m_iSharedLevel > 0)
        {
            stParam.m_iLevelID = pLevelConfig->m_iSharedLevel;
            pLevelConfig = CLogicConfig::GetLevelConfig().GetLevelConfigElem(stParam.m_iLevelID);
            if(!pLevelConfig)
            {
                SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
            }
        }

        auto iterLevelData = m_pUserInfo->m_stLevelInfoTableMap.Find(stParam.m_iLevelID);
        if(iterLevelData == m_pUserInfo->m_stLevelInfoTableMap.End())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::LEVEL_HAS_ENOUGH_TIMES)
        }

        // 关卡今天是否已打过
        if(LOGIC_CHECK_DAY_REFRESH(iterLevelData->second->GetLastPlayTime()))
        {
            iterLevelData->second->SetSharedTimes(0);
            iterLevelData->second->SetPlayTimes(0);
            iterLevelData->second->SetLastPlayTime(CLogicCommon::GetSec());
        }

        if(iterLevelData->second->GetSharedTimes() <= 0 && iterLevelData->second->GetPlayTimes() <= 0)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::LEVEL_HAS_ENOUGH_TIMES)
        }

        // 检查重置次数
        if(LOGIC_CHECK_DAY_REFRESH(iterLevelData->second->GetLastResetTime()))
        {
            iterLevelData->second->SetResetTimes(0);
            iterLevelData->second->SetLastResetTime(CLogicCommon::GetSec());
        }

        int iVipRefreshTimes = CLogicConfig::GetVipConfig().GetRefreshLevelTimes(m_pUserInfo->m_stUserInfoTable.GetVipLevel(), pLevelConfig->m_iLevelType);
        if(iterLevelData->second->GetResetTimes() >= iVipRefreshTimes)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_BUY_RESOURCE_TIMES)
        }

        auto pConsume = CLogicConfig::GetBuyResourceConfig().GetRefreshLevelTypeConfig(pLevelConfig->m_iLevelType, iterLevelData->second->GetResetTimes());
        if(!pConsume)
        {
            SEND_ERRCODE_AND_RET1(SERVER_ERRCODE::NOT_ENOUGH_BUY_RESOURCE_TIMES, stParam.m_iLevelID)
        }

        CResponseRefreshLevelTimes stRsp;
        stRsp.m_stConsume = *pConsume;
        if(!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, stRsp.m_stConsume, m_usCmd))
        {
            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        if(iterLevelData->second->GetSharedTimes() > 0) iterLevelData->second->AddSharedTimes(-1);
        if(iterLevelData->second->GetPlayTimes() > 0) iterLevelData->second->AddPlayTimes(-1);
        iterLevelData->second->AddResetTimes(1);

        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|LEVEL_ID:" << stParam.m_iLevelID << std::endl;
        stRsp.m_stLevelInfo = CLogicCacheCommon::GetUserLevelInfo(iterLevelData->second->m_stData);

        // 通用事件
        CLogicEventData stEventNumData;
        stEventNumData.m_eType = CLogicConfigDefine::ECET_RefreshLevelTimes;
        stEventNumData.m_iPara = stParam.m_iLevelID;
        stEventNumData.m_iCmd = m_usCmd;
        LOGIC_COMMON_PUBLISHER_INS->NotifyAllObservers(m_pUserInfo, stEventNumData);

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
        return true;
    }
    else if ((int)m_usCmd == MSG_LOGIC_REFRESH_PVP_FIGHT_TIMES)
    {
//        int iVip = m_pUserInfo->m_stUserInfoTable.GetVipLevel();
//        auto pstVipInfo = CLogicConfig::GetVipConfig().GetVipInfo(iVip);
//        if (!pstVipInfo)
//        {
//            using namespace SERVER_ERRCODE;
//            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
//                                << "|MSG:" << CLogicError::GetErrMsg()
//                                << "|VIP:" << iVip << std::endl;
//
//            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
//        }
//
//        int iBuyTimes = 1;
//        if (!LOGIC_CHECK_DAY_REFRESH(m_pUserInfo->m_stUserInfoTable.GetLastRefreshPvpTime()))
//            iBuyTimes = m_pUserInfo->m_stUserInfoTable.GetRefreshPvpTimes() + 1;
//
//        int iCanBuyTimes = pstVipInfo->m_iCanBuyPVPTimes;
//        if(iCanBuyTimes < iBuyTimes)
//        {
//            using namespace SERVER_ERRCODE;
//            PROCESSOR_LOG_ERROR << "ERRCODE:" << NOT_ENOUGH_BUY_RESOURCE_TIMES
//                                << "|MSG:" << GET_ERRMSG_BY_ERRCODE(NOT_ENOUGH_BUY_RESOURCE_TIMES)
//                                << "|BUY_TIMES:" << iBuyTimes
//                                << "|CAN_BUY_TIMES:" << iCanBuyTimes
//                                << std::endl;
//
//            SEND_ERRCODE_AND_RET(NOT_ENOUGH_BUY_RESOURCE_TIMES)
//        }
//
//        if(m_pUserInfo->m_stUserInfoTable.GetPvpFightTimes() < CLogicConfig::GetBuyResourceConfig().m_iPVPOnceBuyTimes)
//        {
//            using namespace SERVER_ERRCODE;
//            SEND_ERRCODE_AND_RET(TIMES_NOT_ALL_CONSUME)
//        }
//
//        auto pstBuyResourceElem = CLogicConfig::GetBuyResourceConfig().GetRefreshPVPConfig(iBuyTimes);
//        if (!pstBuyResourceElem)
//        {
//            using namespace SERVER_ERRCODE;
//            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
//                                << "|MSG:" << CLogicError::GetErrMsg()
//                                << "|BUY_TIMES:" << iBuyTimes
//                                << std::endl;
//
//            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
//        }
//
//        if (!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, *pstBuyResourceElem, m_usCmd))
//        {
//            using namespace SERVER_ERRCODE;
//            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
//                                << "|MSG:" << CLogicError::GetErrMsg()
//                                << "|BUY_TIMES:" << iBuyTimes
//                                << std::endl;
//
//            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
//        }
//
//        m_pUserInfo->m_stUserInfoTable.AddPvpFightTimes(-CLogicConfig::GetBuyResourceConfig().m_iPVPOnceBuyTimes);
//        m_pUserInfo->m_stUserInfoTable.SetRefreshPvpTimes(iBuyTimes);
//        m_pUserInfo->m_stUserInfoTable.SetLastRefreshPvpTime(CLogicCommon::GetSec());
//
//		CResponseBuyRefresh stRsp;
//        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS|LEFT_TIMES:" << stRsp.m_iLeftTimes << std::endl;
//
//        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    }
    else if ((int)m_usCmd == MSG_LOGIC_REFRESH_CROSS_FIGHT_RESET_TIMES)
    {
        auto pstVipInfo = CLogicConfig::GetVipConfig().GetVipInfo(m_pUserInfo->m_stUserInfoTable.GetVipLevel());
        if (!pstVipInfo)
        {
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg()
                                << "|VIP:" << m_pUserInfo->m_stUserInfoTable.GetVipLevel()
                                << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        int iBuyTimes = 1;
        if (!LOGIC_CHECK_DAY_REFRESH(m_pUserInfo->m_stUserInfoTable.GetCrossFightLastRefreshResetTimesTime()))
            iBuyTimes = m_pUserInfo->m_stUserInfoTable.GetCrossFightRefreshResetTimesTimes() + 1;

        int iCanBuyTimes = pstVipInfo->m_iCanBuyRefreshResetCrossFightTimesTimes;
        if (iCanBuyTimes < iBuyTimes)
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << NOT_ENOUGH_BUY_RESOURCE_TIMES
                                << "|MSG:" << GET_ERRMSG_BY_ERRCODE(NOT_ENOUGH_BUY_RESOURCE_TIMES)
                                << "|BUY_TIMES:" << iBuyTimes << "|CAN_BUY_TIMES:" << iCanBuyTimes
                                << std::endl;

            SEND_ERRCODE_AND_RET(NOT_ENOUGH_BUY_RESOURCE_TIMES);
        }

		auto stIter = CLogicConfig::GetCrossFightConfig().m_stConfig.m_aiRefreshResetCost.find(iBuyTimes);
		if (stIter == CLogicConfig::GetCrossFightConfig().m_stConfig.m_aiRefreshResetCost.end())
		{
			using namespace SERVER_ERRCODE;
			PROCESSOR_LOG_ERROR << "ERRCODE:" << NOT_ENOUGH_BUY_RESOURCE_TIMES
				<< "|MSG:" << GET_ERRMSG_BY_ERRCODE(NOT_ENOUGH_BUY_RESOURCE_TIMES)
				<< "|BUY_TIMES:" << iBuyTimes << "|CAN_BUY_TIMES:" << iCanBuyTimes
				<< std::endl;

			SEND_ERRCODE_AND_RET(NOT_ENOUGH_BUY_RESOURCE_TIMES);
		}

        if (!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo,
                    CLogicConfigDefine::CURRENCY,
                    CLogicConfigDefine::ECIT_YuanBaoID,
					stIter->second, m_usCmd))
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg()
                                << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        m_pUserInfo->m_stUserInfoTable.SetCrossFightResetTimes(0);
        m_pUserInfo->m_stUserInfoTable.SetCrossFightRefreshResetTimesTimes(iBuyTimes);
        m_pUserInfo->m_stUserInfoTable.SetCrossFightLastRefreshResetTimesTime(CLogicCommon::GetSec());

        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS" << std::endl;

        SendToClient(SERVER_ERRCODE::SUCCESS);
    }
    else if ((int)m_usCmd == MSG_LOGIC_USER_DEPOSIT_INFO)
    {
        CResponseGetUserDepositInfo stRsp;

		const auto last_refresh_double_time = m_pUserInfo->m_stUserInfoTable.GetRefreshDoubleDepositTime();
		const auto iterBegin = CLogicConfig::GetActivityConfig().m_stActivityType2IDMap.lower_bound(CLogicConfigDefine::ACTIVITY_TYPE_DOUBLE_RECHARGE);
		const auto iterEnd = CLogicConfig::GetActivityConfig().m_stActivityType2IDMap.upper_bound(CLogicConfigDefine::ACTIVITY_TYPE_DOUBLE_RECHARGE);
		for (auto iterType = iterBegin; iterType != iterEnd; ++iterType)
		{
			const auto iterActivity = CLogicConfig::GetActivityConfig().m_stActivityRef.find(iterType->second);
			if (iterActivity != CLogicConfig::GetActivityConfig().m_stActivityRef.end())
			{
				auto is_in_time_func = std::bind(&CLogicActive::IsWithinActivityTime, m_pUserInfo, iterActivity->second.m_iType, std::placeholders::_1
					, iterActivity->second.m_iTimeType, iterActivity->second.m_iStartDay, iterActivity->second.m_iEndDay
					, iterActivity->second.m_iServerDay, iterActivity->second.m_iStartTime, iterActivity->second.m_iEndTime
					, iterActivity->second.m_iCustomCreateTime, iterActivity->second.m_iCreateDay);
				
				if (!is_in_time_func(last_refresh_double_time) && is_in_time_func(CLogicCommon::GetSec()))
				{
					m_pUserInfo->m_stUserInfoTable.AddActiveDoubleDepositCount(1);
					m_pUserInfo->m_stUserInfoTable.SetRefreshDoubleDepositTime(CLogicCommon::GetSec());
				}
			}			
		}

		for (const auto& deposit : CLogicConfig::GetDepositConfig().m_stDepositConfig)
		{
			stRsp.m_stLeftDoubleCount[deposit.first] = CLogicUserStat::GetLeftRechargeDoubleCount(m_pUserInfo, deposit.first);
			stRsp.m_stLeftBuyCount[deposit.first] = CLogicUserStat::GetLeftRechargeBuyCount(m_pUserInfo, deposit.first);
			stRsp.m_stLastDepositTime[deposit.first] = CLogicUserStat::GetLastRechargeTime(m_pUserInfo, deposit.first);
		}

        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS" << std::endl;

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    }
    else if ((int)m_usCmd == MSG_LOGIC_BUY_REFRESH_TOKEN)
    {
        CResponseBuyRefreshToken stRsp;
        CRequestBuyRefreshToken stParam;

        if (DecodeMsgPack(stFormatData, stParam) == false) return (false);

        int iYuanBaoAmount = CLogicConfig::GetGlobalConfig().m_iBuyRefreshTokenCostYuanbao * stParam.m_iNum;
        if (!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo,
                    CLogicConfigDefine::CURRENCY,
                    CLogicConfigDefine::ECIT_YuanBaoID,
                    iYuanBaoAmount, m_usCmd))
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERRCODE:" << CLogicError::GetErrCode()
                                << "|MSG:" << CLogicError::GetErrMsg()
                                << std::endl;

            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }

        // 刷新令
        CLogicCacheCommon::AddGameItem(m_pUserInfo, CLogicConfigDefine::EXCHANGE_ITEM, 8006, stParam.m_iNum, m_usCmd);

        stRsp.m_iNum = stParam.m_iNum;

        PROCESSOR_LOG_DEBUG << "PROCESS SUCCESS" << std::endl;

        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
        return (true);
    }
	else if ((int)m_usCmd == MSG_LOGIC_SHOP_ITEM)
	{
		CResponseShopItem stRsp;
		CRequestShopItem stParam;

		if (!DecodeMsgPack(stFormatData, stParam)) return (false);

		if (stParam.m_iNum <= 0 || stParam.m_iNum > 9999)
		{
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
		}

        //判断是否刷出对应的念写卡
        bool isHalfPrice = false;
        bool isRefresh = true;

        CPackConstelShopInfo* ptrConstelShopInfo = nullptr;
        auto vecConstel = m_pUserInfo->m_stIBusyData.GetConstelShop();
        if (stParam.m_bConstelShop)
        {
            for(auto& itemConstel: vecConstel)
            {
                if (itemConstel.m_iIndex == stParam.m_iID)
                {
                    ptrConstelShopInfo = &itemConstel;
                    isHalfPrice = itemConstel.m_iHalfPrice;
                    isRefresh = false;
                    break;
                }
            }
        }

        if (isRefresh && stParam.m_bConstelShop) SEND_ERRCODE_AND_RET(SERVER_ERRCODE::CONSTEL_SHOP_HAS_REFRESH)

		auto targetConfig = CLogicConfig::GetShopConfig().GetConfig(stParam.m_iID);
		if (nullptr == targetConfig)
		{
			SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
		}

        //确定是集合还是的单个物品
        std::vector<int32_t> vecBuyIndex;
        if (targetConfig->m_vecContentIndex.empty())
        {
            vecBuyIndex.push_back(stParam.m_iID);
        } else
        {
            vecBuyIndex.insert(vecBuyIndex.end(),targetConfig->m_vecContentIndex.begin(),targetConfig->m_vecContentIndex.end());
        }

        //先检查各个条件是否满足
        std::vector<int32_t> vecCanBuyIndex;
        for (auto buyItemIndex: vecBuyIndex)
        {
            auto pConfig = CLogicConfig::GetShopConfig().GetConfig(buyItemIndex);
            if (nullptr == pConfig)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
            }

            if (pConfig->m_iAvyID > 0
                && !CLogicActive::IsAvyOpen(pConfig->m_iAvyID, m_pUserInfo->m_stUserInfoTable.GetCreateTime()))
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ACTIVE_IS_NOT_OPEN)
            }

            if (stParam.m_bOrConsume && pConfig->m_stOrConsumeItem.m_iItemType <= 0)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
            }

            if (!CheckShopRuleType(pConfig->m_iRuleType, pConfig->m_iRulePara))
            {
                continue;
            }

            auto iterData = m_pUserInfo->m_stUserShopMap.Find(buyItemIndex);
            int32_t iHasBuyCount = 0;
            if(iterData != m_pUserInfo->m_stUserShopMap.End())
            {
                iHasBuyCount = iterData->second->GetBuyCount();
            }
            if(pConfig->m_iLimitType != CLogicConfigDefine::ESLT_Unlimit && iHasBuyCount + stParam.m_iNum > pConfig->m_iLimitBuyCount)
            {
                continue;
            }
            vecCanBuyIndex.push_back(buyItemIndex);
        }

        //检查商品总价是否可以购买
        if(vecCanBuyIndex.empty())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::STORE_HAS_SALE_OUT)
        } else
        {
            std::map<int32_t,CPackGameItem> mapConsumeItem;
            for (auto canBuyIndex : vecCanBuyIndex)
            {
                auto pConfig = CLogicConfig::GetShopConfig().GetConfig(canBuyIndex);
                if (nullptr == pConfig)
                {
                    SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
                }
                auto stConsume = pConfig->m_stConsumeItem;

                if(stParam.m_bOrConsume) stConsume = pConfig->m_stOrConsumeItem;
                stConsume.m_iNum *= stParam.m_iNum;

                if (isHalfPrice) stConsume.m_iNum /= 2;

                auto iterConsumeItem = mapConsumeItem.find(stConsume.m_iCardID);
                if (iterConsumeItem != mapConsumeItem.end())
                {
                    iterConsumeItem->second.m_iNum += stConsume.m_iNum;
                } else
                {
                    mapConsumeItem.insert(std::make_pair(stConsume.m_iCardID,stConsume));
                }
            }
            for (auto& consumeItem : mapConsumeItem)
            {
                if(!CLogicCacheCommon::HasEnoughItem(m_pUserInfo, consumeItem.second))
                {
                    SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ITEM)
                }
            }
        }

        //实际消耗和发放道具
        for (auto canBuyIndex: vecCanBuyIndex)
        {
            auto pConfig = CLogicConfig::GetShopConfig().GetConfig(canBuyIndex);
            //添加特殊礼包的记录
            auto awardConfig = CLogicConfig::GetAwardConfig().GetAwardConfig(canBuyIndex);
            if (nullptr != awardConfig)
            {
                user_info_key_t key(m_iUin,m_iGroupID);
                TGlobalStoreRewardTableValueType value;
                value.m_iUid = key.m_stRoleIDDetail.m_iUin;
                value.m_iGroupID = key.m_stRoleIDDetail.m_iGroupID;
                value.m_iServerID = CLogicConfig::GetSystemConfig().m_stServerConfig.m_iServerID;
                value.m_iDepositID = canBuyIndex;
                value.m_iIncreaseIndex = CLogicCacheCommon::GetUserPrimaryKeyIndex(m_pUserInfo);
                value.m_iEndTime = CLogicCommon::GetSec() + awardConfig->m_iDuration * 1_day;

                // 持续天数BUFF时间修正
                if(awardConfig->m_iDuration > 0)
                {
                    struct tm stNowDate = CLogicCommon::GetDate();
                    if (stNowDate.tm_hour < 5)
                    {
                        value.m_iEndTime = get_day_begin_time(value.m_iEndTime) - 1 + 5_hour - 1_day;
                    }
                    else
                    {
                        value.m_iEndTime = get_day_begin_time(value.m_iEndTime) - 1 + 5_hour;
                    }
                }

                TGlobalStoreRewardTable storeRewardTable;
                storeRewardTable.Initialize(value);
                CLogicGlobal::GetGlobalCache()->m_stStoreRewardTable.emplace_back(value);
            }

            auto iterData = m_pUserInfo->m_stUserShopMap.Find(canBuyIndex);
            if(iterData == m_pUserInfo->m_stUserShopMap.End())
            {
                TUserShopTableValueType stValue;
                stValue.m_iUid = m_iUin;
                stValue.m_iGroupID = m_iGroupID;
                stValue.m_iItemIndex = canBuyIndex;
                stValue.m_iBuyCount = 0;
                auto stRet = m_pUserInfo->m_stUserShopMap.Insert(std::make_pair(canBuyIndex, stValue));
                if(!stRet.second)
                {
                    SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR)
                }

                iterData = stRet.first;
            }

            auto stConsume = pConfig->m_stConsumeItem;
            if(stParam.m_bOrConsume) stConsume = pConfig->m_stOrConsumeItem;
            stConsume.m_iNum *= stParam.m_iNum;

            if (isHalfPrice) stConsume.m_iNum /= 2;

            if(!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, stConsume, m_usCmd))
            {
                SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
            }

            auto stGet = pConfig->m_stGetItem;
            stGet.m_iNum *= stParam.m_iNum;
            CLogicCacheCommon::AddGameItem(m_pUserInfo, stGet, m_usCmd);
            iterData->second->AddBuyCount(stParam.m_iNum);

            if (nullptr != ptrConstelShopInfo) ptrConstelShopInfo->m_iHasBuy = true;
            m_pUserInfo->m_stIBusyData.SetConstelShop(vecConstel);

            stRsp.m_vecSucBuyIndex.push_back(canBuyIndex);
        }

		stRsp.m_iID = stParam.m_iID;
        stRsp.m_iNum = stParam.m_iNum;
        stRsp.m_stItemData = m_pUserInfo->m_stCacheBonusData;
		SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
		return (true);
	}
    else if ((int)m_usCmd == MSG_LOGIC_REFRESH_CONSTEL_SHOP)
    {
        CResponseRefreshConstelShop stRsp;
        auto constelShopConfig = CLogicConfig::GetShopConfig().m_stConstelShopConfig;
        if(m_pUserInfo->m_stIBusyData.GetConstelShopRefreshCount() >= constelShopConfig.m_iRefreshCount)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::STORE_HAS_SALE_OUT)
        }

        PROCESSOR_LOG_DEBUG << "m_iNum" << constelShopConfig.m_iRefreshConsume.m_iNum
                            << "m_iItemType"   << constelShopConfig.m_iRefreshConsume.m_iItemType
                            << "m_iCardID" << constelShopConfig.m_iRefreshConsume.m_iCardID << std::endl;
        if(!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, constelShopConfig.m_iRefreshConsume, m_usCmd))
        {
            SEND_ERRCODE_AND_RET(CLogicError::GetErrCode())
        }
        CLogicCacheCommon::RefreshConstelShop(m_pUserInfo);
        m_pUserInfo->m_stIBusyData.AddConstelShopRefreshCount(1);

        stRsp.m_vecShopInfo = m_pUserInfo->m_stIBusyData.GetConstelShop();
        stRsp.m_vecConsumeItem = m_pUserInfo->m_stCacheBonusData;
        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    }
    else if ((int)m_usCmd == MSG_LOGIC_GET_CONSTEL_SHOP_INFO)
    {
        if (m_pUserInfo->m_stIBusyData.GetConstelShop().empty())
        {
            CLogicCacheCommon::RefreshConstelShop(m_pUserInfo);
        }
        auto constelShopConfig = CLogicConfig::GetShopConfig().m_stConstelShopConfig;
        auto stNow = CLogicCommon::GetDate();
        time_t iNow = CLogicCommon::GetSec();
        time_t iLastRefreshTime = m_pUserInfo->m_stIBusyData.GetConstelShopLastTime();

        if (stNow.tm_hour >= constelShopConfig.m_iRefreshTime && !is_same_day(iNow,iLastRefreshTime))
        {
            CLogicCacheCommon::RefreshConstelShop(m_pUserInfo);
            m_pUserInfo->m_stIBusyData.SetConstelShopLastTime(iNow);
        }
        CResponseConstelShopInfo stRsp;
        stRsp.m_vecShopInfo = m_pUserInfo->m_stIBusyData.GetConstelShop();
        stRsp.m_iRefreshCount = m_pUserInfo->m_stIBusyData.GetConstelShopRefreshCount();
        SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
    }

    return (true);
}

bool
CLogicBuyResourceProcessor::CheckVipLevel(unsigned short iCommand)
{
    int iNeedVip = 0;

    if (iCommand == (unsigned short)MSG_LOGIC_BUY_ENERGY)
    {
        iNeedVip = CLogicConfig::GetVipUnlockConfig().m_iBuyEnergy;
    }
    else if (iCommand == (unsigned short)MSG_LOGIC_BUY_VITALITY)
    {
        iNeedVip = CLogicConfig::GetVipUnlockConfig().m_iBuyVitality;
    }
    else if (iCommand == (unsigned short)MSG_LOGIC_BUY_GOLD)
    {
        iNeedVip = CLogicConfig::GetVipUnlockConfig().m_iBuyGold;
    }
    else if (iCommand == (unsigned short)MSG_LOGIC_REFRESH_PVP_FIGHT_TIMES)
    {
        iNeedVip = CLogicConfig::GetVipUnlockConfig().m_iRefreshPVP;
    }
    else
    {
        return true;
    }

    if (iNeedVip > m_pUserInfo->m_stUserInfoTable.GetVipLevel())
    {
        using namespace SERVER_ERRCODE;
        SET_ERR_INFO(NOT_ENOUGH_VIP_LEVEL);
        return false;
    }

    return true;
}

bool CLogicBuyResourceProcessor::CheckShopRuleType(int32_t iRuleType, int32_t iNeedGrade)
{
    if (iRuleType == CLogicConfigDefine::ESRT_Empty)
    {
        return true;
    }

    if (iRuleType == CLogicConfigDefine::ESRT_VIP && m_pUserInfo->m_stUserInfoTable.GetVipLevel() >= iNeedGrade)
    {
        return true;
    }

    if (iRuleType == CLogicConfigDefine::ESRT_Level && m_pUserInfo->m_stUserInfoTable.GetLevel() >= iNeedGrade)
    {
        return true;
    }

    if (iRuleType == CLogicConfigDefine::ESRT_Hero)
    {
        if(m_pUserInfo->m_stHeroCardTableMap.Find(iNeedGrade) != m_pUserInfo->m_stHeroCardTableMap.End())
            return true;
    }

    if(iRuleType == CLogicConfigDefine::ESRT_Server)
    {
        const int32_t iOpenServerDay = day_diff(CLogicConfig::GetSystemConfig().GetOpenSvrTime(), CLogicCommon::GetSec()) + 1;
        if(iOpenServerDay >= iNeedGrade) return true;
    }

    return false;
}
