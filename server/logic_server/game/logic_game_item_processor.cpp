#include "server_errcode.h"
#include "server_cmd_logic_define.h"
#include "server_fashion_protocol.h"
#include "server_game_item_protocol.h"
#include "logic_link_common.h"
#include "logic_link_error.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_game_item_processor.h"
#include "logic_game_active_info.h"
#include "logic_config.h"
#include "logic_game_cache_common.h"
#include "logic_game_user_notify_system.h"
#include "logic_game_global.h"
#include "logic_game_daily_task_system.h"
#include "logic_game_cache_card_operation.h"
#include "logic_game_active.h"
#include "logic_game_item_factory.h"

REGISTER_CMD_FACTORY(MSG_LOGIC_EQUIP_FASHION, CLogicGameItemProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_UNLOCK_HONOR, CLogicGameItemProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_EQUIP_HONOR, CLogicGameItemProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_PHY_STORE_EXTRACT, CLogicGameItemProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_USE_ITEM, CLogicGameItemProcessor)

CLogicGameItemProcessor::CLogicGameItemProcessor(uint16_t usCmd, const std::string &strCmdName)
		: CLogicProcessorBase(usCmd, strCmdName)
{
}

bool CLogicGameItemProcessor::DoUserRun(const msgpack::unpacked & stFormatData)
{
	bool bRet = false;
	switch (m_usCmd)
	{
	case MSG_LOGIC_EQUIP_FASHION:
		bRet = DoUserRunEquipFashion(stFormatData);
		break;

	case MSG_LOGIC_UNLOCK_HONOR:
		bRet = DoUserRunUnlockHonor(stFormatData);
		break;

	case MSG_LOGIC_EQUIP_HONOR:
		bRet = DoUserRunEquipHonor(stFormatData);
		break;

    case MSG_LOGIC_PHY_STORE_EXTRACT:
        bRet = DoUserRunPhyStoreExtract(stFormatData);
        break;

	case MSG_LOGIC_USE_ITEM:
		bRet = DoUserRunUseItem(stFormatData);
		break;

	default:
		break;
	}
	return bRet;
}

