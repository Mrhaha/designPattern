#include "server_errcode.h"
#include "server_cmd_logic_define.h"
#include "logic_link_common.h"
#include "logic_link_error.h"
#include "logic_config.h"
#include "logic_game_qiyu_processor.h"
#include "logic_game_cmd_processor_factory.h"
#include "logic_game_cache_common.h"
#include "logic_game_cache_card_operation.h"
#include "logic_game_active.h"
#include "logic_game_user_notify_system.h"
#include "logic_game_publisher.h"

REGISTER_CMD_FACTORY(MSG_LOGIC_QIYU_GET_INFO_REQ, CLogicQiyuProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_QIYU_MOVE_REQ, CLogicQiyuProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_QIYU_FINISH_REQ, CLogicQiyuProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_QIYU_REFRESH_REQ, CLogicQiyuProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_QIYU_RONGLIAN_REQ, CLogicQiyuProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_QIYU_FIGHT_START, CLogicQiyuProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_QIYU_FIGHT_RESULT, CLogicQiyuProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_QIYU_CANGBAOTU_DUIHUAN_REQ, CLogicQiyuProcessor)
REGISTER_CMD_FACTORY(MSG_LOGIC_QIYU_CANGBAOTU_USE_REQ, CLogicQiyuProcessor)


std::string CLogicQiyuProcessor::RandomRewardGroup(int userLevel, const std::string &levelRewardPoolName)
{
	std::string rewardGroupName;

	do
	{
		//1.等级奖励池
		auto levelRewardPoolIter = CLogicConfig::GetQiyuConfig().m_mpLevelRewardPool.find(levelRewardPoolName);
		if (levelRewardPoolIter == CLogicConfig::GetQiyuConfig().m_mpLevelRewardPool.end())
			break;

		//2.奖励池
		std::string rewardPoolName;
		for (auto &iter1 : levelRewardPoolIter->second.m_aLevelReward)
		{
			if ((userLevel >= iter1.m_iBeginLevel) && (userLevel <= iter1.m_iEndLevel))
			{
				rewardPoolName = iter1.m_strRewardPoolName;
				break;
			}
		}

		auto rewardPoolIter = CLogicConfig::GetQiyuConfig().m_mpRewardPool.find(rewardPoolName);
		if (rewardPoolIter == CLogicConfig::GetQiyuConfig().m_mpRewardPool.end())
			break;

		//3.奖励组
		int totalCoef = 0;
		for (auto &iter2 : rewardPoolIter->second.m_aRewardGroup)
		{
			totalCoef += iter2.second;
		}
		int randomNum = CLogicCommon::GetRandNum(1, totalCoef + 1);
		
		totalCoef = 0;
		for (auto &iter2 : rewardPoolIter->second.m_aRewardGroup)
		{
			totalCoef += iter2.second;
			if (totalCoef >= randomNum)
			{
				rewardGroupName = iter2.first;
				break;
			}
		}
	} while (false);

	return rewardGroupName;
}

std::string CLogicQiyuProcessor::RandomJiaoyiGroup(int userLevel, const std::string &jiaoyiLevelPoolName)
{
	std::string jiaoyiGroupName;

	do
	{
		//1.等级奖励池
		auto jiaoyiLevelPoolIter = CLogicConfig::GetQiyuConfig().m_mpJiaoyiLevelPool.find(jiaoyiLevelPoolName);
		if (jiaoyiLevelPoolIter == CLogicConfig::GetQiyuConfig().m_mpJiaoyiLevelPool.end())
			break;

		//2.奖励池
		std::string jiaoyiPoolName;
		for (auto &iter1 : jiaoyiLevelPoolIter->second.m_aLevelJiaoyi)
		{
			if ((userLevel >= iter1.m_iBeginLevel) && (userLevel <= iter1.m_iEndLevel))
			{
				jiaoyiPoolName = iter1.m_strJiaoyiPoolName;
				break;
			}
		}

		auto jiaoyiPoolIter = CLogicConfig::GetQiyuConfig().m_mpJiaoyiPool.find(jiaoyiPoolName);
		if (jiaoyiPoolIter == CLogicConfig::GetQiyuConfig().m_mpJiaoyiPool.end())
			break;

		//3.奖励组
		int totalCoef = 0;
		for (auto &iter2 : jiaoyiPoolIter->second.m_aJiaoyiGroup)
		{
			totalCoef += iter2.second;
		}
		int randomNum = CLogicCommon::GetRandNum(1, totalCoef + 1);

		totalCoef = 0;
		for (auto &iter2 : jiaoyiPoolIter->second.m_aJiaoyiGroup)
		{
			totalCoef += iter2.second;
			if (totalCoef >= randomNum)
			{
				jiaoyiGroupName = iter2.first;
				break;
			}
		}
	} while (false);

	return jiaoyiGroupName;
}

std::pair<int, stGuanqiaItem>	CLogicQiyuProcessor::RandomGuanqia(int userLevel, int levelGuanqiaPoolId)
{
	int guanqiaID = 0;
	stGuanqiaItem guanqiaItem;

	do
	{
		//1.等级关卡池
		auto levelGuanqiaPoolIter = CLogicConfig::GetQiyuConfig().m_mpGuanqiaLevelPool.find(levelGuanqiaPoolId);
		if (levelGuanqiaPoolIter == CLogicConfig::GetQiyuConfig().m_mpGuanqiaLevelPool.end())
			break;

		//2.关卡池
		int poolId = 0;
		for (auto &iter1 : levelGuanqiaPoolIter->second.m_aLevelGuanqia)
		{
			if ((userLevel >= iter1.m_iBeginLevel) && (userLevel <= iter1.m_iEndLevel))
			{
				poolId = iter1.m_iGuanqiaPoolId;
				break;
			}
		}

		auto guanqiaPoolIter = CLogicConfig::GetQiyuConfig().m_mpGuanqiaPool.find(poolId);
		if (guanqiaPoolIter == CLogicConfig::GetQiyuConfig().m_mpGuanqiaPool.end())
			break;

		//3.关卡
		int totalCoef = 0;
		for (auto &iter2 : guanqiaPoolIter->second.m_mpGuanqia)
		{
			totalCoef += iter2.second.m_iCoef;
		}
		int randomNum = CLogicCommon::GetRandNum(1, totalCoef + 1);

		totalCoef = 0;
		for (auto &iter2 : guanqiaPoolIter->second.m_mpGuanqia)
		{
			totalCoef += iter2.second.m_iCoef;
			if (totalCoef >= randomNum)
			{
				guanqiaID = iter2.first;
				guanqiaItem.m_iSex = iter2.second.m_iSex;
				guanqiaItem.m_bPVEOrPVP = iter2.second.m_bPVEOrPVP;
				break;
			}
		}
	} while (false);


	return std::make_pair(guanqiaID, guanqiaItem);
}

int	CLogicQiyuProcessor::RandomConsumeGroup(int userLevel, int levelConsumePoolId)
{
	int groupID = 0;

	do
	{
		//1.等级消耗池
		auto levelConsumePoolIter = CLogicConfig::GetQiyuConfig().m_mpLevelConsumePool.find(levelConsumePoolId);
		if (levelConsumePoolIter == CLogicConfig::GetQiyuConfig().m_mpLevelConsumePool.end())
			break;

		//2.消耗池
		for (auto &iter1 : levelConsumePoolIter->second.m_aConsume)
		{
			if ((userLevel >= iter1.m_iBeginLevel) && (userLevel <= iter1.m_iEndLevel))
			{
				groupID = iter1.m_iConsumeGroupID;
				break;
			}
		}
	
	} while (false);


	return groupID;
}

void CLogicQiyuProcessor::GenQiyuBuff(bool dayQiyuOrCangbaotu, const CPackEntireWarBuff &newBuff)
{
	if (newBuff.m_iType == 7)
	{
		if (newBuff.m_iPercentage < 0)
		{
			if ((newBuff.m_iPercentage >= -100))
			{
				for (auto stIter = m_pUserInfo->m_stHeroCardTableMap.Begin();
					stIter != m_pUserInfo->m_stHeroCardTableMap.End();
					++stIter)
				{
					CLogicCacheCardOperation cardOperation(m_pUserInfo);
					if (!cardOperation.CheckHeroCardExist(stIter->first))
						continue;

					if (dayQiyuOrCangbaotu)
					{
						if (stIter->second->GetQiyuFightCostHp() < 100)
						{
							stIter->second->AddQiyuFightCostHp(-newBuff.m_iPercentage);
							if (stIter->second->GetQiyuFightCostHp() >= 100)
								stIter->second->SetQiyuFightCostHp(99);
						}
					}
					else
					{
						if (stIter->second->GetCangbaotuFightCostHp() < 100)
						{
							stIter->second->AddCangbaotuFightCostHp(-newBuff.m_iPercentage);
							if (stIter->second->GetCangbaotuFightCostHp() >= 100)
								stIter->second->SetCangbaotuFightCostHp(99);
						}
					}
				}
			}
		}
		else
		{
			for (auto stIter = m_pUserInfo->m_stHeroCardTableMap.Begin();
				stIter != m_pUserInfo->m_stHeroCardTableMap.End();
				++stIter)
			{
				CLogicCacheCardOperation cardOperation(m_pUserInfo);
				if (!cardOperation.CheckHeroCardExist(stIter->first))
					continue;

				if (dayQiyuOrCangbaotu)
				{
					stIter->second->AddQiyuFightCostHp(-newBuff.m_iPercentage);
					if (stIter->second->GetQiyuFightCostHp() < 0)
						stIter->second->SetQiyuFightCostHp(0);
				}
				else
				{
					stIter->second->AddCangbaotuFightCostHp(-newBuff.m_iPercentage);
					if (stIter->second->GetCangbaotuFightCostHp() < 0)
						stIter->second->SetCangbaotuFightCostHp(0);
				}
			}
		}
	}
	else
	{
		if (dayQiyuOrCangbaotu)
		{
			auto qiyuBuf = m_pUserInfo->m_stUserInfoTable.GetQiyuBuff();
			qiyuBuf.m_stBuffVec.push_back(newBuff);
			m_pUserInfo->m_stUserInfoTable.SetQiyuBuff(qiyuBuf);
		}
		else
		{
			auto qiyuBuf = m_pUserInfo->m_stUserInfoTable.GetCangbaotuBuff();
			qiyuBuf.m_stBuffVec.push_back(newBuff);
			m_pUserInfo->m_stUserInfoTable.SetCangbaotuBuff(qiyuBuf);
		}
	}
}

CLogicQiyuProcessor::CLogicQiyuProcessor(uint16_t usCmd, const std::string &strCmdName)
        : CLogicProcessorBase(usCmd, strCmdName)
{
}

bool CLogicQiyuProcessor::DoUserRun(const msgpack::unpacked& stFormatData)
{
	switch ((int)m_usCmd)
	{
	case MSG_LOGIC_QIYU_GET_INFO_REQ:
		return DoQIYU_GET_INFO_REQ(stFormatData);
	case MSG_LOGIC_QIYU_RONGLIAN_REQ:
		return DoQIYU_RONGLIAN_REQ(stFormatData);
	case MSG_LOGIC_QIYU_REFRESH_REQ:
		return DoQIYU_REFRESH_REQ(stFormatData);
	case MSG_LOGIC_QIYU_MOVE_REQ:
		return DoQIYU_MOVE_REQ(stFormatData);
	case MSG_LOGIC_QIYU_FINISH_REQ:
		return DoQIYU_FINISH_REQ(stFormatData);
	case MSG_LOGIC_QIYU_FIGHT_RESULT:
		return DoQIYU_FIGHT_RESULT(stFormatData);
	case MSG_LOGIC_QIYU_FIGHT_START:
		return DoQIYU_FIGHT_START(stFormatData);
	case MSG_LOGIC_QIYU_CANGBAOTU_DUIHUAN_REQ:
		return DoQIYU_CANGBAOTU_DUIHUAN_REQ(stFormatData);
	case MSG_LOGIC_QIYU_CANGBAOTU_USE_REQ:
		return DoQIYU_CANGBAOTU_USE_REQ(stFormatData);
	}
    
    return (true);
}

bool CLogicQiyuProcessor::DoQIYU_GET_INFO_REQ(const msgpack::unpacked& stFormatData)
{
	CLogicUserNotifySystem::NotifyQiyuData(m_pUserInfo);

	return true;
}

