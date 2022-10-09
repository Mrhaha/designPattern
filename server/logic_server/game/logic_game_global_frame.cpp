#include "common_datetime.h"
#include "server_cmd_logic_define.h"
#include "server_cmd_cross_define.h"
#include "logic_link_common.h"
#include "logic_game_global_frame.h"
#include "logic_config.h"
#include "logic_game_user.h"
#include "logic_game_global.h"
#include "logic_game_user_notify_system.h"
#include "logic_game_global.h"
#include "logic_game_talk_common.h"
#include "logic_game_cache_common.h"
#include "logic_game_rank_list.h"
#include "logic_game_bill.h"
#include "logic_game_publisher.h"

CLogicGlobalFrame::CLogicGlobalFrame() :
	m_stWorldTalkList(),
	m_iRedBagMaxIndex(0)
{

}

CLogicGlobalFrame::~CLogicGlobalFrame()
{
}


bool CLogicGlobalFrame::Initialize()
{
	auto bRet = true;

	bRet &= RefreshHappyTurnCardVec();

	return bRet;
}

void CLogicGlobalFrame::Refresh()
{
	static int32_t s_iLastCheckTime = CLogicCommon::GetSec();
	if (LOGIC_CHECK_DAY_REFRESH(s_iLastCheckTime))
	{
		OnCrossOneDay();

		s_iLastCheckTime = CLogicCommon::GetSec();
	}

	RefreshOnlineAction();
}

void CLogicGlobalFrame::OnCrossOneDay()
{
	RefreshHappyTurnCardVec();
}

void CLogicGlobalFrame::RefreshOnlineAction()
{
	static int32_t s_iLastCheckTime = CLogicCommon::GetSec();
	const int32_t iNow = CLogicCommon::GetSec();
	constexpr int32_t interval = 20;
	if (iNow - s_iLastCheckTime > interval)
	{
		s_iLastCheckTime = iNow; 

		logic_game_user_action_channel_data_t stData;
		stData.m_iTimes = interval;
		stData.m_iAction = CLogicGameChannelEnum::ACTION_ONLINE_SECOND;

		for (auto iterUser = CLogicUser::GetInstance().Begin(); iterUser != CLogicUser::GetInstance().End(); ++iterUser)
		{
			auto pstUserInfo = CLogicCacheCommon::GetInCacheUserData(iterUser->second.m_iGroupID, iterUser->first);
			if (pstUserInfo)
			{				
				LOGIC_PUBLISHER_INS->PublishGameUserActionInfo(pstUserInfo, stData);
			}
		}

		// 防沉迷在线时长更新
		CLogicGlobal::AntiAddUpdateOnlineTime(interval);
	}
}

/************************************************************************************************************/
/******************************************世  界  聊  天*****************************************************/
/************************************************************************************************************/
void CLogicGlobalFrame::AddWorldTalkCache(const CPackTalkInfo& stTalk)
{
    if(CLogicConfig::GetSystemConfig().isServerMaintain()) return;

    m_stWorldTalkList.emplace_back(stTalk);
    if(m_stWorldTalkList.size() >= CLogicConfigDefine::ESD_WorldTalkCacheCount)
    {
        m_stWorldTalkList.pop_front();
    }
}