bool CLogicGameItemProcessor::DoUserRunEquipFashion(const msgpack::unpacked& stFormatData)
{
	CRequestEquipFashion stReqBody;
	CResponseEquipFashion stRspBody;

	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

    auto iterCard = m_pUserInfo->m_stHeroCardTableMap.Find(stReqBody.m_iCardID);
    if(iterCard == m_pUserInfo->m_stHeroCardTableMap.End())
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HERO_CARD_IS_NOT_OWNED)
    }

	if (stReqBody.m_iFashionID > 0)
	{
		auto pConfig = CLogicConfig::GetFashionConfig().GetFashionConfig(stReqBody.m_iFashionID);
		if (nullptr == pConfig)
		{
			SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR)
		}

		if (pConfig->m_iBelongCardID != stReqBody.m_iCardID)
		{
			SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HERO_CARD_IS_NOT_CONFIGED)
		}

        auto iterFashion = m_pUserInfo->m_stFashionInfoMap.Find(stReqBody.m_iFashionID);
        if(iterFashion == m_pUserInfo->m_stFashionInfoMap.End())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::ITEM_NOT_OWNED)
        }

        if(iterFashion->second->GetExpiredTime() > 0 && iterFashion->second->GetExpiredTime() < CLogicCommon::GetSec())
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::FASHION_EXPIRED)
        }

        iterCard->second->SetFashionID(stReqBody.m_iFashionID);
	}
    else
    {
        iterCard->second->SetFashionID(0);
    }

	stRspBody.m_iFashionID = stReqBody.m_iFashionID;
	stRspBody.m_iCardID = stReqBody.m_iCardID;

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicGameItemProcessor::DoUserRunUnlockHonor(const msgpack::unpacked& stFormatData)
{
	CRequestUnlockHonor stReqBody;
	CResponseUnlockHonor stRspBody;
	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	const auto* pstConfig = CLogicConfig::GetHonorConfig().GetHonorConfig(stReqBody.m_iHonorID);
	if (nullptr == pstConfig)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HONOR_NOT_EXIST);
	}

	auto iterHonor = m_pUserInfo->m_stUserHonorMap.Find(stReqBody.m_iHonorID);
	if (iterHonor == m_pUserInfo->m_stUserHonorMap.End())
	{
		TUserHonorTableValueType value;
		value.m_iUid = m_pUserInfo->m_iUin;
		value.m_iGroupID = m_pUserInfo->m_iGroupID;
		value.m_iHonorID = stReqBody.m_iHonorID;
		const auto ret = m_pUserInfo->m_stUserHonorMap.Insert(std::make_pair(value.m_iHonorID, value));
		if (!ret.second)
		{
			SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HONOR_NOT_EXIST);
		}
		iterHonor = ret.first;
	}

	if (iterHonor->second->GetHonorStatus() != CLogicConfigDefine::HONOR_STATUS_NONE)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::HONOR_HAD_UNLOCK);
	}

	const std::vector<int32_t> progress = {
		iterHonor->second->GetTaskProgress1(),
		iterHonor->second->GetTaskProgress2(),
		iterHonor->second->GetTaskProgress3(),
		iterHonor->second->GetTaskProgress4(),
		iterHonor->second->GetTaskProgress5()
	};

	if (pstConfig->m_stTaskVec.size() > progress.size())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR);
	}

	for (size_t index = 0; index < pstConfig->m_stTaskVec.size(); ++index)
	{
		if (progress[index] < pstConfig->m_stTaskVec[index].m_iTargetAmount)
		{
			SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_MEET_CONDITION);
		}
	}

	stRspBody.m_iHonorID = stReqBody.m_iHonorID;
	stRspBody.m_stConsumeItem = pstConfig->m_stConsumeItem;
	if (stRspBody.m_stConsumeItem.m_iItemType > 0 && !CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, stRspBody.m_stConsumeItem, m_usCmd))
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ITEM);
	}

	iterHonor->second->SetHonorStatus(CLogicConfigDefine::HONOR_STATUS_UNLOCK);
	CLogicUserNotifySystem::NotifyUpdateHonor(m_pUserInfo, iterHonor->second->m_stData);
	CLogicGlobal::PublishBroadcast(pstConfig->m_strBroadcast, 0, {
			{"nick", m_pUserInfo->m_strNick},
			{"honor", pstConfig->m_strName}
		});

	if (0 == iterHonor->second->GetExpireTime())
	{
		int32_t iHaveHonorCount = 0;
		for (auto iter = m_pUserInfo->m_stUserHonorMap.Begin(); iter != m_pUserInfo->m_stUserHonorMap.End(); ++iter)
		{
			if (iter->second->GetExpireTime() == 0 && iter->second->GetHonorStatus() != CLogicConfigDefine::HONOR_STATUS_NONE)
			{
				++iHaveHonorCount;
			}
		}
		CLogicDailyTaskSystem::SetProgress(m_pUserInfo, CLogicDailyTaskSystem::USER_GAME_STATUS_TASK, CLogicDailyTaskSystem::TASK_STATUS_HONOR_COUNT, iHaveHonorCount);
	}

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicGameItemProcessor::DoUserRunEquipHonor(const msgpack::unpacked& stFormatData)
{
	CRequestEquipHonor stReqBody;
	CResponseEquipHonor stRspBody;
	if (!DecodeMsgPack(stFormatData, stReqBody))
	{
		return false;
	}

	for (auto iter = m_pUserInfo->m_stUserHonorMap.Begin(); iter != m_pUserInfo->m_stUserHonorMap.End(); ++iter)
	{
		if (CLogicConfigDefine::HONOR_STATUS_EQUIP == iter->second->GetHonorStatus())
		{
			iter->second->SetHonorStatus(CLogicConfigDefine::HONOR_STATUS_UNLOCK);
			CLogicUserNotifySystem::NotifyUpdateHonor(m_pUserInfo, iter->second->m_stData);
		}

		if (iter->first == stReqBody.m_iHonorID && CLogicConfigDefine::HONOR_STATUS_UNLOCK == iter->second->GetHonorStatus())
		{
			if (iter->second->GetExpireTime() == 0 || iter->second->GetExpireTime() > CLogicCommon::GetSec())
			{
				iter->second->SetHonorStatus(CLogicConfigDefine::HONOR_STATUS_EQUIP);
				CLogicUserNotifySystem::NotifyUpdateHonor(m_pUserInfo, iter->second->m_stData);
				stRspBody.m_iHonorID = stReqBody.m_iHonorID;
			}
		}
	}

	SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicGameItemProcessor::DoUserRunPhyStoreExtract(const msgpack::unpacked& stFormatData)
{
    CResponsePhyStoreExtract stRspBody;

    stRspBody.m_iAddPhy = m_pUserInfo->m_stUserInfoTable.GetPhyStore() / CLogicConfigDefine::ESD_NormalPctWeight;
    if(stRspBody.m_iAddPhy <= 0)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_PHY_STORE)
    }

    CLogicCacheCommon::AddGameItem(m_pUserInfo, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_EnergyID, stRspBody.m_iAddPhy, m_usCmd);

    m_pUserInfo->m_stUserInfoTable.AddPhyStore(-(stRspBody.m_iAddPhy * CLogicConfigDefine::ESD_NormalPctWeight));

    stRspBody.m_iPhyStore = m_pUserInfo->m_stUserInfoTable.GetPhyStore();
    SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
    return true;
}