bool CLogicQiyuProcessor::DoQIYU_CANGBAOTU_DUIHUAN_REQ(const msgpack::unpacked& stFormatData)
{
	CRequestQiyuCangbaotuDuihuan stReq;
	if (!DecodeMsgPack(stFormatData, stReq)) return (false);

	//找到玩家的该藏宝图碎片信息
	int suipianBitmap = 0;
	auto userCangbaotusuipian = m_pUserInfo->m_stUserInfoTable.GetCangbaotuSuipian();
	auto suipianIter = userCangbaotusuipian.m_mpCangbaotuSuipian.find(stReq.m_iCangbaotuID);
	if (suipianIter != userCangbaotusuipian.m_mpCangbaotuSuipian.end())
		suipianBitmap = suipianIter->second;

	//2.如果碎片未满，则返回失败
	if (suipianBitmap != 0x000001ff)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::QIYU_CANGBAOTUSUIPIAN_NOT_FULL);
	}

	//如果碎片集齐，则修改碎片和藏宝图信息
	std::vector<CPackGameItem> gameItems;
    gameItems.emplace_back(CLogicConfigDefine::EXCHANGE_ITEM, stReq.m_iCangbaotuID, 1);

	if (CLogicCacheCommon::AddGameItem(m_pUserInfo, gameItems, m_usCmd))
	{
		auto newUserCangbaotusuipian = userCangbaotusuipian;
		newUserCangbaotusuipian.m_mpCangbaotuSuipian[stReq.m_iCangbaotuID] = 0;
		m_pUserInfo->m_stUserInfoTable.SetCangbaotuSuipian(newUserCangbaotusuipian);

		CNotifyCangbaotuSuipian suipianNotice;
		for (auto &newSuipianIter : newUserCangbaotusuipian.m_mpCangbaotuSuipian)
		{
			suipianNotice.m_mpCangbaotuSuipian[newSuipianIter.first] = int2bin<uint32_t>(newSuipianIter.second);
		}
		CLogicUserNotifySystem::NotifyCangbaotuSuipianData(m_pUserInfo, suipianNotice);

        CResponseQiyuCangbaotuHeCheng stRsp;
		stRsp.m_stItems = std::move(gameItems);
		SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
	}
	else
	{
		SEND_ERRCODE_AND_RET(CLogicError::GetErrCode());
	}
	return true;
}

bool CLogicQiyuProcessor::DoQIYU_CANGBAOTU_USE_REQ(const msgpack::unpacked& stFormatData)
{
	CRequestQiyuCangbaotuUse stReq;
	if (!DecodeMsgPack(stFormatData, stReq)) return (false);

	//判断等级
	if (m_pUserInfo->m_stUserInfoTable.GetLevel() < CLogicConfig::GetQiyuConfig().m_iOpenLevel)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ROLE_LEVEL);
	}

	//1.是否存在该道具
	if (false == CLogicCacheCommon::CanConsume(m_pUserInfo, CLogicConfigDefine::EXCHANGE_ITEM, stReq.m_iCangbaotuID, 1, m_usCmd))
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ITEM);
	}

	//2.现在不存在藏宝图奇遇
	CLogicActive::UpdateUserQiyu(m_pUserInfo);

	if (m_pUserInfo->m_stUserInfoTable.GetCangbaotuQiyuID() != 0)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::QIYU_HAVE_CANGBAOTU_QIYU);
	}

	//3.找到配置
	// auto pConf = CLogicConfig::GetItemConfig().GetConfig(stReq.m_iCangbaotuID);
	// if ((pConf == nullptr) || (pConf->m_aMapID.empty()))
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
	}

	//4.使用成功，消耗道具，初始化藏宝图奇遇,通知奇遇信息
	CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, CLogicConfigDefine::EXCHANGE_ITEM, stReq.m_iCangbaotuID, 1, m_usCmd);
	// int randomNum = CLogicCommon::GetRandNum(0, pConf->m_aMapID.size());
	// CLogicActive::InitNewQiyuMap(m_pUserInfo, false, pConf->m_aMapID[randomNum]);

	CLogicUserNotifySystem::NotifyQiyuData(m_pUserInfo);

	//5.返回
	CResponseQiyuCangbaotuDuihuan stRsp;
	stRsp.m_stDuihuanItem.m_iItemType = CLogicConfigDefine::EXCHANGE_ITEM;
	stRsp.m_stDuihuanItem.m_iCardID = stReq.m_iCangbaotuID;
	stRsp.m_stDuihuanItem.m_iNum = 1;

	logic_game_user_action_channel_data_t stData;
	stData.m_iTimes = 1;
	stData.m_iAction = CLogicGameChannelEnum::ACTION_OPEN_CANGBAOTU_COUNT;
	LOGIC_PUBLISHER_INS->PublishGameUserActionInfo(m_pUserInfo, stData);

	SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicQiyuProcessor::DoQIYU_RONGLIAN_REQ(const msgpack::unpacked& stFormatData)
{
	CRequestQiyuRonglian stReq;
	if (!DecodeMsgPack(stFormatData, stReq)) return (false);

	CLogicActive::UpdateUserQiyu(m_pUserInfo);

	//1.找到该地图的该结点，类型是否为熔炉，状态是否为已打开，熔炼值是否已满
	auto pMap = m_pUserInfo->m_stQiyuMap.find(stReq.m_iMapID);
	if (pMap == m_pUserInfo->m_stQiyuMap.end())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::QIYU_MAP_NOT_EXIST);
	}

	auto pNode = pMap->second.Find(stReq.m_iNodeID);
	if (pNode == pMap->second.End())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
	}

	if (pNode->second->GetNodeEventType() != QIYU_RONGLU)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
	}

	if (pNode->second->GetNodeState() == 0)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::QIYU_NODE_NOT_OPEN);
	}

	if (pNode->second->GetNodeState() == 2)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::QIYU_NODE_HAS_FINISHED);
	}

	std::vector<int32_t> intParams = pNode->second->GetParamsInt();
	if (intParams.size() < 2)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR);
	}

	if (intParams[1] >= intParams[0])
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::QIYU_MAX_RONGLIAN_VALUE);
	}	

	//3.找到该物品的熔炼配置
	auto pItemConf = CLogicConfig::GetQiyuConfig().m_mpItemRonglianValue.find(std::make_tuple(stReq.m_stSourceItem.m_iItemType, stReq.m_stSourceItem.m_iCardID));
	if (pItemConf == CLogicConfig::GetQiyuConfig().m_mpItemRonglianValue.end())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
	}

	int needItemNum = (intParams[0] - intParams[1]) / (pItemConf->second) + (((intParams[0] - intParams[1]) % (pItemConf->second)) == 0 ? 0 : 1);
	if (stReq.m_stSourceItem.m_iNum > needItemNum)
		stReq.m_stSourceItem.m_iNum = needItemNum;

	//2.判断该物品原料是否足够
	if (CLogicCacheCommon::CanConsume(m_pUserInfo, stReq.m_stSourceItem.m_iItemType, stReq.m_stSourceItem.m_iCardID, stReq.m_stSourceItem.m_iNum, m_usCmd) == false)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(NOT_ENOUGH_ITEM);
		return false;
	}

	//4.熔炼成功，消耗物品，增加该结点的熔炼值；
	CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, stReq.m_stSourceItem.m_iItemType, stReq.m_stSourceItem.m_iCardID, stReq.m_stSourceItem.m_iNum, m_usCmd);
	intParams[1] += (pItemConf->second) * stReq.m_stSourceItem.m_iNum;
	pNode->second->SetParamsInt(intParams);

	CResponseQiyuCangbaotuDuihuan stRsp;
	stRsp.m_stDuihuanItem = stReq.m_stSourceItem;
	SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicQiyuProcessor::DoQIYU_FIGHT_START(const msgpack::unpacked& stFormatData)
{
	CPackQiyuFightStart stReq;
	if (!DecodeMsgPack(stFormatData, stReq)) return (false);

	CLogicActive::UpdateUserQiyu(m_pUserInfo);

	//参数判断
	if (!CLogicCacheCommon::CheckTeamMemberValid(m_pUserInfo, stReq.m_stTeam))
	{
		SEND_ERRCODE_AND_RET(CLogicError::GetErrCode());
	}

	if (stReq.m_iMapID == m_pUserInfo->m_stUserInfoTable.GetDayQiyuID())
	{
        for(auto cardID : stReq.m_stTeam)
        {
            if(cardID == 0) continue;

            auto iterHero = m_pUserInfo->m_stHeroCardTableMap.Find(cardID);
            if(iterHero == m_pUserInfo->m_stHeroCardTableMap.End() || iterHero->second->GetQiyuFightCostHp() >= 100)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::QIYU_HERO_DEAD);
            }
        }
	}
	else
	{
        for(auto cardID : stReq.m_stTeam)
        {
            if(cardID == 0) continue;

            auto iterHero = m_pUserInfo->m_stHeroCardTableMap.Find(cardID);
            if(iterHero == m_pUserInfo->m_stHeroCardTableMap.End() || iterHero->second->GetCangbaotuFightCostHp() >= 100)
            {
                SEND_ERRCODE_AND_RET(SERVER_ERRCODE::QIYU_HERO_DEAD);
            }
        }
	}

	//1.找到对应的结点
	auto pMap = m_pUserInfo->m_stQiyuMap.find(stReq.m_iMapID);
	if (pMap == m_pUserInfo->m_stQiyuMap.end())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::QIYU_MAP_NOT_EXIST);
	}

	auto pNode = pMap->second.Find(stReq.m_iNodeID);
	if (pNode == pMap->second.End())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
	}

	//2.判断结点类型为战斗结点，且结点状态为已打开
	int nodeEventType = pNode->second->GetNodeEventType();
	if ((nodeEventType != QIYU_SHOUWEI) && \
		(nodeEventType != QIYU_MAOZEI) && \
		(nodeEventType != QIYU_QIANGZHETIAOZHAN))
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR);
	}

	if (pNode->second->GetNodeState() == 0)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::QIYU_NODE_NOT_OPEN);
	}

	if (pNode->second->GetNodeState() == 2)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::QIYU_NODE_HAS_FINISHED);
	}

	//3.如果是宝藏守卫，判断消耗是否足够
	if (nodeEventType == QIYU_SHOUWEI)
	{
		auto pShouweiConf = CLogicConfig::GetQiyuConfig().m_mpBaozangshouwei.find(pNode->second->GetNodeEventID());
		if (pShouweiConf == CLogicConfig::GetQiyuConfig().m_mpBaozangshouwei.end())
		{
			SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
		}

		for (auto &consumeIter : pShouweiConf->second.m_aConsumeItem)
		{
			if (false == CLogicCacheCommon::CanConsume(m_pUserInfo, consumeIter.m_iItemType, consumeIter.m_iCardID, consumeIter.m_iNum, m_usCmd))
				SEND_ERRCODE_AND_RET(SERVER_ERRCODE::NOT_ENOUGH_ITEM);
		}
		CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, pShouweiConf->second.m_aConsumeItem, m_usCmd);
	}

	//4.设置队伍
	if (stReq.m_iMapID == m_pUserInfo->m_stUserInfoTable.GetDayQiyuID())
	{
        CLogicCacheCommon::SetTeam(m_pUserInfo, CLogicConfigDefine::ETT_QiyuTeam, stReq.m_stTeam);
	}
	else
	{
        CLogicCacheCommon::SetTeam(m_pUserInfo, CLogicConfigDefine::ETT_CangbaotuTeam, stReq.m_stTeam);
	}

	//4. 返回
	CResponseQiyuFightStart stRsp;
	stRsp.m_iMapID = stReq.m_iMapID;
	stRsp.m_iNodeID = stReq.m_iNodeID;
	SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicQiyuProcessor::DoQIYU_FIGHT_RESULT(const msgpack::unpacked& stFormatData)
{
	CRequestQiyuFightResult stReq;
	if (!DecodeMsgPack(stFormatData, stReq)) return (false);

	CLogicActive::UpdateUserQiyu(m_pUserInfo);

	//1.找到对应的结点
	auto pMap = m_pUserInfo->m_stQiyuMap.find(stReq.m_iMapID);
	if (pMap == m_pUserInfo->m_stQiyuMap.end())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::QIYU_MAP_NOT_EXIST);
	}

	auto pNode = pMap->second.Find(stReq.m_iNodeID);
	if (pNode == pMap->second.End())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
	}

	//2.判断结点类型为战斗结点，且结点状态为已打开
	int nodeEventType = pNode->second->GetNodeEventType();
	if ((nodeEventType != QIYU_SHOUWEI) && \
		(nodeEventType != QIYU_MAOZEI) && \
		(nodeEventType != QIYU_QIANGZHETIAOZHAN))
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR);
	}

	if (pNode->second->GetNodeState() == 0)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::QIYU_NODE_NOT_OPEN);
	}

	if (pNode->second->GetNodeState() == 2)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::QIYU_NODE_HAS_FINISHED);
	}

	//3.更新结点的怪物血量，和自身伙伴血量
	std::vector<int> params = pNode->second->GetParamsInt();
	switch (nodeEventType)
	{
	case QIYU_QIANGZHETIAOZHAN:
		params.resize(3);
		params.insert(params.end(), stReq.m_stCostHP.begin(), stReq.m_stCostHP.end());
		break;
	case QIYU_SHOUWEI:
		params.resize(2);
		params.insert(params.end(), stReq.m_stCostHP.begin(), stReq.m_stCostHP.end());
		break;
	case QIYU_MAOZEI:
		params.resize(4);
		params.insert(params.end(), stReq.m_stCostHP.begin(), stReq.m_stCostHP.end());
		break;
	}
	pNode->second->SetParamsInt(params);

	if (stReq.m_iMapID == m_pUserInfo->m_stUserInfoTable.GetDayQiyuID())
	{
		for (auto &costHpIter : stReq.m_stUserCostHP)
		{
			if ((costHpIter.second >= 0) && (costHpIter.second <= 100))
			{
				auto heroIter = m_pUserInfo->m_stHeroCardTableMap.Find(costHpIter.first);
				if (heroIter != m_pUserInfo->m_stHeroCardTableMap.End())
				{
					CLogicCacheCardOperation cardOperation(m_pUserInfo);
					if (!cardOperation.CheckHeroCardExist(heroIter->first))
						continue;

					heroIter->second->SetQiyuFightCostHp(costHpIter.second);
				}
			}
		}
	}
	else
	{
		for (auto &costHpIter : stReq.m_stUserCostHP)
		{
			if ((costHpIter.second >= 0) && (costHpIter.second <= 100))
			{
				auto heroIter = m_pUserInfo->m_stHeroCardTableMap.Find(costHpIter.first);
				if (heroIter != m_pUserInfo->m_stHeroCardTableMap.End())
				{
					CLogicCacheCardOperation cardOperation(m_pUserInfo);
					if (!cardOperation.CheckHeroCardExist(heroIter->first))
						continue;

					heroIter->second->SetCangbaotuFightCostHp(costHpIter.second);
				}
			}
		}
	}

	//4.返回
	CResponseQiyuFightStart stRsp;
	stRsp.m_iMapID = stReq.m_iMapID;
	stRsp.m_iNodeID = stReq.m_iNodeID;
	SendToClient(stRsp, SERVER_ERRCODE::SUCCESS);
	return true;
}