void CLogicGlobalFrame::RemoveNickWorldTalk(const std::string& strNick)
{
	for (auto iter = m_stWorldTalkList.begin(); iter != m_stWorldTalkList.end(); )
	{
		if (iter->m_stUserBrief.m_strNick == strNick)
		{
			iter = m_stWorldTalkList.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void CLogicGlobalFrame::AddGuildTalkCache(const CPackTalkInfo& stTalk, int iGuildID)
{
    if(CLogicConfig::GetSystemConfig().isServerMaintain()) return;

    m_stGuildTalkList[iGuildID].emplace_back(stTalk);
    if(m_stGuildTalkList[iGuildID].size() >= CLogicConfigDefine::ESD_WorldTalkCacheCount)
    {
        m_stGuildTalkList[iGuildID].pop_front();
    }
}

const talk_cache_list* CLogicGlobalFrame::GetGuildTalkList(int iGuildID) const
{
    auto stList = m_stGuildTalkList.find(iGuildID);
    return stList == m_stGuildTalkList.end() ? nullptr : &(stList->second);
}

bool CLogicGlobalFrame::IsRepeatTalk(int32_t iUin, int32_t iGroupID, const std::string& strMsg)
{
	if (strMsg.size() <= 30)
	{
		return false;
	}

	std::string strFiter;
	for (const auto& c : strMsg)
	{
		if (c <= 0)
		{
			strFiter.push_back(c);
		}
	}

	if (strFiter.size() <= 30)
	{
		return false;
	}

	auto iter = m_stTalkCount.find(strFiter);
	if (iter == m_stTalkCount.end())
	{
		const auto stRet = m_stTalkCount.insert(std::make_pair(strFiter, 0));
		if (!stRet.second)
		{
			return true;
		}
		iter = stRet.first;
	}

	++(iter->second);

	if (iter->second % 5 == 0)
	{
        LOGIC_SERVICE_API_INS->RequestServiceAlert("repeat_talk", {
                { "uin", std::to_string(iUin) },
                { "group_id", std::to_string(iGroupID) },
                { "talk", strMsg },
        });
	}

	return iter->second > 1;
}

void CLogicGlobalFrame::AddBroadcastPushProto(const CPackBroadcastTalkMsg& rstProto)
{
	if (!CLogicConfig::GetSystemConfig().isServerMaintain())
	{
		m_stBroadcastList.emplace_back(rstProto);

		//先删除低优先级公告;再次按照时间删除
		if (m_stBroadcastList.size() >= CLogicConfigDefine::ESD_WorldTalkCacheCount)
		{
			/*auto eraseIter = m_stBroadcastList.begin();
			for (auto iter = m_stBroadcastList.begin(); iter != m_stBroadcastList.end(); ++iter)
			{
			if (iter->m_iPriority < eraseIter->m_iPriority)
			eraseIter = iter;
			}

			m_stBroadcastList.erase(eraseIter);*/

			m_stBroadcastList.pop_front();
		}
	}	
}
/************************************************************************************************************/
/******************************************开  心  翻  牌*****************************************************/
/************************************************************************************************************/
bool CLogicGlobalFrame::RefreshHappyTurnCardVec()
{
	m_stTurnCardVec.clear();

	const auto iterBag = CLogicConfig::GetCardbagConfig().m_stCardbagConfig.find(CLogicConfig::GetAwardConfig().m_stHappyTurnCard.m_strRandBag);
	if (iterBag == CLogicConfig::GetCardbagConfig().m_stCardbagConfig.end())
	{
		return false;
	}

	auto stRandVec = iterBag->second.m_stCardbagElem;

	int32_t iLeftWeight = iterBag->second.m_iTotalWeight;
	std::mt19937 stRand19937(get_date_integer(CLogicCommon::GetSec()));
	auto RandFunc = [&](int32_t iNum) { return stRand19937() % iNum; };

	for (auto i = 0 ; i < CLogicConfigDefine::HAPPY_TURN_CARD_COUNT / 2; ++i)
	{
		if (iLeftWeight <= 0)
		{
			return false;
		}
		int iRandWeight = RandFunc(iLeftWeight);

		for (auto iter = stRandVec.begin(); iter!=  stRandVec.end(); ++iter)
		{
			if (iRandWeight < iter->m_iWeight)
			{
				iLeftWeight -= iter->m_iWeight;
				m_stTurnCardVec.emplace_back(iter->m_iItemType, iter->m_iCardID, iter->m_iNum);
				m_stTurnCardVec.emplace_back(iter->m_iItemType, iter->m_iCardID, iter->m_iNum);
				stRandVec.erase(iter);
				break;
			}
			iRandWeight -= iter->m_iWeight;
		}
	}

	std::random_shuffle(m_stTurnCardVec.begin(), m_stTurnCardVec.end(), RandFunc);
	return true;
}

/************************************************************************************************************/
/******************************************抢    红    包*****************************************************/
/************************************************************************************************************/
void CLogicGlobalFrame::SendRedBag(user_data_table_ptr_type pUserInfo, int32_t iRedBagItemID)
{
	const auto* pstConfig = CLogicConfig::GetRedBagConfig().GetRedBagConfig(iRedBagItemID);
	if (nullptr == pstConfig)
	{
		return ;
	}

	red_bag_info_t info;
	info.m_iUid = pUserInfo->m_iUin;
	info.m_iGroupID = pUserInfo->m_iGroupID;
	info.m_iRedBagItemID = iRedBagItemID;
    info.m_iExpiredTime = CLogicCommon::GetSec() + pstConfig->m_iTime;
	info.m_iTalkChannel = pstConfig->m_iSendChannel;
    info.m_iGuildID = 0;
    if (pstConfig->m_iSendChannel == CLogicConfigDefine::talk_channel_guild)
    {
        info.m_iGuildID = pUserInfo->m_stUserInfoTable.GetGuildID();
    }

    LOGIC_LOG_TRACE_DEBUG << "SEND_RED_BAG|RedBagItemID:" << iRedBagItemID << std::endl;

    // 旧红包
    if(pstConfig->m_strNewRedBag.empty())
    {
        info.m_iTotalYuanbao = pstConfig->m_iOldTotalYuanbao;
        info.m_iTotalCount = pstConfig->m_iOldCount;
        int32_t iLeftYuanbao = pstConfig->m_iOldTotalYuanbao;
        for (int32_t i = 1 ; i <= pstConfig->m_iOldCount; ++i)
        {
            int32_t iAvgNum = iLeftYuanbao / (pstConfig->m_iOldCount - i + 1);
            int32_t iMinNum = std::max(pstConfig->m_iOldMinYuanbao, iLeftYuanbao - (pstConfig->m_iOldCount - i) * pstConfig->m_iOldMaxYuanbao);
            int32_t iMaxNum = std::min(pstConfig->m_iOldMaxYuanbao, iLeftYuanbao - (pstConfig->m_iOldCount - i) * pstConfig->m_iOldMinYuanbao);
            iMinNum = std::min(pstConfig->m_iOldMaxYuanbao, iMinNum);
            iMaxNum = std::max(pstConfig->m_iOldMinYuanbao, iMaxNum);
            if (iMinNum < iAvgNum && iMaxNum > iAvgNum && 2 * iAvgNum - iMinNum < iMaxNum && CLogicCommon::GetRandNum() % 4)
            {
                iMaxNum = std::min(iMaxNum, 2 * iAvgNum - iMinNum);
            }

            const int32_t iRandYuanbao = CLogicCommon::GetRandNum(iMinNum, iMaxNum);
            LOGIC_LOG_TRACE_DEBUG << "YUANBAO_RED_BAG|INDEX:" << i << "|LEFT:" << iLeftYuanbao << "|MIN:" << iMinNum << "|MAX:" << iMaxNum
                                  << "|AVG:" << iAvgNum  << "|RAND:" << iRandYuanbao << std::endl;
            info.m_iReceiveYuanbao.push_back(iRandYuanbao);
            iLeftYuanbao -= iRandYuanbao;
        }

        std::random_shuffle(info.m_iReceiveYuanbao.begin(), info.m_iReceiveYuanbao.end());
    }
    else
    {
        // 新红包
        auto iterBagDetail = CLogicConfig::GetRedBagConfig().m_stNewRedBagMap.find(pstConfig->m_strNewRedBag);
        if(iterBagDetail == CLogicConfig::GetRedBagConfig().m_stNewRedBagMap.end())
            return;

        // 公会红包特殊处理
        int32_t iBestCount = 0;
        if (pstConfig->m_iSendChannel == CLogicConfigDefine::talk_channel_guild)
        {
            /*
            auto pGuild = CLogicGuild::GetGuild(pUserInfo);
            if (!pGuild) return;

            info.m_iTotalCount = pGuild->GetMemberCount();
            if(info.m_iTotalCount <= 0)
            {
                LOGIC_LOG_TRACE_ERROR << "Guild RedBag guild member count is 0! |GuildID:" << pUserInfo->m_iCacheGuildID
                                      << "|UIN:" << pUserInfo->m_iUin << "|ORDER_ID:" << pUserInfo->m_iGroupID << std::endl;
                return;
            }
*/
            for(auto& iter : iterBagDetail->second)
            {
                if(iter.second.m_bIsBest)
                {
                    iBestCount = iter.second.m_iCount;
                    break;
                }
            }
        }

        info.m_iTotalWeight = 0;
        for(auto iter = iterBagDetail->second.begin(); iter != iterBagDetail->second.end(); ++iter)
        {
            auto& elem = iter->second;
            if(elem.m_iWeight > 0 && elem.m_iCount > 0)
            {
                if (pstConfig->m_iSendChannel == CLogicConfigDefine::talk_channel_guild)
                {
                    int32_t iCount = (elem.m_bIsBest ? elem.m_iCount : info.m_iTotalCount - iBestCount);
                    if(iCount <= 0) continue;

                    info.m_iTotalWeight += elem.m_iWeight;
                    info.m_stCurNewBagReward[iter->first] = std::make_pair(elem.m_iWeight, iCount);
                }
                else
                {
                    info.m_iTotalCount += elem.m_iCount;
                    info.m_iTotalWeight += elem.m_iWeight;
                    info.m_stCurNewBagReward[iter->first] = std::make_pair(elem.m_iWeight, elem.m_iCount);
                }
            }
        }
    }

	LOGIC_LOG_TRACE_DEBUG << "END_RED_BAG=================================================================" << std::endl;

	const int32_t iRedBagID = ++m_iRedBagMaxIndex;
	m_stRedBag[iRedBagID] = std::move(info);

	CLogicTalk::SendWordBroadcastTalk(pUserInfo, pstConfig->m_iSendChannel, pstConfig->m_strSendTalk, std::vector<int32_t>{ CLogicTalk::talk_red_bag, iRedBagID, 0, iRedBagItemID});

	CLogicGlobal::PublishBroadcast(pstConfig->m_strSendBroadcast, pstConfig->m_iOldTotalYuanbao, {
		{"nick", pUserInfo->m_strNick}, {"item_name", CLogicConfig::GetItemName(CLogicConfigDefine::EXCHANGE_ITEM, iRedBagItemID)}
	});
}

bool CLogicGlobalFrame::ReceiveRedBag(int32_t iUid, int32_t iGroupID, int32_t iRedBagID, CPackGameItem& stReward, bool& bIsBest)
{
    stReward = {}; bIsBest = false;
	if (HasReceiveRedBagYuanbao(iUid, iGroupID, iRedBagID))
        return false;

    auto iter = m_stRedBag.find(iRedBagID);
    if (iter == m_stRedBag.end() || iter->second.m_iExpiredTime < CLogicCommon::GetSec())
        return false;

    auto pConfig = CLogicConfig::GetRedBagConfig().GetRedBagConfig(iter->second.m_iRedBagItemID);
    if(nullptr == pConfig)
        return false;

    auto& stRedBag = iter->second;

    // 旧红包
    if(pConfig->m_strNewRedBag.empty())
    {
        if(stRedBag.m_stReceivePlayer.size() >= stRedBag.m_iReceiveYuanbao.size())
            return false;

        stReward.m_iItemType = CLogicConfigDefine::CURRENCY;
        stReward.m_iCardID = CLogicConfigDefine::ECIT_YuanBaoID;
        stReward.m_iNum = stRedBag.m_iReceiveYuanbao[stRedBag.m_stReceivePlayer.size()];

        stRedBag.m_stReceivePlayer.emplace_back(iUid, iGroupID, stReward);
    }
    else
    {
        // 新红包
        if(stRedBag.m_stCurNewBagReward.empty())
            return false;

        int32_t iCurWeight = 0;
        int32_t iRandWeight = CLogicCommon::GetRandNum() % stRedBag.m_iTotalWeight + 1;
        for(auto iterReward = stRedBag.m_stCurNewBagReward.begin(); iterReward != stRedBag.m_stCurNewBagReward.end(); ++iterReward)
        {
            iCurWeight += iterReward->second.first;
            if(iCurWeight >= iRandWeight)
            {
                auto pRewardConfig = CLogicConfig::GetRedBagConfig().GetNewRedBagReward(pConfig->m_strNewRedBag, iterReward->first);
                if(nullptr == pRewardConfig)
                    return false;

                stReward.m_iItemType = pRewardConfig->m_iItemType;
                stReward.m_iCardID = pRewardConfig->m_iItemID;
                stReward.m_iNum = CLogicCommon::GetRandNum(pRewardConfig->m_iItemNumMin, pRewardConfig->m_iItemNumMax, true);
                bIsBest = pRewardConfig->m_bIsBest;

                stRedBag.m_stReceivePlayer.emplace_back(iUid, iGroupID, stReward, pRewardConfig->m_bIsBest);

                // 处理剩余奖励内容
                --iterReward->second.second;
                if(iterReward->second.second <= 0)
                {
                    stRedBag.m_iTotalWeight -= iterReward->second.first;
                    stRedBag.m_stCurNewBagReward.erase(iterReward);
                }

                break;
            }
        }
    }

	return true;
}

bool CLogicGlobalFrame::HasReceiveRedBagYuanbao(int32_t iUid, int32_t iGroupID, int32_t iRedBagID) const
{
	const auto iter = m_stRedBag.find(iRedBagID);
	if (iter != m_stRedBag.end())
	{
        for(auto& player : iter->second.m_stReceivePlayer)
		{
			if (player.m_iUid == iUid && player.m_iGroupID == iGroupID)
			{
                return true;
			}
		}
	}

	return false;
}

const CLogicGlobalFrame::red_bag_info_t* CLogicGlobalFrame::GetRedBagInfo(int32_t iRedBagID) const
{
	const auto iter = m_stRedBag.find(iRedBagID);;
	return iter == m_stRedBag.end() ? nullptr : &iter->second;
}

CResponseGetRedBagList CLogicGlobalFrame::GetRedBagList(user_data_table_ptr_type pUserInfo)
{
	std::vector<int32_t> deleted;
	CResponseGetRedBagList stRspBody;
	for (const auto& bag : m_stRedBag)
	{
		if (int32_t(bag.second.m_iExpiredTime + 1_hour) < CLogicCommon::GetSec())
		{
			deleted.push_back(bag.first);
		}
		else if (!HasReceiveRedBagYuanbao(pUserInfo->m_iUin, pUserInfo->m_iGroupID, bag.first)
			&& bag.second.m_iExpiredTime > CLogicCommon::GetSec()
			&& (bag.second.m_stReceivePlayer.size() < bag.second.m_iReceiveYuanbao.size() || !bag.second.m_stCurNewBagReward.empty()))
		{
			if (bag.second.m_iGuildID == 0 || bag.second.m_iGuildID == pUserInfo->m_stUserInfoTable.GetGuildID())
			{
				stRspBody.m_stRedBagVec.push_back(GetRedBagBrief(bag.first));
			}
		}
	}

	while (!deleted.empty())
	{
		m_stRedBag.erase(deleted.back());
		deleted.pop_back();
	}

	return stRspBody;
}

int32_t CLogicGlobalFrame::GetRedBagStatus(int32_t iUid, int32_t iGroupID, int32_t iRedBagID) const
{
	const auto* pstRedBag = GetRedBagInfo(iRedBagID);
	if (nullptr == pstRedBag)
	{
		return 1;
	}

    if(pstRedBag->m_iReceiveYuanbao.empty())
    {
        // 新红包
        if(pstRedBag->m_stCurNewBagReward.empty())
            return 2;
    }
    else
    {
        // 旧红包
        if(pstRedBag->m_stReceivePlayer.size() >= pstRedBag->m_iReceiveYuanbao.size())
            return 2;
    }

	if (pstRedBag->m_iExpiredTime < CLogicCommon::GetSec())
	{
		return 3;
	}

	if (HasReceiveRedBagYuanbao(iUid, iGroupID, iRedBagID))
    {
	    return 4;
    }

	return 0;
}

CPackRedBagBrief CLogicGlobalFrame::GetRedBagBrief(int32_t iRedBagID) const
{
	CPackRedBagBrief stPack;
	const auto* pstInfo = GetRedBagInfo(iRedBagID);
	if (nullptr != pstInfo)
	{
		auto* pstRedBagUser = CLogicCacheCommon::LoadUserData(pstInfo->m_iGroupID, pstInfo->m_iUid);
		if (pstRedBagUser)
		{
			stPack.m_iRedBagID = iRedBagID;
			stPack.m_iUid = pstRedBagUser->m_iUin;
			stPack.m_iGroupID = pstRedBagUser->m_iGroupID;
			stPack.m_strNick = pstRedBagUser->m_strNick;
			stPack.m_iAvatarID = pstRedBagUser->m_stUserInfoTable.GetAvatarID();
			stPack.m_iAvatarBorder = pstRedBagUser->m_stUserInfoTable.GetAvatarBorder();
			stPack.m_iTalkChannel = pstInfo->m_iTalkChannel;
			stPack.m_iTotalYuanbao = pstInfo->m_iTotalYuanbao;
			stPack.m_iTotalCount = pstInfo->m_iTotalCount;
			stPack.m_iRedBagItemID = pstInfo->m_iRedBagItemID;
		}
	}
	return stPack;
}