bool CLogicGameItemProcessor::DoUserRunUseItem(const msgpack::unpacked& stFormatData)
{
    CRequestUseItem stReqBody;
    CResponseUseItem stRspBody;

    if (!DecodeMsgPack(stFormatData, stReqBody))
    {
        return false;
    }

    if (stReqBody.m_iNum < 1 || stReqBody.m_iNum > 1000)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NUMBER_INVALID)
    }

    auto pConfig = CLogicConfig::GetItemConfig().GetConfig(stReqBody.m_iItemID);
    if(pConfig == nullptr)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::GAME_ITEM_NOT_CFG)
    }

    if (CLogicCacheCommon::GetItemNum(m_pUserInfo, CLogicConfigDefine::EXCHANGE_ITEM, stReqBody.m_iItemID) < stReqBody.m_iNum)
    {
        SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ITEM)
    }


    int iRetCode;
    switch (pConfig->m_iUseType)
    {
    case CLogicConfigDefine::EIUT_Fashion:
        iRetCode = UseFashionItem(stReqBody.m_iItemID, stReqBody.m_iNum);
        break;
    case CLogicConfigDefine::EIUT_GiftBag:
        iRetCode = UseGiftBagItem(stReqBody.m_iItemID, stReqBody.m_iNum);
        break;
    case CLogicConfigDefine::EIUT_Buff:
        iRetCode = UseBuffItem(stReqBody.m_iItemID, stReqBody.m_iNum, pConfig->m_iUsePara1);
        break;
    case CLogicConfigDefine::EIUT_ExploreCangBaoTu:
        iRetCode = UseExploreCangBaoTuItem(stReqBody.m_iItemID, stReqBody.m_iNum, pConfig->m_iUsePara1);
        break;
    default:
        iRetCode = SERVER_ERRCODE::ITEM_CANNOT_USE;
        break;
    }

    if(iRetCode != SERVER_ERRCODE::SUCCESS)
    {
        SEND_ERRCODE_AND_RET(iRetCode)
    }

    stRspBody.m_stBonus = m_pUserInfo->m_stCacheBonusData;
    SendToClient(stRspBody, SERVER_ERRCODE::SUCCESS);
    return true;
}