bool CLogicQiyuProcessor::DoQIYU_REFRESH_REQ(const msgpack::unpacked& stFormatData)
{
#if LOGIC_BUILD_PLATFORM == BUILD_PLATFORM_LOCAL
	CRequestQiyuRefresh stReq;
	if (!DecodeMsgPack(stFormatData, stReq)) return (false);

	CLogicActive::RefreshQiyuMap(m_pUserInfo, stReq.m_bDayQiyuOrCangbaotu, stReq.m_iMapID);

	CLogicUserNotifySystem::NotifyQiyuData(m_pUserInfo);

	SendToClient(SERVER_ERRCODE::SUCCESS);
#endif
	return true;
}

bool CLogicQiyuProcessor::DoQIYU_MOVE_REQ(const msgpack::unpacked& stFormatData)
{
	CRequestQiyuMove stReq;
	if (!DecodeMsgPack(stFormatData, stReq)) return (false);

	CLogicActive::UpdateUserQiyu(m_pUserInfo);

	//1.判断该位置状态是否为 未打开 或 已打开;如果结点状态是未打开，判断该位置前置结点是否状态是已完成
	auto pMap = m_pUserInfo->m_stQiyuMap.find(stReq.m_iMapID);
	if (pMap == m_pUserInfo->m_stQiyuMap.end())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::QIYU_MAP_NOT_EXIST);
	}

	auto pNode = pMap->second.Find(stReq.m_iNodeID);
	if (pNode == pMap->second.End())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
	}

	if (pNode->second->GetNodeState() == 2)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::QIYU_NODE_HAS_FINISHED);
	}

	CNotifyQiyuEvent notice;
	if (pNode->second->GetNodeState() == 0)
	{
		auto pMapConf = CLogicConfig::GetQiyuConfig().m_mpQiyuMap.find(stReq.m_iMapID);
		if (pMapConf == CLogicConfig::GetQiyuConfig().m_mpQiyuMap.end())
			SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);
		auto pNodeConf = pMapConf->second.m_mpMapNodes.find(stReq.m_iNodeID);
		if (pNodeConf == pMapConf->second.m_mpMapNodes.end())
			SEND_ERRCODE_AND_RET(SERVER_ERRCODE::COMMON_CONFIG_NOT_EXIST);

		//later:临时屏蔽，方便测试
		if (pNodeConf->second.m_iPreID != 0)
		{
			auto pPreNode = pMap->second.Find(pNodeConf->second.m_iPreID);
			if ((pPreNode == pMap->second.End()) || (pPreNode->second->GetNodeState() != 2))
				SEND_ERRCODE_AND_RET(SERVER_ERRCODE::QIYU_NODE_CANNOT_MOVE);
		}

		//2.如果是首次打开，判断事件，生成事件参数,该结点状态及信息更新		
		if (false == GenEventParam(pMapConf->second, pNode->second->GetNodeEventType(), pNode->second->GetNodeEventID(), stReq.m_aParams, notice.m_aParams, notice.m_aStrParams))
		{
			SEND_ERRCODE_AND_RET(CLogicError::GetErrCode());
		}

		pNode->second->SetNodeState(1);
		pNode->second->SetParamsInt(notice.m_aParams);
		pNode->second->SetParamsStr(notice.m_aStrParams);
	}

	//3.跳转成功；事件通知前端；	
	notice.m_iEventType = pNode->second->GetNodeEventType();
	notice.m_iEventID = pNode->second->GetNodeEventID();
	notice.m_aParams = pNode->second->GetParamsInt();
	notice.m_aStrParams = pNode->second->GetParamsStr();

	CLogicUserNotifySystem::NotifyQiyuEvent(m_pUserInfo, notice);

	//4.返回
	SendToClient(SERVER_ERRCODE::SUCCESS);

	return true;
}

bool CLogicQiyuProcessor::DoQIYU_FINISH_REQ(const msgpack::unpacked& stFormatData)
{
	CRequestQiyuFinish stReq;
	if (!DecodeMsgPack(stFormatData, stReq)) return (false);

	CLogicActive::UpdateUserQiyu(m_pUserInfo);

	//1.判断该结点状态是否为 已打开
	auto pMap = m_pUserInfo->m_stQiyuMap.find(stReq.m_iMapID);
	if (pMap == m_pUserInfo->m_stQiyuMap.end())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::QIYU_MAP_NOT_EXIST);
	}

	auto pNode = pMap->second.Find(stReq.m_iNodeID);
	if (pNode == pMap->second.End())
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::PARAMETER_ERROR);
	}

	if (pNode->second->GetNodeState() != 1)
	{
		SEND_ERRCODE_AND_RET(SERVER_ERRCODE::INTERN_ERROR);
	}

	bool dayQiyuOrCangbaotu = (stReq.m_iMapID == m_pUserInfo->m_stUserInfoTable.GetDayQiyuID()) ? true : false;

	//2.发放奖励，置该结点状态为已完成
	CNotifyQiyuReward notice;
	notice.m_iMapID = stReq.m_iMapID;
	notice.m_iNodeID = stReq.m_iNodeID;

	bool ret = false;
	switch (pNode->second->GetNodeEventType())
	{
	case QIYU_HONGYUN:
	case QIYU_HIDEHONGYUN:
		ret = FinishHongyunEvent(pNode->second, dayQiyuOrCangbaotu, notice);
		break;
	case QIYU_MINGYUN:
		ret = FinishMingyunEvent(pNode->second, dayQiyuOrCangbaotu, stReq.m_aParams, notice);
		break;
	case QIYU_MITI:
		ret = FinishMitiEvent(pNode->second, dayQiyuOrCangbaotu, stReq.m_aParams, notice);
		break;
	case QIYU_ZHIYU:
		ret = FinishZhiyuEvent(pNode->second, dayQiyuOrCangbaotu, notice);
		break;
	case QIYU_ZHANZHENG:
		ret = FinishZhanzhengEvent(pNode->second, dayQiyuOrCangbaotu, notice);
		break;
	case QIYU_BAOZANG:
		ret = FinishBaozangEvent(pNode->second, dayQiyuOrCangbaotu, notice);
		break;
	case QIYU_JIAOYI:
		ret = FinishJiaoyiEvent(pNode->second, dayQiyuOrCangbaotu, stReq.m_aParams, notice);
		break;
	case QIYU_JINGJIE:
		ret = FinishJingjieEvent(pNode->second, dayQiyuOrCangbaotu, stReq.m_aParams, notice);
		break;
	case QIYU_RONGLU:
		ret = FinishRonglianEvent(pNode->second, dayQiyuOrCangbaotu, notice);
		break;
	case QIYU_MIBAOKU:
		ret = FinishMibaokuEvent(pNode->second, dayQiyuOrCangbaotu, notice);
		break;
	case QIYU_NORMALREWARD:
		ret = FinishNormalRewardEvent(pNode->second, dayQiyuOrCangbaotu, notice);
		break;
	case QIYU_QIANGZHETIAOZHAN:
		ret = FinishQiangzheEvent(pNode->second, dayQiyuOrCangbaotu, notice);
		break;
	case QIYU_SHOUWEI:
		ret = FinishShouweiEvent(pNode->second, dayQiyuOrCangbaotu, notice);
		break;
	case QIYU_MAOZEI:
		ret = FinishMaozeiEvent(pNode->second, dayQiyuOrCangbaotu, stReq.m_aParams, notice);
		break;
	case QIYU_MUBEI:
		ret = FinishMubeiEvent(pNode->second, dayQiyuOrCangbaotu, stReq.m_iMapID, notice);
		break;
	}

	//4.返回
	if (ret == true)
	{
		//交易所,秘宝窟特殊处理，内部处理状态
		if ((pNode->second->GetNodeEventType() != QIYU_JIAOYI) && (pNode->second->GetNodeEventType() != QIYU_MIBAOKU))
			pNode->second->SetNodeState(2);

		SendToClient(notice, SERVER_ERRCODE::SUCCESS);

		//如果是藏宝图地图，判断是否所有节点都已完成，如果是，则关闭藏宝图地图,并通知
		if (dayQiyuOrCangbaotu == false)
		{
			bool allFinish = true;
			for (auto nodeIter = pMap->second.Begin(); nodeIter != pMap->second.End(); ++nodeIter)
			{
				if (nodeIter->second->GetNodeState() != 2)
				{
					allFinish = false;
					break;
				}
			}
			if (allFinish == true)
			{
				CLogicActive::ClearQiyuMap(m_pUserInfo, dayQiyuOrCangbaotu);

				CLogicUserNotifySystem::NotifyQiyuData(m_pUserInfo);
			}
		}
	}
	else
		SEND_ERRCODE_AND_RET(CLogicError::GetErrCode());

	return true;
}

