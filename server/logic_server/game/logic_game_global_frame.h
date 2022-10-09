#pragma once
#include "tc_singleton.h"
#include "server_award_protocol.h"
#include "server_task_protocol.h"
#include "server_recovery_protocol.h"
#include "logic_game_cache_table.h"

class CLogicGlobalFrame
{
public:
	CLogicGlobalFrame();

	~CLogicGlobalFrame();

	bool Initialize();

	void Refresh();

	void OnCrossOneDay();

	void RefreshOnlineAction();

/************************************************************************************************************/
/******************************************世  界  聊  天*****************************************************/
/************************************************************************************************************/
public:
    void AddWorldTalkCache(const CPackTalkInfo& stTalk);
	void RemoveNickWorldTalk(const std::string& strNick);
    const talk_cache_list* GetWorldTalkList() const { return &m_stWorldTalkList; }

    void AddGuildTalkCache(const CPackTalkInfo& stTalk, int iGuildID);
    const talk_cache_list* GetGuildTalkList(int iGuildID) const;

    bool IsRepeatTalk(int32_t iUin, int32_t iGroupID, const std::string& strMsg);

	void AddBroadcastPushProto(const CPackBroadcastTalkMsg& rstProto);
	const std::list<CPackBroadcastTalkMsg>& GetBroadcastList() const { return m_stBroadcastList; }

private:
    talk_cache_list m_stWorldTalkList;
    std::unordered_map<int, talk_cache_list> m_stGuildTalkList;
	std::list<CPackBroadcastTalkMsg> m_stBroadcastList;

	std::unordered_map<std::string, int32_t> m_stTalkCount;

/************************************************************************************************************/
/******************************************开  心  翻  牌*****************************************************/
/************************************************************************************************************/
public:
	bool RefreshHappyTurnCardVec();

	const std::vector<CPackGameItem>& GetTurnCardVec() const { return m_stTurnCardVec; }

private:
	std::vector<CPackGameItem> m_stTurnCardVec;

/************************************************************************************************************/
/******************************************抢    红    包*****************************************************/
/************************************************************************************************************/
public:
    struct red_bag_receive_t
    {
    public:
        red_bag_receive_t(int32_t iUid = 0, int32_t iGroupID = 0, CPackGameItem stReward = {}, bool bIsBest = false)
        {
            m_iUid = iUid;
            m_iGroupID = iGroupID;
            m_stReward = stReward;
            m_bIsBest = bIsBest;
        }

        int32_t m_iUid = 0;
        int32_t m_iGroupID = 0;
        CPackGameItem m_stReward;
        bool    m_bIsBest = false;
    };

	struct red_bag_info_t
	{		
		int32_t m_iUid = 0;
		int32_t m_iGroupID = 0;
		int32_t m_iGuildID = 0;
		int32_t m_iRedBagItemID = 0;
		int32_t m_iExpiredTime = 0;
		int32_t m_iTalkChannel = 0;
        int32_t m_iTotalCount = 0;      // 红包总共可领取数量

        // 旧红包数据
		int32_t m_iTotalYuanbao = 0;
		std::vector<int32_t> m_iReceiveYuanbao;

        // 新红包数据
        int32_t m_iTotalWeight = 0;     // 当前剩余奖励总权重
        std::map<int32_t, std::pair<int32_t, int32_t>> m_stCurNewBagReward;     // 当前剩余奖励包内容 [index, <weight, count>]

        std::vector<red_bag_receive_t> m_stReceivePlayer;   // 红包领取列表
	};

	void SendRedBag(user_data_table_ptr_type pUserInfo, int32_t iRedBagItemID);

	bool ReceiveRedBag(int32_t iUid, int32_t iGroupID, int32_t iRedBagID, CPackGameItem& stReward, bool& bIsBest);

	bool HasReceiveRedBagYuanbao(int32_t iUid, int32_t iGroupID, int32_t iRedBagID) const;

	const red_bag_info_t* GetRedBagInfo(int32_t iRedBagID) const;

	CResponseGetRedBagList GetRedBagList(user_data_table_ptr_type pUserInfo);

	int32_t GetRedBagStatus(int32_t iUid, int32_t iGroupID, int32_t iRedBagID) const;

	CPackRedBagBrief GetRedBagBrief(int32_t iRedBagID) const;

private:
	int32_t m_iRedBagMaxIndex;
	std::map<int32_t, red_bag_info_t> m_stRedBag;
};

using LOGIC_GLOBAL_FRAME_SINGLE = taf::TC_Singleton<CLogicGlobalFrame> ;

#define LOGIC_GLOBAL_FRAME_INS LOGIC_GLOBAL_FRAME_SINGLE::getInstance()