int32_t CLogicGameItemProcessor::UseFashionItem(const int32_t iItemID, const int32_t iNum)
{
    if(iNum != 1)
    {
        return SERVER_ERRCODE::NUMBER_INVALID;
    }

    const auto iterItemConfig = CLogicConfig::GetFashionConfig().m_stFashionItemMap.find(iItemID);
    if (iterItemConfig == CLogicConfig::GetFashionConfig().m_stFashionItemMap.end())
    {
        return SERVER_ERRCODE::PARAMETER_ERROR;
    }

    if(!CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, CLogicConfigDefine::EXCHANGE_ITEM, iItemID, iNum, m_usCmd))
    {
        return CLogicError::GetErrCode();
    }

    auto iterFashion = m_pUserInfo->m_stFashionInfoMap.Find(iterItemConfig->second.m_iFashionID);
    if (iterFashion == m_pUserInfo->m_stFashionInfoMap.End())
    {
        TFashionInfoTableValueType stVal;
        stVal.m_iUid = m_pUserInfo->m_iUin;
        stVal.m_iGroupID = m_pUserInfo->m_iGroupID;
        stVal.m_iFashionID = iterItemConfig->second.m_iFashionID;
        stVal.m_iExpiredTime = CLogicCommon::GetSec();
        const auto stRet = m_pUserInfo->m_stFashionInfoMap.Insert(std::make_pair(stVal.m_iFashionID, stVal));
        if (!stRet.second)
        {
            SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR);
        }
        iterFashion = stRet.first;
    }

    if (0 == iterFashion->second->GetExpiredTime())
    {
        CLogicCacheCommon::AddGameItem(m_pUserInfo, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_YuanBaoID, iterItemConfig->second.m_iYuanbao, m_usCmd);
    }
    else
    {
        int32_t iNewExpiredTime = 0;
        if (iterItemConfig->second.m_iDay > 0)
        {
            const int32_t iTime = std::max(CLogicCommon::GetSec(), iterFashion->second->GetExpiredTime());
            iNewExpiredTime = iTime + iterItemConfig->second.m_iDay * 1_day;
        }
        iterFashion->second->SetExpiredTime(iNewExpiredTime);

        m_pUserInfo->m_stCacheBonusData.m_stBonus.m_stFashionVec.emplace_back(CLogicCacheCommon::GetFashionInfo(iterFashion->second->m_stData));
    }

    return SERVER_ERRCODE::SUCCESS;
}

int32_t CLogicGameItemProcessor::UseGiftBagItem(const int32_t iItemID, const int32_t iNum)
{
    auto pConfig = CLogicConfig::GetGiftBagConfig().GetConfig(iItemID);
    if(nullptr == pConfig)
    {
        return SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST;
    }

    if(pConfig->m_iOpenDate > CLogicCommon::GetSec())
    {
        return SERVER_ERRCODE::AWARD_NOT_GET_TIME;
    }

    auto iterData = m_pUserInfo->m_stItemMap.Find(iItemID);
    if(iterData == m_pUserInfo->m_stItemMap.End())
    {
        return SERVER_ERRCODE::INTERN_ERROR;
    }

    if(pConfig->m_iLevel > m_pUserInfo->m_stUserInfoTable.GetLevel())
    {
        return SERVER_ERRCODE::NOT_ENOUGH_ROLE_LEVEL;
    }

    // 判断开启时间和次数
    if(iterData->second->GetNextOpenTime() > CLogicCommon::GetSec())
    {
        return SERVER_ERRCODE::AWARD_NOT_GET_TIME;
    }

    // 可无限开启的和有次数限制的，不能多开
    if(pConfig->m_iOpenCount != 0 && iNum != 1)
    {
        return SERVER_ERRCODE::NUMBER_INVALID;
    }

    if(pConfig->m_iOpenCount > 0 && pConfig->m_iOpenCount <= iterData->second->GetOpenedCount())
    {
        return SERVER_ERRCODE::COUNT_NOT_ENOUGH;
    }

    // 打开礼包
    std::vector<CPackGameItem> stItemVec;
    CLogicConfig::GetGiftBagConfig().OpenGift(iItemID, stItemVec, iNum);
    CLogicCacheCommon::AddGameItem(m_pUserInfo, stItemVec, m_usCmd);

    // 记录时间和次数
    iterData->second->AddOpenedCount(iNum);

    // 消耗
    if(pConfig->m_iOpenCount >= 0 && iterData->second->GetOpenedCount() >= pConfig->m_iOpenCount)
    {
        CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, CLogicConfigDefine::EXCHANGE_ITEM, iItemID, iNum, m_usCmd);
    }
    else
    {
        int32_t iIntervalTime = pConfig->m_bIsPerDay ? 1_day : pConfig->m_iIntervalTime;
        iIntervalTime += CLogicCommon::GetSec();
        if(pConfig->m_bIsPerDay) iIntervalTime = get_day_begin_time(iIntervalTime) + pConfig->m_iIntervalTime;
        iterData->second->SetNextOpenTime(iIntervalTime);

        CLogicUserNotifySystem::NotifyUserItemAttr(m_pUserInfo, iItemID, iterData->second->GetNextOpenTime(), iterData->second->GetOpenedCount());
    }

    return SERVER_ERRCODE::SUCCESS;
}