bool CLogicQiyuProcessor::GenEventParam(const stQiyuMapData &mapData, int eventType, int eventID, const std::vector<int> &inputParams, std::vector<int> &params, std::vector<std::string> &strParams)
{
	params.clear();
	strParams.clear();

	switch (eventType)
	{
	case QIYU_HONGYUN:
	case QIYU_HIDEHONGYUN:
		return GenHongyunParam(mapData, eventType, eventID, params, strParams);
		break;
	case QIYU_MINGYUN:
		return GenMingyunParam(mapData, eventType, eventID, params, strParams);
		break;
	case QIYU_MITI:
		return GenMitiParam(mapData, eventType, eventID, params, strParams);
		break;
	case QIYU_ZHIYU:
		return GenZhiyuParam(mapData, eventType, eventID, params, strParams);
		break;
	case QIYU_ZHANZHENG:
		return GenZhanzhengParam(mapData, eventType, eventID, params, strParams);
		break;
	case QIYU_BAOZANG:
		return GenBaozangParam(mapData, eventType, eventID, params, strParams);
		break;
	case QIYU_JIAOYI:
		return GenJiaoyiParam(mapData, eventType, eventID, params, strParams);
		break;
	case QIYU_JINGJIE:
		return GenJingjieParam(mapData, eventType, eventID, inputParams, params, strParams);
		break;
	case QIYU_RONGLU:
		return GenRonglianParam(mapData, eventType, eventID, params, strParams);
		break;
	case QIYU_MIBAOKU:
		return GenMibaokuParam(mapData, eventType, eventID, params, strParams);
		break;
	case QIYU_NORMALREWARD:
		return GenNormalRewardParam(mapData, eventType, eventID, params, strParams);
		break;
	case QIYU_QIANGZHETIAOZHAN:
		return GenQiangzheParam(mapData, eventType, eventID, params, strParams);
		break;
	case QIYU_SHOUWEI:
		return GenShouweiParam(mapData, eventType, eventID, params, strParams);
		break;
	case QIYU_MAOZEI:
		return GenMaozeiParam(mapData, eventType, eventID, params, strParams);
		break;
	case QIYU_MUBEI:
		return GenMubeiParam(mapData, eventType, eventID, params, strParams);
		break;
	}

	return true;
}

bool CLogicQiyuProcessor::GenHongyunParam(const stQiyuMapData &mapData, int eventType, int eventID, std::vector<int> &params, std::vector<std::string> &strParams)
{
	params.clear();
	strParams.clear();

	do
	{
		if (eventType == 1)
		{
			//1.奖励组
			auto eventIter = CLogicConfig::GetQiyuConfig().m_mpHongyun.find(eventID);
			if ((eventIter == CLogicConfig::GetQiyuConfig().m_mpHongyun.end()) || (CLogicConfig::GetQiyuConfig().m_aHongyunContent.empty()))
				break;

			std::string rewardGroupName = RandomRewardGroup(m_pUserInfo->m_stUserInfoTable.GetLevel(), eventIter->second);
			if (!rewardGroupName.empty())
				strParams.push_back(rewardGroupName);

			//2.文本
			int randomNum = CLogicCommon::GetRandNum(0, CLogicConfig::GetQiyuConfig().m_aHongyunContent.size());
			params.push_back(CLogicConfig::GetQiyuConfig().m_aHongyunContent[randomNum]);
		}
		else if (eventType == 16)
		{
			//1.奖励组
			auto eventIter = CLogicConfig::GetQiyuConfig().m_mpHideHongyun.find(eventID);
			if ((eventIter == CLogicConfig::GetQiyuConfig().m_mpHideHongyun.end()) || (CLogicConfig::GetQiyuConfig().m_aHideHongyunContent.empty()))
				break;

			std::string rewardGroupName = RandomRewardGroup(m_pUserInfo->m_stUserInfoTable.GetLevel(), eventIter->second);
			if (!rewardGroupName.empty())
				strParams.push_back(rewardGroupName);

			//2.文本
			int randomNum = CLogicCommon::GetRandNum(0, CLogicConfig::GetQiyuConfig().m_aHideHongyunContent.size());
			params.push_back(CLogicConfig::GetQiyuConfig().m_aHideHongyunContent[randomNum]);
		}
	} while (false);

	if ((params.size() != 1) || (strParams.size() != 1))
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	return true;
}

bool CLogicQiyuProcessor::FinishHongyunEvent(TUserQiyuTable *node, bool dayQiyuOrCangbaotu, CNotifyQiyuReward &notice)
{
	if (node == nullptr)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	std::vector<std::string> params = node->GetParamsStr();
	if (!params.empty())
	{
		auto rewardGroupIter = CLogicConfig::GetQiyuConfig().m_mpRewardGroup.find(params[0]);
		if (rewardGroupIter != CLogicConfig::GetQiyuConfig().m_mpRewardGroup.end())
		{
			CLogicCacheCommon::AddGameItem(m_pUserInfo, rewardGroupIter->second.m_aRewards, m_usCmd);

			notice.m_stItemVec = rewardGroupIter->second.m_aRewards;

			//战报
			for (auto fightReportIter = CLogicConfig::GetFightReportConfig().m_stConfigMap.lower_bound("qiyu"); fightReportIter != CLogicConfig::GetFightReportConfig().m_stConfigMap.upper_bound("qiyu"); ++fightReportIter)
			{
				if (fightReportIter->second.m_iTriggerValue == node->GetNodeEventType())
				{
					CPackFightReportItem reportItem;
					reportItem.m_iReportType = dayQiyuOrCangbaotu ? CLogicConfigDefine::FIGHT_REPORT_QIYU : CLogicConfigDefine::FIGHT_REPORT_CANGBAOTU;
					reportItem.m_iReportID = CLogicCommon::GetSec();
					std::map<std::string, std::string> stParamMap;
					stParamMap["reward"] = CLogicConfig::GenRewardContent(rewardGroupIter->second.m_aRewards);

					reportItem.m_strReportContent = fightReportIter->second.m_stContentObj.Replace(stParamMap);
                    CLogicCacheCommon::AddUserFightReport(m_pUserInfo, reportItem);

					break;
				}
			}
		}
	}

	return true;
}

bool CLogicQiyuProcessor::GenMitiParam(const stQiyuMapData &mapData, int eventType, int eventID, std::vector<int> &params, std::vector<std::string> &strParams)
{
	params.clear();
	strParams.clear();

	//1.题目
	int randomNum = CLogicCommon::GetRandNum(0, CLogicConfig::GetQiyuConfig().m_mpMitiContent.size());
	auto mitiIter = CLogicConfig::GetQiyuConfig().m_mpMitiContent.begin();
	std::advance(mitiIter, randomNum);
	params.push_back(mitiIter->first);
	params.push_back(mitiIter->second);

	if ((params.size() != 2) || (strParams.size() != 0))
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	return true;
}

bool CLogicQiyuProcessor::FinishMitiEvent(TUserQiyuTable *node, bool dayQiyuOrCangbaotu, const std::vector<int> &params, CNotifyQiyuReward &notice)
{
	if (node == nullptr)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	//1.判断结果是否正确
	std::vector<int> intParams = node->GetParamsInt();

	if ((params.empty()) || (intParams.size() != 2))
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	auto mitiIter = CLogicConfig::GetQiyuConfig().m_mpMiti.find(node->GetNodeEventID());
	if (mitiIter != CLogicConfig::GetQiyuConfig().m_mpMiti.end())
	{
		//2.如果正确，则发放奖励
		if (params[0] == intParams[1])
		{
			std::string rewardGroupName = RandomRewardGroup(m_pUserInfo->m_stUserInfoTable.GetLevel(), mitiIter->second.m_strLevelRewardPool);
			auto rewardGroupIter = CLogicConfig::GetQiyuConfig().m_mpRewardGroup.find(rewardGroupName);
			if (rewardGroupIter != CLogicConfig::GetQiyuConfig().m_mpRewardGroup.end())
			{
				CLogicCacheCommon::AddGameItem(m_pUserInfo, rewardGroupIter->second.m_aRewards, m_usCmd);

				notice.m_stItemVec = rewardGroupIter->second.m_aRewards;

				//战报
				for (auto fightReportIter = CLogicConfig::GetFightReportConfig().m_stConfigMap.lower_bound("qiyu"); fightReportIter != CLogicConfig::GetFightReportConfig().m_stConfigMap.upper_bound("qiyu"); ++fightReportIter)
				{
					if (fightReportIter->second.m_iTriggerValue == 301)
					{
						CPackFightReportItem reportItem;
						reportItem.m_iReportType = dayQiyuOrCangbaotu ? CLogicConfigDefine::FIGHT_REPORT_QIYU : CLogicConfigDefine::FIGHT_REPORT_CANGBAOTU;
						reportItem.m_iReportID = CLogicCommon::GetSec();
						reportItem.m_cFightResult = 1;

						std::map<std::string, std::string> stParamMap;
						stParamMap["reward"] = CLogicConfig::GenRewardContent(rewardGroupIter->second.m_aRewards);
						reportItem.m_strReportContent = fightReportIter->second.m_stContentObj.Replace(stParamMap);
                        CLogicCacheCommon::AddUserFightReport(m_pUserInfo, reportItem);

						break;
					}
				}
			}
		}
		//3.如果错误，则做buff处理
		else
		{
			notice.m_stBuff.m_iType = mitiIter->second.m_iBuffType;
			notice.m_stBuff.m_iPercentage = mitiIter->second.m_iBuffPercent;

			GenQiyuBuff(dayQiyuOrCangbaotu, notice.m_stBuff);

			if (notice.m_stBuff.m_iType == 7)
			{
				for (auto stIter = m_pUserInfo->m_stHeroCardTableMap.Begin();
					stIter != m_pUserInfo->m_stHeroCardTableMap.End();
					++stIter)
				{
					CLogicCacheCardOperation cardOperation(m_pUserInfo);
					if (!cardOperation.CheckHeroCardExist(stIter->first))
						continue;

					if (dayQiyuOrCangbaotu)
					{
						notice.m_stUserCostHP.emplace_back(stIter->first, stIter->second->GetQiyuFightCostHp());
					}
					else
					{
						notice.m_stUserCostHP.emplace_back(stIter->first, stIter->second->GetCangbaotuFightCostHp());
					}
				}
			}

			//战报
			for (auto fightReportIter = CLogicConfig::GetFightReportConfig().m_stConfigMap.lower_bound("qiyu"); fightReportIter != CLogicConfig::GetFightReportConfig().m_stConfigMap.upper_bound("qiyu"); ++fightReportIter)
			{
				if (fightReportIter->second.m_iTriggerValue == 300)
				{
					CPackFightReportItem reportItem;
					reportItem.m_iReportType = dayQiyuOrCangbaotu ? CLogicConfigDefine::FIGHT_REPORT_QIYU : CLogicConfigDefine::FIGHT_REPORT_CANGBAOTU;
					reportItem.m_iReportID = CLogicCommon::GetSec();
					reportItem.m_cFightResult = 0;

					std::map<std::string, std::string> stParamMap;
					stParamMap["num"] = std::to_string(notice.m_stBuff.m_iPercentage);
					reportItem.m_strReportContent = fightReportIter->second.m_stContentObj.Replace(stParamMap);
                    CLogicCacheCommon::AddUserFightReport(m_pUserInfo, reportItem);

					break;
				}
			}
		}
	}

	return true;
}

bool CLogicQiyuProcessor::GenMingyunParam(const stQiyuMapData &mapData, int eventType, int eventID, std::vector<int> &params, std::vector<std::string> &strParams)
{
	params.clear();
	strParams.clear();

	//1.奖励
	int contentID = 0;
	auto pConf = CLogicConfig::GetQiyuConfig().m_mpMingyun.find(eventID);
	if (pConf != CLogicConfig::GetQiyuConfig().m_mpMingyun.end())
	{
		std::string levelRewardPoolName = pConf->second.m_strLevelRewardPool1;
		std::string rewardGroupName = RandomRewardGroup(m_pUserInfo->m_stUserInfoTable.GetLevel(), levelRewardPoolName);
		auto rewardGroupIter = CLogicConfig::GetQiyuConfig().m_mpRewardGroup.find(rewardGroupName);
		if (rewardGroupIter != CLogicConfig::GetQiyuConfig().m_mpRewardGroup.end())
		{
			strParams.push_back(rewardGroupName);
			contentID = rewardGroupIter->second.m_iExParam;
		}

		std::string rewardGroupName2 = rewardGroupIter->second.m_strBadRewardID;
		rewardGroupIter = CLogicConfig::GetQiyuConfig().m_mpRewardGroup.find(rewardGroupName2);
		if (rewardGroupIter != CLogicConfig::GetQiyuConfig().m_mpRewardGroup.end())
		{
			strParams.push_back(rewardGroupName2);
		}
	}

	//2.文本
	if (CLogicConfig::GetQiyuConfig().m_aMingyunContent.find(contentID) != CLogicConfig::GetQiyuConfig().m_aMingyunContent.end())
	{
		params.push_back(contentID);
	}	

	if ((params.size() != 1) || (strParams.size() != 2))
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	return true;
}

bool CLogicQiyuProcessor::FinishMingyunEvent(TUserQiyuTable *node, bool dayQiyuOrCangbaotu, const std::vector<int> &params, CNotifyQiyuReward &notice)
{
	if ((node == nullptr) || (params.empty()))
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	//发放奖励
	auto pConf = CLogicConfig::GetQiyuConfig().m_mpMingyun.find(node->GetNodeEventID());
	if (pConf != CLogicConfig::GetQiyuConfig().m_mpMingyun.end())
	{
		std::vector<std::string> nodeParams = node->GetParamsStr();
		if (nodeParams.size() != 2)
		{
			using namespace SERVER_ERRCODE;
			SET_ERR_INFO(INTERN_ERROR);
			return false;
		}

		std::string rewardGroupName = (params[0] == 1) ? nodeParams[0] : nodeParams[1];		
		auto rewardGroupIter = CLogicConfig::GetQiyuConfig().m_mpRewardGroup.find(rewardGroupName);
		if (rewardGroupIter != CLogicConfig::GetQiyuConfig().m_mpRewardGroup.end())
		{
			CLogicCacheCommon::AddGameItem(m_pUserInfo, rewardGroupIter->second.m_aRewards, m_usCmd);

			notice.m_stItemVec = rewardGroupIter->second.m_aRewards;

			//战报
			for (auto fightReportIter = CLogicConfig::GetFightReportConfig().m_stConfigMap.lower_bound("qiyu"); fightReportIter != CLogicConfig::GetFightReportConfig().m_stConfigMap.upper_bound("qiyu"); ++fightReportIter)
			{
				if (fightReportIter->second.m_iTriggerValue == QIYU_MINGYUN)
				{
					CPackFightReportItem reportItem;
					reportItem.m_iReportType = dayQiyuOrCangbaotu ? CLogicConfigDefine::FIGHT_REPORT_QIYU : CLogicConfigDefine::FIGHT_REPORT_CANGBAOTU;
					reportItem.m_iReportID = CLogicCommon::GetSec();
					reportItem.m_cFightResult = 1;

					std::map<std::string, std::string> stParamMap;
					stParamMap["reward"] = CLogicConfig::GenRewardContent(rewardGroupIter->second.m_aRewards);
					reportItem.m_strReportContent = fightReportIter->second.m_stContentObj.Replace(stParamMap);
                    CLogicCacheCommon::AddUserFightReport(m_pUserInfo, reportItem);

					break;
				}
			}
		}
	}

	return true;
}

bool CLogicQiyuProcessor::GenZhiyuParam(const stQiyuMapData &mapData, int eventType, int eventID, std::vector<int> &params, std::vector<std::string> &strParams)
{
	params.clear();
	strParams.clear();

	if ((params.size() != 0) || (strParams.size() != 0))
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	return true;
}

bool CLogicQiyuProcessor::FinishZhiyuEvent(TUserQiyuTable *node, bool dayQiyuOrCangbaotu, CNotifyQiyuReward &notice)
{
	if (node == nullptr)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	auto zhiyuIter = CLogicConfig::GetQiyuConfig().m_mpZhiyu.find(node->GetNodeEventID());
	if (zhiyuIter != CLogicConfig::GetQiyuConfig().m_mpZhiyu.end())
	{		
		notice.m_stBuff.m_iType = zhiyuIter->second.m_iBuffType;
		notice.m_stBuff.m_iPercentage = zhiyuIter->second.m_iBuffPercent;

		GenQiyuBuff(dayQiyuOrCangbaotu, notice.m_stBuff);

		if (notice.m_stBuff.m_iType == 7)
		{
			for (auto stIter = m_pUserInfo->m_stHeroCardTableMap.Begin();
				stIter != m_pUserInfo->m_stHeroCardTableMap.End();
				++stIter)
			{
				CLogicCacheCardOperation cardOperation(m_pUserInfo);
				if (!cardOperation.CheckHeroCardExist(stIter->first))
					continue;

				if (dayQiyuOrCangbaotu)
				{
					notice.m_stUserCostHP.push_back(std::make_pair(stIter->first, stIter->second->GetQiyuFightCostHp()));
				}
				else
				{
					notice.m_stUserCostHP.push_back(std::make_pair(stIter->first, stIter->second->GetCangbaotuFightCostHp()));
				}
			}
		}

		//战报
		for (auto fightReportIter = CLogicConfig::GetFightReportConfig().m_stConfigMap.lower_bound("qiyu"); fightReportIter != CLogicConfig::GetFightReportConfig().m_stConfigMap.upper_bound("qiyu"); ++fightReportIter)
		{
			if (fightReportIter->second.m_iTriggerValue == QIYU_ZHIYU)
			{
				CPackFightReportItem reportItem;
				reportItem.m_iReportType = dayQiyuOrCangbaotu ? CLogicConfigDefine::FIGHT_REPORT_QIYU : CLogicConfigDefine::FIGHT_REPORT_CANGBAOTU;
				reportItem.m_iReportID = CLogicCommon::GetSec();
				reportItem.m_cFightResult = 1;

				std::map<std::string, std::string> stParamMap;
				reportItem.m_strReportContent = fightReportIter->second.m_stContentObj.Replace(stParamMap);
                CLogicCacheCommon::AddUserFightReport(m_pUserInfo, reportItem);

				break;
			}
		}
	}

	return true;
}

bool CLogicQiyuProcessor::GenZhanzhengParam(const stQiyuMapData &mapData, int eventType, int eventID, std::vector<int> &params, std::vector<std::string> &strParams)
{
	params.clear();
	strParams.clear();

	if ((params.size() != 0) || (strParams.size() != 0))
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	return true;
}

bool CLogicQiyuProcessor::FinishZhanzhengEvent(TUserQiyuTable *node, bool dayQiyuOrCangbaotu, CNotifyQiyuReward &notice)
{
	if (node == nullptr)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	auto zhanzhengIter = CLogicConfig::GetQiyuConfig().m_mpZhanzheng.find(node->GetNodeEventID());
	if (zhanzhengIter != CLogicConfig::GetQiyuConfig().m_mpZhanzheng.end())
	{
		notice.m_stBuff.m_iType = zhanzhengIter->second.m_iBuffType;
		notice.m_stBuff.m_iPercentage = zhanzhengIter->second.m_iBuffPercent;

		GenQiyuBuff(dayQiyuOrCangbaotu, notice.m_stBuff);

		//战报
		for (auto fightReportIter = CLogicConfig::GetFightReportConfig().m_stConfigMap.lower_bound("qiyu"); fightReportIter != CLogicConfig::GetFightReportConfig().m_stConfigMap.upper_bound("qiyu"); ++fightReportIter)
		{
			if (fightReportIter->second.m_iTriggerValue == QIYU_ZHANZHENG)
			{
				CPackFightReportItem reportItem;
				reportItem.m_iReportType = dayQiyuOrCangbaotu ? CLogicConfigDefine::FIGHT_REPORT_QIYU : CLogicConfigDefine::FIGHT_REPORT_CANGBAOTU;
				reportItem.m_iReportID = CLogicCommon::GetSec();
				reportItem.m_cFightResult = 1;

				std::map<std::string, std::string> stParamMap;
				stParamMap["num"] = std::to_string(notice.m_stBuff.m_iPercentage);
				reportItem.m_strReportContent = fightReportIter->second.m_stContentObj.Replace(stParamMap);
                CLogicCacheCommon::AddUserFightReport(m_pUserInfo, reportItem);

				break;
			}
		}
	}

	return true;
}

bool CLogicQiyuProcessor::GenBaozangParam(const stQiyuMapData &mapData, int eventType, int eventID, std::vector<int> &params, std::vector<std::string> &strParams)
{
	params.clear();
	strParams.clear();
	
	//1.奖励组
	auto eventIter = CLogicConfig::GetQiyuConfig().m_mpBaozang.find(eventID);
	if (eventIter != CLogicConfig::GetQiyuConfig().m_mpBaozang.end())
	{
		std::string rewardGroupName = RandomRewardGroup(m_pUserInfo->m_stUserInfoTable.GetLevel(), eventIter->second);
		if (!rewardGroupName.empty())
			strParams.push_back(rewardGroupName);
	}

	if ((params.size() != 0) || (strParams.size() != 1))
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	return true;
}

bool CLogicQiyuProcessor::FinishBaozangEvent(TUserQiyuTable *node, bool dayQiyuOrCangbaotu, CNotifyQiyuReward &notice)
{
	if (node == nullptr)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	std::vector<std::string> params = node->GetParamsStr();
	if (!params.empty())
	{
		auto rewardGroupIter = CLogicConfig::GetQiyuConfig().m_mpRewardGroup.find(params[0]);
		if (rewardGroupIter != CLogicConfig::GetQiyuConfig().m_mpRewardGroup.end())
		{
			CLogicCacheCommon::AddGameItem(m_pUserInfo, rewardGroupIter->second.m_aRewards, m_usCmd);

			notice.m_stItemVec = rewardGroupIter->second.m_aRewards;

			//战报
			for (auto fightReportIter = CLogicConfig::GetFightReportConfig().m_stConfigMap.lower_bound("qiyu"); fightReportIter != CLogicConfig::GetFightReportConfig().m_stConfigMap.upper_bound("qiyu"); ++fightReportIter)
			{
				if (fightReportIter->second.m_iTriggerValue == QIYU_BAOZANG)
				{
					CPackFightReportItem reportItem;
					reportItem.m_iReportType = dayQiyuOrCangbaotu ? CLogicConfigDefine::FIGHT_REPORT_QIYU : CLogicConfigDefine::FIGHT_REPORT_CANGBAOTU;
					reportItem.m_iReportID = CLogicCommon::GetSec();
					reportItem.m_cFightResult = 1;

					std::map<std::string, std::string> stParamMap;
					stParamMap["reward"] = CLogicConfig::GenRewardContent(notice.m_stItemVec);
					reportItem.m_strReportContent = fightReportIter->second.m_stContentObj.Replace(stParamMap);
                    CLogicCacheCommon::AddUserFightReport(m_pUserInfo, reportItem);

					break;
				}
			}
		}
	}

	return true;
}

bool CLogicQiyuProcessor::GenJiaoyiParam(const stQiyuMapData &mapData, int eventType, int eventID, std::vector<int> &params, std::vector<std::string> &strParams)
{
	params.clear();
	strParams.clear();

	auto eventIter = CLogicConfig::GetQiyuConfig().m_mpJiaoyisuo.find(eventID);
	if (eventIter != CLogicConfig::GetQiyuConfig().m_mpJiaoyisuo.end())
	{
		for (auto &iter1 : eventIter->second.m_aJiaoyiLevelPool)
		{
			if (!iter1.empty())
			{
				std::string jiaoyiGroupName = RandomJiaoyiGroup(m_pUserInfo->m_stUserInfoTable.GetLevel(), iter1);
				if (!jiaoyiGroupName.empty())
				{
					strParams.push_back(jiaoyiGroupName);
					params.push_back(0);
				}
			}
		}
	}

	if ((params.size() == 0) || (strParams.size() == 0))
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	return true;
}