int32_t CLogicGameItemProcessor::UseBuffItem(const int32_t iItemID, const int32_t iNum, const int32_t iBuffID)
{
    if(iNum != 1)
        return SERVER_ERRCODE::NUMBER_INVALID;

    int32_t iRetCode = CLogicBuff::AddBuff(m_pUserInfo, iBuffID);
    if(iRetCode != SERVER_ERRCODE::SUCCESS)
        return iRetCode;

    CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, CLogicConfigDefine::EXCHANGE_ITEM, iItemID, iNum, m_usCmd);

    return SERVER_ERRCODE::SUCCESS;
}

int32_t CLogicGameItemProcessor::UseExploreCangBaoTuItem(const int32_t iItemID, const int32_t iNum, const int32_t iQuestID)
{
    if(iNum != 1) return SERVER_ERRCODE::NUMBER_INVALID;

    auto pConfig = CLogicConfig::GetHomeGardenExploreConfig().GetExploreQuestConfig(iQuestID);
    if(nullptr == pConfig || !pConfig->m_bIsCbt)
    {
        return SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST;
    }

    auto iterData = m_pUserInfo->m_stHomeExploreMap.Find(iQuestID);
    if(iterData != m_pUserInfo->m_stHomeExploreMap.End())
    {
        if(iterData->second->GetState() != CLogicConfigHomeGardenExploreParser::EQS_Received)
            return SERVER_ERRCODE::DAILY_TASK_NOT_COMPLETED;

        iterData->second->SetState(CLogicConfigHomeGardenExploreParser::EQS_CanAccept);
        iterData->second->SetTeam({});
        iterData->second->SetEndTime(0);
        iterData->second->SetIsCbt(true);
    }
    else
    {
        TUserHomeExploreTableValueType stValue;
        stValue.m_iUid = m_pUserInfo->m_iUin;
        stValue.m_iGroupID = m_pUserInfo->m_iGroupID;
        stValue.m_iExploreQuestID = iQuestID;
        stValue.m_iEndTime = 0;
        stValue.m_cIsCbt = true;
        stValue.m_cState = CLogicConfigHomeGardenExploreParser::EQS_CanAccept;
        auto stRet = m_pUserInfo->m_stHomeExploreMap.Insert(std::make_pair(stValue.m_iExploreQuestID, stValue));
        if (stRet.second == false)
        {
            using namespace SERVER_ERRCODE;
            PROCESSOR_LOG_ERROR << "ERROR:" << INTERN_ERROR << "|MSG:" << GET_ERRMSG_BY_ERRCODE(INTERN_ERROR)
                                << "|EXPLORE_QUEST_ID:" << iQuestID << std::endl;

            return SERVER_ERRCODE::INTERN_ERROR;
        }

        iterData = stRet.first;
    }

    CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, CLogicConfigDefine::EXCHANGE_ITEM, iItemID, iNum, m_usCmd);

    // 发送通知
    CLogicUserNotifySystem::NotifyUserExploreQuest(m_pUserInfo, iterData->second->m_stData);

    return SERVER_ERRCODE::SUCCESS;
}