bool CLogicQiyuProcessor::FinishJiaoyiEvent(TUserQiyuTable *node, bool dayQiyuOrCangbaotu, const std::vector<int> &params, CNotifyQiyuReward &notice)
{
	if (node == nullptr)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	if (params.empty())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	uint32_t jiaoyiIndex = params[0];
	std::vector<std::string> strParams = node->GetParamsStr();
	std::vector<int> intParams = node->GetParamsInt();

	if ((jiaoyiIndex >= strParams.size()) || (jiaoyiIndex >= intParams.size()))
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	if (intParams[jiaoyiIndex] == 1)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}


	auto pConfIter = CLogicConfig::GetQiyuConfig().m_mpJiaoyiItem.find(strParams[jiaoyiIndex]);
	if (pConfIter == CLogicConfig::GetQiyuConfig().m_mpJiaoyiItem.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(COMMON_CONFIG_NOT_EXIST);
		return false;
	}

	//判断原料是否足够
	for (auto &srcIter : pConfIter->second.m_aSourceItemVec)
	{
		if (!CLogicCacheCommon::CanConsume(m_pUserInfo, srcIter.m_iItemType, srcIter.m_iCardID, srcIter.m_iNum,
                                             m_usCmd))
		{
			using namespace SERVER_ERRCODE;
			SET_ERR_INFO(NOT_ENOUGH_ITEM);
			return false;
		}
	}

	//成功兑换
	CLogicCacheCommon::AddGameItem(m_pUserInfo, pConfIter->second.m_aDesItemVec, m_usCmd);
	CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, pConfIter->second.m_aSourceItemVec, m_usCmd);
	notice.m_stItemVec = pConfIter->second.m_aDesItemVec;

	//保存交易状态
	intParams[jiaoyiIndex] = 1;
	node->SetParamsInt(intParams);

	//交易所特殊处理，内部处理状态
	bool allFinish = true;
	for (size_t i = 0; i < intParams.size(); ++i)
	{
		if (intParams[i] == 0)
		{
			allFinish = false;
			break;
		}
	}
	if (allFinish)
	{
		node->SetNodeState(2);
	}

	//战报
	for (auto fightReportIter = CLogicConfig::GetFightReportConfig().m_stConfigMap.lower_bound("qiyu"); fightReportIter != CLogicConfig::GetFightReportConfig().m_stConfigMap.upper_bound("qiyu"); ++fightReportIter)
	{
		if (fightReportIter->second.m_iTriggerValue == QIYU_JIAOYI)
		{
			CPackFightReportItem reportItem;
			reportItem.m_iReportType = dayQiyuOrCangbaotu ? CLogicConfigDefine::FIGHT_REPORT_QIYU : CLogicConfigDefine::FIGHT_REPORT_CANGBAOTU;
			reportItem.m_iReportID = CLogicCommon::GetSec();
			reportItem.m_cFightResult = 1;

			std::map<std::string, std::string> stParamMap;
			stParamMap["reward"] = CLogicConfig::GenRewardContent(notice.m_stItemVec);
			stParamMap["item_name"] = CLogicConfig::GenRewardContent(pConfIter->second.m_aSourceItemVec);
			reportItem.m_strReportContent = fightReportIter->second.m_stContentObj.Replace(stParamMap);
            CLogicCacheCommon::AddUserFightReport(m_pUserInfo, reportItem);

			break;
		}
	}

	return true;
}

bool CLogicQiyuProcessor::GenJingjieParam(const stQiyuMapData &mapData, int eventType, int eventID, const std::vector<int> &inputParams, std::vector<int> &params, std::vector<std::string> &strParams)
{
	params.clear();
	strParams.clear();

	//1.随机伙伴
	std::multimap<int, int>	mpCombatHero;
	for (size_t i = 0; i + 1 < inputParams.size(); i += 2)
	{
		//later:临时特殊处理，不随机孙策
		if (inputParams[i] != 44)
		{
			mpCombatHero.insert(std::make_pair(inputParams[i + 1], inputParams[i]));
		}
	}

	if (mapData.m_iJingjieHeroNum > 0)
	{
		while (mpCombatHero.size() > (size_t)(mapData.m_iJingjieHeroNum))
		{
			mpCombatHero.erase(mpCombatHero.begin());
		}
	}

	if (mpCombatHero.empty())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	int randomNum = CLogicCommon::GetRandNum(0, mpCombatHero.size());
	auto heroIter = mpCombatHero.begin();
	std::advance(heroIter, randomNum);

	int heroID = heroIter->second;

	CLogicCacheCardOperation cardOperation(m_pUserInfo);
	if (!cardOperation.CheckHeroCardExist(heroID))
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	params.push_back(heroID);

	//2.随机文本
	auto pConf = CLogicConfig::GetHeroCardConfig().GetConfig(heroID);
	if (pConf != nullptr)
	{
		if (pConf->m_iSexual == 1)
		{
			if (!CLogicConfig::GetQiyuConfig().m_aMaleGanwuContent.empty())
			{
				randomNum = CLogicCommon::GetRandNum(0, CLogicConfig::GetQiyuConfig().m_aMaleGanwuContent.size());
				params.push_back(CLogicConfig::GetQiyuConfig().m_aMaleGanwuContent[randomNum]);
			}
		}
		else
		{
			if (!CLogicConfig::GetQiyuConfig().m_aFemaleGanwuContent.empty())
			{
				randomNum = CLogicCommon::GetRandNum(0, CLogicConfig::GetQiyuConfig().m_aFemaleGanwuContent.size());
				params.push_back(CLogicConfig::GetQiyuConfig().m_aFemaleGanwuContent[randomNum]);
			}
		}
	}

	if ((params.size() != 2) || (strParams.size() != 0))
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	return true;
}

bool CLogicQiyuProcessor::FinishJingjieEvent(TUserQiyuTable *node, bool dayQiyuOrCangbaotu, const std::vector<int> &params, CNotifyQiyuReward &notice)
{
	if ((node == nullptr)|| (params.empty()))
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	std::vector<int> intParams = node->GetParamsInt();
	if (intParams.empty())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	auto pConf = CLogicConfig::GetQiyuConfig().m_mpGanwuHero.find(intParams[0]);
	if (pConf == CLogicConfig::GetQiyuConfig().m_mpGanwuHero.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(COMMON_CONFIG_NOT_EXIST);
		return false;
	}	

	if (params[0] == 1)
	{		
		if (CLogicCacheCommon::CanConsume(m_pUserInfo, pConf->second.m_stConsumeItem1.m_iItemType, pConf->second.m_stConsumeItem1.m_iCardID, pConf->second.m_stConsumeItem1.m_iNum, m_usCmd) == false)
		{
			using namespace SERVER_ERRCODE;
			SET_ERR_INFO(NOT_ENOUGH_ITEM);
			return false;
		}

		//成功兑换
		CLogicCacheCommon::AddGameItem(m_pUserInfo, pConf->second.m_stReward1, m_usCmd);
		CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, pConf->second.m_stConsumeItem1, m_usCmd);
		notice.m_stItemVec.push_back(pConf->second.m_stReward1);
	}
	else
	{
		if (CLogicCacheCommon::CanConsume(m_pUserInfo, pConf->second.m_stConsumeItem2.m_iItemType, pConf->second.m_stConsumeItem2.m_iCardID, pConf->second.m_stConsumeItem2.m_iNum, m_usCmd) == false)
		{
			using namespace SERVER_ERRCODE;
			SET_ERR_INFO(NOT_ENOUGH_ITEM);
			return false;
		}

		//成功兑换
		CLogicCacheCommon::AddGameItem(m_pUserInfo, pConf->second.m_stReward2, m_usCmd);
		CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, pConf->second.m_stConsumeItem2, m_usCmd);
		notice.m_stItemVec.push_back(pConf->second.m_stReward2);

	}

	//战报
	for (auto fightReportIter = CLogicConfig::GetFightReportConfig().m_stConfigMap.lower_bound("qiyu"); fightReportIter != CLogicConfig::GetFightReportConfig().m_stConfigMap.upper_bound("qiyu"); ++fightReportIter)
	{
		if (fightReportIter->second.m_iTriggerValue == QIYU_JINGJIE)
		{
			CPackFightReportItem reportItem;
			reportItem.m_iReportType = dayQiyuOrCangbaotu ? CLogicConfigDefine::FIGHT_REPORT_QIYU : CLogicConfigDefine::FIGHT_REPORT_CANGBAOTU;
			reportItem.m_iReportID = CLogicCommon::GetSec();
			reportItem.m_cFightResult = 1;

			std::map<std::string, std::string> stParamMap;
			stParamMap["card_name"] = CLogicConfig::GetItemName(CLogicConfigDefine::HERO_CARD, intParams[0]);
			stParamMap["num"] = std::to_string(notice.m_stItemVec.empty() ? 0 : notice.m_stItemVec[0].m_iNum);
			reportItem.m_strReportContent = fightReportIter->second.m_stContentObj.Replace(stParamMap);
            CLogicCacheCommon::AddUserFightReport(m_pUserInfo, reportItem);

			break;
		}
	}

	return true;
}

bool CLogicQiyuProcessor::GenRonglianParam(const stQiyuMapData &mapData, int eventType, int eventID, std::vector<int> &params, std::vector<std::string> &strParams)
{
	params.clear();
	strParams.clear();
	
	auto ronglianIter = CLogicConfig().GetQiyuConfig().m_mpRonglian.find(eventID);
	if (ronglianIter != CLogicConfig().GetQiyuConfig().m_mpRonglian.end())
	{
		params.push_back(ronglianIter->second.m_iRonglianValue);
		params.push_back(0);
	}

	if ((params.size() != 2) || (strParams.size() != 0))
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	return true;
}

bool CLogicQiyuProcessor::FinishRonglianEvent(TUserQiyuTable *node, bool dayQiyuOrCangbaotu, CNotifyQiyuReward &notice)
{
	if (node == nullptr)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	//2.熔炼值是否达到最大值
	std::vector<int> intParams = node->GetParamsInt();
	if (intParams.size () != 2)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	if (intParams[1] < intParams[0])
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(QIYU_NODE_NOT_FINISH);
		return false;
	}

	//3.成功，发放奖励
	auto pConf = CLogicConfig::GetQiyuConfig().m_mpRonglian.find(node->GetNodeEventID());
	if (pConf == CLogicConfig::GetQiyuConfig().m_mpRonglian.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(COMMON_CONFIG_NOT_EXIST);
		return false;
	}

	std::string rewardGroupName = RandomRewardGroup(m_pUserInfo->m_stUserInfoTable.GetLevel(), pConf->second.m_strLevelRewardPool);
	auto rewardGroupIter = CLogicConfig::GetQiyuConfig().m_mpRewardGroup.find(rewardGroupName);
	if (rewardGroupIter != CLogicConfig::GetQiyuConfig().m_mpRewardGroup.end())
	{
		CLogicCacheCommon::AddGameItem(m_pUserInfo, rewardGroupIter->second.m_aRewards, m_usCmd);

		notice.m_stItemVec = rewardGroupIter->second.m_aRewards;

		//战报
		for (auto fightReportIter = CLogicConfig::GetFightReportConfig().m_stConfigMap.lower_bound("qiyu"); fightReportIter != CLogicConfig::GetFightReportConfig().m_stConfigMap.upper_bound("qiyu"); ++fightReportIter)
		{
			if (fightReportIter->second.m_iTriggerValue == QIYU_RONGLU)
			{
				CPackFightReportItem reportItem;
				reportItem.m_iReportType = dayQiyuOrCangbaotu ? CLogicConfigDefine::FIGHT_REPORT_QIYU : CLogicConfigDefine::FIGHT_REPORT_CANGBAOTU;
				reportItem.m_iReportID = CLogicCommon::GetSec();
				reportItem.m_cFightResult = 1;

				std::map<std::string, std::string> stParamMap;
				stParamMap["reward"] = CLogicConfig::GenRewardContent(rewardGroupIter->second.m_aRewards);
				reportItem.m_strReportContent = fightReportIter->second.m_stContentObj.Replace(stParamMap);
                CLogicCacheCommon::AddUserFightReport(m_pUserInfo, reportItem);

				break;
			}
		}
	}


	return true;
}

bool CLogicQiyuProcessor::GenMibaokuParam(const stQiyuMapData &mapData, int eventType, int eventID, std::vector<int> &params, std::vector<std::string> &strParams)
{
	params.clear();
	strParams.clear();

	//随机三组奖励
	auto pConf = CLogicConfig::GetQiyuConfig().m_mpMibaoku.find(eventID);
	if (pConf != CLogicConfig::GetQiyuConfig().m_mpMibaoku.end())
	{
		std::string rewardGroupName = RandomRewardGroup(m_pUserInfo->m_stUserInfoTable.GetLevel(), pConf->second.m_strLevelRewardPool1);
		if (!rewardGroupName.empty())
		{
			strParams.push_back(rewardGroupName);
			params.push_back(0);
		}

		rewardGroupName = RandomRewardGroup(m_pUserInfo->m_stUserInfoTable.GetLevel(), pConf->second.m_strLevelRewardPool2);
		if (!rewardGroupName.empty())
		{
			strParams.push_back(rewardGroupName);
			params.push_back(0);
		}

		rewardGroupName = RandomRewardGroup(m_pUserInfo->m_stUserInfoTable.GetLevel(), pConf->second.m_strLevelRewardPool3);
		if (!rewardGroupName.empty())
		{
			strParams.push_back(rewardGroupName);
			params.push_back(0);
		}
	}

	if ((params.size() != 3) || (strParams.size() != 3))
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	return true;
}

bool CLogicQiyuProcessor::FinishMibaokuEvent(TUserQiyuTable *node, bool dayQiyuOrCangbaotu, CNotifyQiyuReward &notice)
{
	if (node == nullptr)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	auto confIter = CLogicConfig::GetQiyuConfig().m_mpMibaoku.find(node->GetNodeEventID());
	if (confIter == CLogicConfig::GetQiyuConfig().m_mpMibaoku.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(COMMON_CONFIG_NOT_EXIST);
		return false;
	}

	std::vector<std::string> strParams = node->GetParamsStr();
	std::vector<int> intParams = node->GetParamsInt();

	if (intParams.size() != 3)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	//1.判断是否所有的宝箱都已打开
	std::vector<std::pair<int, int> > notOpenBoxes;
	int totalCoef = 0;
	if (intParams[0] == 0)
	{
		notOpenBoxes.push_back(std::make_pair(0, confIter->second.m_iCoef1));
		totalCoef += confIter->second.m_iCoef1;
	}
	if (intParams[1] == 0)
	{
		notOpenBoxes.push_back(std::make_pair(1, confIter->second.m_iCoef2));
		totalCoef += confIter->second.m_iCoef2;
	}
	if (intParams[2] == 0)
	{
		notOpenBoxes.push_back(std::make_pair(2, confIter->second.m_iCoef3));
		totalCoef += confIter->second.m_iCoef3;
	}

	if (notOpenBoxes.empty())
	{
		node->SetNodeState(2);

		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	//判断元宝是否足够
	if (notOpenBoxes.size() == 2)
	{
		if (false == CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_YuanBaoID, confIter->second.m_iBox2Cash, m_usCmd))
		{
			using namespace SERVER_ERRCODE;
			SET_ERR_INFO(NOT_ENOUGH_RMB);
			return false;
		}
	}
	else if (notOpenBoxes.size() == 1)
	{
		if (false == CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, CLogicConfigDefine::CURRENCY, CLogicConfigDefine::ECIT_YuanBaoID, confIter->second.m_iBox3Cash, m_usCmd))
		{
			using namespace SERVER_ERRCODE;
			SET_ERR_INFO(NOT_ENOUGH_RMB);
			return false;
		}
	}


	//2.在未打开的宝箱随机一个宝箱，置宝箱状态为已打开，发放奖励
	int baoxiangIndex = 0;
	int randomNum = CLogicCommon::GetRandNum(0, totalCoef);
	totalCoef = 0;
	for (auto &boxIter : notOpenBoxes)
	{
		totalCoef += boxIter.second;
		if (randomNum < totalCoef)
		{
			baoxiangIndex = boxIter.first;
			break;
		}
	}

	std::string rewardGroupName = strParams[baoxiangIndex];
	auto rewardGroupIter = CLogicConfig::GetQiyuConfig().m_mpRewardGroup.find(rewardGroupName);
	if (rewardGroupIter != CLogicConfig::GetQiyuConfig().m_mpRewardGroup.end())
	{
		CLogicCacheCommon::AddGameItem(m_pUserInfo, rewardGroupIter->second.m_aRewards, m_usCmd);

		notice.m_stItemVec = rewardGroupIter->second.m_aRewards;

		//战报
		for (auto fightReportIter = CLogicConfig::GetFightReportConfig().m_stConfigMap.lower_bound("qiyu"); fightReportIter != CLogicConfig::GetFightReportConfig().m_stConfigMap.upper_bound("qiyu"); ++fightReportIter)
		{
			if (fightReportIter->second.m_iTriggerValue == QIYU_MIBAOKU)
			{
				CPackFightReportItem reportItem;
				reportItem.m_iReportType = dayQiyuOrCangbaotu ? CLogicConfigDefine::FIGHT_REPORT_QIYU : CLogicConfigDefine::FIGHT_REPORT_CANGBAOTU;
				reportItem.m_iReportID = CLogicCommon::GetSec();
				reportItem.m_cFightResult = 1;

				std::map<std::string, std::string> stParamMap;
				stParamMap["reward"] = CLogicConfig::GenRewardContent(rewardGroupIter->second.m_aRewards);
				reportItem.m_strReportContent = fightReportIter->second.m_stContentObj.Replace(stParamMap);
                CLogicCacheCommon::AddUserFightReport(m_pUserInfo, reportItem);

				break;
			}
		}
	}

	intParams[baoxiangIndex] = 1;
	node->SetParamsInt(intParams);

	//3.如果所有宝箱都已打开，置结点状态为已完成
	if (notOpenBoxes.size() == 1)
	{
		node->SetNodeState(2);
	}

	return true;
}

bool CLogicQiyuProcessor::GenNormalRewardParam(const stQiyuMapData &mapData, int eventType, int eventID, std::vector<int> &params, std::vector<std::string> &strParams)
{
	return true;
}

bool CLogicQiyuProcessor::FinishNormalRewardEvent(TUserQiyuTable *node, bool dayQiyuOrCangbaotu, CNotifyQiyuReward &notice)
{
	if (node == nullptr)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	//发放奖励
	auto pConf = CLogicConfig::GetQiyuConfig().m_mpNormalReward.find(node->GetNodeEventID());
	if (pConf != CLogicConfig::GetQiyuConfig().m_mpNormalReward.end())
	{
		std::string levelRewardPoolName = pConf->second;

		std::string rewardGroupName = RandomRewardGroup(m_pUserInfo->m_stUserInfoTable.GetLevel(), levelRewardPoolName);
		auto rewardGroupIter = CLogicConfig::GetQiyuConfig().m_mpRewardGroup.find(rewardGroupName);
		if (rewardGroupIter != CLogicConfig::GetQiyuConfig().m_mpRewardGroup.end())
		{
			CLogicCacheCommon::AddGameItem(m_pUserInfo, rewardGroupIter->second.m_aRewards, m_usCmd);

			notice.m_stItemVec = rewardGroupIter->second.m_aRewards;

			//战报
			for (auto fightReportIter = CLogicConfig::GetFightReportConfig().m_stConfigMap.lower_bound("qiyu"); fightReportIter != CLogicConfig::GetFightReportConfig().m_stConfigMap.upper_bound("qiyu"); ++fightReportIter)
			{
				if (fightReportIter->second.m_iTriggerValue == QIYU_NORMALREWARD)
				{
					CPackFightReportItem reportItem;
					reportItem.m_iReportType = dayQiyuOrCangbaotu ? CLogicConfigDefine::FIGHT_REPORT_QIYU : CLogicConfigDefine::FIGHT_REPORT_CANGBAOTU;
					reportItem.m_iReportID = CLogicCommon::GetSec();
					reportItem.m_cFightResult = 1;

					std::map<std::string, std::string> stParamMap;
					stParamMap["reward"] = CLogicConfig::GenRewardContent(rewardGroupIter->second.m_aRewards);
					reportItem.m_strReportContent = fightReportIter->second.m_stContentObj.Replace(stParamMap);
                    CLogicCacheCommon::AddUserFightReport(m_pUserInfo, reportItem);

					break;
				}
			}
		}
	}

	return true;
}

bool CLogicQiyuProcessor::GenQiangzheParam(const stQiyuMapData &mapData, int eventType, int eventID, std::vector<int> &params, std::vector<std::string> &strParams)
{
	auto qiangzhetiaozhanIter = CLogicConfig::GetQiyuConfig().m_mpQiangzhetiaozhan.find(eventID);
	if (qiangzhetiaozhanIter == CLogicConfig::GetQiyuConfig().m_mpQiangzhetiaozhan.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	//随机关卡
	auto guanqiaItem = RandomGuanqia(m_pUserInfo->m_stUserInfoTable.GetLevel(), qiangzhetiaozhanIter->second.m_iLevelGuanqiaPool);
	params.push_back(guanqiaItem.first);
	if (guanqiaItem.second.m_bPVEOrPVP)
		params.push_back(1);
	else
		params.push_back(2);

	//文本
	if (guanqiaItem.second.m_iSex == 1)
	{
		int randomNum = CLogicCommon::GetRandNum(0, CLogicConfig::GetQiyuConfig().m_aMaleQiangzheContent.size());
		params.push_back(CLogicConfig::GetQiyuConfig().m_aMaleQiangzheContent[randomNum]);
	}
	else
	{
		int randomNum = CLogicCommon::GetRandNum(0, CLogicConfig::GetQiyuConfig().m_aFemaleQiangzheContent.size());
		params.push_back(CLogicConfig::GetQiyuConfig().m_aFemaleQiangzheContent[randomNum]);
	}

	//随机奖励	
	auto pvpBotIter = CLogicConfig::GetQiyuBotConfig().m_mpPVPBot.find(guanqiaItem.first);
	if (pvpBotIter != CLogicConfig::GetQiyuBotConfig().m_mpPVPBot.end())
	{
		strParams.push_back(pvpBotIter->second.m_strRewardGroup);
	}

	if ((params.size() != 3) || (strParams.size() != 1))
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	return true;
}

bool CLogicQiyuProcessor::FinishQiangzheEvent(TUserQiyuTable *node, bool dayQiyuOrCangbaotu, CNotifyQiyuReward &notice)
{
	if (node == nullptr)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	std::vector<int> intParams = node->GetParamsInt();
	std::vector<std::string> params = node->GetParamsStr();
	if ((!params.empty()) && (!intParams.empty()))
	{
		auto rewardGroupIter = CLogicConfig::GetQiyuConfig().m_mpRewardGroup.find(params[0]);
		if (rewardGroupIter != CLogicConfig::GetQiyuConfig().m_mpRewardGroup.end())
		{
			CLogicCacheCommon::AddGameItem(m_pUserInfo, rewardGroupIter->second.m_aRewards, m_usCmd);

			notice.m_stItemVec = rewardGroupIter->second.m_aRewards;

			//战报
			for (auto fightReportIter = CLogicConfig::GetFightReportConfig().m_stConfigMap.lower_bound("qiyu"); fightReportIter != CLogicConfig::GetFightReportConfig().m_stConfigMap.upper_bound("qiyu"); ++fightReportIter)
			{
				if (fightReportIter->second.m_iTriggerValue == QIYU_QIANGZHETIAOZHAN)
				{
					CPackFightReportItem reportItem;
					reportItem.m_iReportType = dayQiyuOrCangbaotu ? CLogicConfigDefine::FIGHT_REPORT_QIYU : CLogicConfigDefine::FIGHT_REPORT_CANGBAOTU;
					reportItem.m_iReportID = CLogicCommon::GetSec();
					reportItem.m_cFightResult = 1;

					std::map<std::string, std::string> stParamMap;
					stParamMap["card_name"] = "";
					auto pvpBotIter = CLogicConfig::GetQiyuBotConfig().m_mpPVPBot.find(intParams[0]);
					if (pvpBotIter != CLogicConfig::GetQiyuBotConfig().m_mpPVPBot.end())
					{
						stParamMap["card_name"] = pvpBotIter->second.m_strCardName;
					}
					stParamMap["reward"] = CLogicConfig::GenRewardContent(rewardGroupIter->second.m_aRewards);
					reportItem.m_strReportContent = fightReportIter->second.m_stContentObj.Replace(stParamMap);
                    CLogicCacheCommon::AddUserFightReport(m_pUserInfo, reportItem);

					break;
				}
			}
		}
	}

	return true;
}

bool CLogicQiyuProcessor::GenShouweiParam(const stQiyuMapData &mapData, int eventType, int eventID, std::vector<int> &params, std::vector<std::string> &strParams)
{
	auto baozangshouweiIter = CLogicConfig::GetQiyuConfig().m_mpBaozangshouwei.find(eventID);
	if (baozangshouweiIter == CLogicConfig::GetQiyuConfig().m_mpBaozangshouwei.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	//随机关卡
	auto guanqiaItem = RandomGuanqia(m_pUserInfo->m_stUserInfoTable.GetLevel(), baozangshouweiIter->second.m_iLevelGuanqiaPool);
	params.push_back(guanqiaItem.first);
	if (guanqiaItem.second.m_bPVEOrPVP)
		params.push_back(1);
	else
		params.push_back(2);

	//随机奖励	
	std::string rewardGroupName = RandomRewardGroup(m_pUserInfo->m_stUserInfoTable.GetLevel(), baozangshouweiIter->second.m_strLevelRewardPool);
	if (!rewardGroupName.empty())
		strParams.push_back(rewardGroupName);

	if ((params.size() != 2) || (strParams.size() != 1))
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	return true;
}

bool CLogicQiyuProcessor::FinishShouweiEvent(TUserQiyuTable *node, bool dayQiyuOrCangbaotu, CNotifyQiyuReward &notice)
{
	if (node == nullptr)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	std::vector<std::string> params = node->GetParamsStr();
	if (!params.empty())
	{
		auto rewardGroupIter = CLogicConfig::GetQiyuConfig().m_mpRewardGroup.find(params[0]);
		if (rewardGroupIter != CLogicConfig::GetQiyuConfig().m_mpRewardGroup.end())
		{
			CLogicCacheCommon::AddGameItem(m_pUserInfo, rewardGroupIter->second.m_aRewards, m_usCmd);

			notice.m_stItemVec = rewardGroupIter->second.m_aRewards;

			//战报
			for (auto fightReportIter = CLogicConfig::GetFightReportConfig().m_stConfigMap.lower_bound("qiyu"); fightReportIter != CLogicConfig::GetFightReportConfig().m_stConfigMap.upper_bound("qiyu"); ++fightReportIter)
			{
				if (fightReportIter->second.m_iTriggerValue == QIYU_SHOUWEI)
				{
					CPackFightReportItem reportItem;
					reportItem.m_iReportType = dayQiyuOrCangbaotu ? CLogicConfigDefine::FIGHT_REPORT_QIYU : CLogicConfigDefine::FIGHT_REPORT_CANGBAOTU;
					reportItem.m_iReportID = CLogicCommon::GetSec();
					reportItem.m_cFightResult = 1;

					std::map<std::string, std::string> stParamMap;
					stParamMap["reward"] = CLogicConfig::GenRewardContent(rewardGroupIter->second.m_aRewards);
					reportItem.m_strReportContent = fightReportIter->second.m_stContentObj.Replace(stParamMap);
                    CLogicCacheCommon::AddUserFightReport(m_pUserInfo, reportItem);

					break;
				}
			}
		}
	}

	return true;
}

bool CLogicQiyuProcessor::GenMaozeiParam(const stQiyuMapData &mapData, int eventType, int eventID, std::vector<int> &params, std::vector<std::string> &strParams)
{
	auto maozeiIter = CLogicConfig::GetQiyuConfig().m_mpMaozei.find(eventID);
	if (maozeiIter == CLogicConfig::GetQiyuConfig().m_mpMaozei.end())
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	//随机关卡
	auto guanqiaItem = RandomGuanqia(m_pUserInfo->m_stUserInfoTable.GetLevel(), maozeiIter->second.m_iLevelGuanqiaPool);
	params.push_back(guanqiaItem.first);
	if (guanqiaItem.second.m_bPVEOrPVP)
		params.push_back(1);
	else
		params.push_back(2);

	//随机消耗组
	auto consumeID = RandomConsumeGroup(m_pUserInfo->m_stUserInfoTable.GetLevel(), maozeiIter->second.m_iLevelConsumePool);
	params.push_back(consumeID);

	//文本
	int randomNum = CLogicCommon::GetRandNum(0, CLogicConfig::GetQiyuConfig().m_aMaozeiContent.size());
	params.push_back(CLogicConfig::GetQiyuConfig().m_aMaozeiContent[randomNum]);

	if ((params.size() != 4) || (strParams.size() != 0))
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	return true;
	
}

bool CLogicQiyuProcessor::FinishMaozeiEvent(TUserQiyuTable *node, bool dayQiyuOrCangbaotu, const std::vector<int> &params, CNotifyQiyuReward &notice)
{
	if ((node == nullptr) || (params.empty()))
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	auto maozeiIter = CLogicConfig::GetQiyuConfig().m_mpMaozei.find(node->GetNodeEventID());
	if (maozeiIter == CLogicConfig::GetQiyuConfig().m_mpMaozei.end())
		return false;

	//战斗完成
	if (params[0] == 1)
	{
		//奖励
		std::string levelRewardPoolName = maozeiIter->second.m_strLevelRewardPool1;

		std::string rewardGroupName = RandomRewardGroup(m_pUserInfo->m_stUserInfoTable.GetLevel(), levelRewardPoolName);
		auto rewardGroupIter = CLogicConfig::GetQiyuConfig().m_mpRewardGroup.find(rewardGroupName);
		if (rewardGroupIter != CLogicConfig::GetQiyuConfig().m_mpRewardGroup.end())
		{
			CLogicCacheCommon::AddGameItem(m_pUserInfo, rewardGroupIter->second.m_aRewards, m_usCmd);

			notice.m_stItemVec = rewardGroupIter->second.m_aRewards;

			//战报
			for (auto fightReportIter = CLogicConfig::GetFightReportConfig().m_stConfigMap.lower_bound("qiyu"); fightReportIter != CLogicConfig::GetFightReportConfig().m_stConfigMap.upper_bound("qiyu"); ++fightReportIter)
			{
				if (fightReportIter->second.m_iTriggerValue == 1301)
				{
					CPackFightReportItem reportItem;
					reportItem.m_iReportType = dayQiyuOrCangbaotu ? CLogicConfigDefine::FIGHT_REPORT_QIYU : CLogicConfigDefine::FIGHT_REPORT_CANGBAOTU;
					reportItem.m_iReportID = CLogicCommon::GetSec();
					reportItem.m_cFightResult = 1;

					std::map<std::string, std::string> stParamMap;
					stParamMap["reward"] = CLogicConfig::GenRewardContent(rewardGroupIter->second.m_aRewards);
					reportItem.m_strReportContent = fightReportIter->second.m_stContentObj.Replace(stParamMap);
                    CLogicCacheCommon::AddUserFightReport(m_pUserInfo, reportItem);

					break;
				}
			}
		}
	}
	//花钱了事
	else
	{
		//消耗材料
		std::vector<int> nodeParams = node->GetParamsInt();
		if (nodeParams.size() < 4)
			return false;

		auto consumeIter = CLogicConfig::GetQiyuConfig().m_mpConsumeGroup.find(nodeParams[2]);
		if (consumeIter != CLogicConfig::GetQiyuConfig().m_mpConsumeGroup.end())
		{
			if (false == CLogicCacheCommon::ConsumeGameItem(m_pUserInfo, consumeIter->second.m_aConsumes, m_usCmd))
				return false;
		}

		//奖励
		std::string levelRewardPoolName = maozeiIter->second.m_strLevelRewardPool2;

		std::string rewardGroupName = RandomRewardGroup(m_pUserInfo->m_stUserInfoTable.GetLevel(), levelRewardPoolName);
		auto rewardGroupIter = CLogicConfig::GetQiyuConfig().m_mpRewardGroup.find(rewardGroupName);
		if (rewardGroupIter != CLogicConfig::GetQiyuConfig().m_mpRewardGroup.end())
		{
			CLogicCacheCommon::AddGameItem(m_pUserInfo, rewardGroupIter->second.m_aRewards, m_usCmd);

			notice.m_stItemVec = rewardGroupIter->second.m_aRewards;

			//战报
			for (auto fightReportIter = CLogicConfig::GetFightReportConfig().m_stConfigMap.lower_bound("qiyu"); fightReportIter != CLogicConfig::GetFightReportConfig().m_stConfigMap.upper_bound("qiyu"); ++fightReportIter)
			{
				if (fightReportIter->second.m_iTriggerValue == 1300)
				{
					CPackFightReportItem reportItem;
					reportItem.m_iReportType = dayQiyuOrCangbaotu ? CLogicConfigDefine::FIGHT_REPORT_QIYU : CLogicConfigDefine::FIGHT_REPORT_CANGBAOTU;
					reportItem.m_iReportID = CLogicCommon::GetSec();
					reportItem.m_cFightResult = 0;

					std::map<std::string, std::string> stParamMap;
					stParamMap["reward"] = CLogicConfig::GenRewardContent(rewardGroupIter->second.m_aRewards);
					reportItem.m_strReportContent = fightReportIter->second.m_stContentObj.Replace(stParamMap);
                    CLogicCacheCommon::AddUserFightReport(m_pUserInfo, reportItem);

					break;
				}
			}
		}
	}

	return true;
}

bool CLogicQiyuProcessor::GenMubeiParam(const stQiyuMapData &mapData, int eventType, int eventID, std::vector<int> &params, std::vector<std::string> &strParams)
{
	//文本
	if (!CLogicConfig::GetQiyuConfig().m_aMubeiContent.empty())
	{
		int randomNum = CLogicCommon::GetRandNum(0, CLogicConfig::GetQiyuConfig().m_aMubeiContent.size());
		params.push_back(CLogicConfig::GetQiyuConfig().m_aMubeiContent[randomNum]);
	}

	if ((params.size() != 1) || (strParams.size() != 0))
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	return true;
}

bool CLogicQiyuProcessor::FinishMubeiEvent(TUserQiyuTable *node, bool dayQiyuOrCangbaotu, int mapID, CNotifyQiyuReward &notice)
{
	if (node == nullptr)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(INTERN_ERROR);
		return false;
	}

	auto mapIter = CLogicConfig::GetQiyuConfig().m_mpQiyuMap.find(mapID);
	if (mapIter == CLogicConfig::GetQiyuConfig().m_mpQiyuMap.end())
		return false;

	int cangbaotuID = mapIter->second.m_iCangbaotuID;
	//1.找到玩家身上该碎片信息
	int suipianBitmap = 0;
	auto userCangbaotusuipian = m_pUserInfo->m_stUserInfoTable.GetCangbaotuSuipian();
	auto suipianIter = userCangbaotusuipian.m_mpCangbaotuSuipian.find(cangbaotuID);
	if (suipianIter != userCangbaotusuipian.m_mpCangbaotuSuipian.end())
		suipianBitmap = suipianIter->second;

	//2.如果碎片已满，则返回失败
	if (suipianBitmap == 0x000001ff)
	{
		using namespace SERVER_ERRCODE;
		SET_ERR_INFO(QIYU_CANGBAOTUSUIPIAN_FULL);
		return false;
	}

	//3.从缺失的碎片中，随机一个返回；
	std::vector<int> lostSuipian;
	for (int i = 1; i <= 9; ++i)
	{
		if ((suipianBitmap & (0x00000001 << (i - 1))) == 0)
			lostSuipian.push_back(i);
	}
	int randomIndex = CLogicCommon::GetRandNum(0, lostSuipian.size());
	notice.m_iSuipianIndex = lostSuipian[randomIndex];

	//4.设置玩家碎片信息,并通知
	suipianBitmap = suipianBitmap | (0x00000001 << (lostSuipian[randomIndex] - 1));
	auto newUserCangbaotuSuipian = userCangbaotusuipian;
	newUserCangbaotuSuipian.m_mpCangbaotuSuipian[cangbaotuID] = suipianBitmap;
	m_pUserInfo->m_stUserInfoTable.SetCangbaotuSuipian(newUserCangbaotuSuipian);

	CNotifyCangbaotuSuipian suipianNotice;
	suipianNotice.m_iSuipianIndex = notice.m_iSuipianIndex;
	for (auto &newSuipianIter : newUserCangbaotuSuipian.m_mpCangbaotuSuipian)
	{
		suipianNotice.m_mpCangbaotuSuipian[newSuipianIter.first] = int2bin<uint32_t>(newSuipianIter.second);
	}
	CLogicUserNotifySystem::NotifyCangbaotuSuipianData(m_pUserInfo, suipianNotice);

	//战报
	for (auto fightReportIter = CLogicConfig::GetFightReportConfig().m_stConfigMap.lower_bound("qiyu"); fightReportIter != CLogicConfig::GetFightReportConfig().m_stConfigMap.upper_bound("qiyu"); ++fightReportIter)
	{
		if (fightReportIter->second.m_iTriggerValue == QIYU_MUBEI)
		{
			CPackFightReportItem reportItem;
			reportItem.m_iReportType = dayQiyuOrCangbaotu ? CLogicConfigDefine::FIGHT_REPORT_QIYU : CLogicConfigDefine::FIGHT_REPORT_CANGBAOTU;
			reportItem.m_iReportID = CLogicCommon::GetSec();
			reportItem.m_cFightResult = 0;

			std::map<std::string, std::string> stParamMap;
			reportItem.m_strReportContent = fightReportIter->second.m_stContentObj.Replace(stParamMap);
            CLogicCacheCommon::AddUserFightReport(m_pUserInfo, reportItem);

			break;
		}
	}

	return true;
}


