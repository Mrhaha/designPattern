#pragma once

#include "server_task_protocol.h"
#include "server_cross_protocol.h"
#include "logic_game_cache_table.h"

class CLogicTalkChannelStrategy
{
public:
    CLogicTalkChannelStrategy() : m_bSelfTalk(false) {}
    virtual ~CLogicTalkChannelStrategy() {}

    // 检查合法性
    virtual bool CheckTalk(user_data_table_ptr_type pUser, CRequestTalk& stPara, std::string& strClientIP);
    // 发送跨服
    virtual bool SendToCross(user_data_table_ptr_type pUser, const CRequestTalk& stPara);
    // 发送聊天，缓存聊天
    void HandleTalk(CPackTalkInfo& stTalkInfo, int iDstUin, int iDstGroupID, std::vector<int>& stIDVec);

private:
    virtual bool CheckTalkByChannel(user_data_table_ptr_type pUser, const CRequestTalk& stPara, std::string& strClientIP) = 0;
    virtual void SendTalk(CPackTalkInfo& stTalkInfo, int iDstUin, int iDstGroupID, std::vector<int>& stIDVec) = 0;
    virtual void CacheTalk(CPackTalkInfo& stTalkInfo, int iDstUin, int iDstGroupID) = 0;

protected:
    bool m_bSelfTalk;           // 是否仅自己可见
};

class CLogicTalkChannelWorldStrategy : public CLogicTalkChannelStrategy
{
public:

private:
    bool CheckTalkByChannel(user_data_table_ptr_type pUser, const CRequestTalk& stPara, std::string& strClientIP) override;
    void SendTalk(CPackTalkInfo& stTalkInfo, int iDstUin, int iDstGroupID, std::vector<int>& stIDVec) override;
    void CacheTalk(CPackTalkInfo& stTalkInfo, int iDstUin, int iDstGroupID) override;
};

class CLogicTalkChannelGuildStrategy : public CLogicTalkChannelStrategy
{
public:

private:
    bool CheckTalkByChannel(user_data_table_ptr_type pUser, const CRequestTalk& stPara, std::string& strClientIP) override;
    void SendTalk(CPackTalkInfo& stTalkInfo, int iDstUin, int iDstGroupID, std::vector<int>& stIDVec) override;
    void CacheTalk(CPackTalkInfo& stTalkInfo, int iDstUin, int iDstGroupID) override;
};

class CLogicTalkChannelPrivateStrategy : public CLogicTalkChannelStrategy
{
public:
    bool SendToCross(user_data_table_ptr_type pUser, const CRequestTalk& stPara) override;

private:
    bool CheckTalkByChannel(user_data_table_ptr_type pUser, const CRequestTalk& stPara, std::string& strClientIP) override;
    void SendTalk(CPackTalkInfo& stTalkInfo, int iDstUin, int iDstGroupID, std::vector<int>& stIDVec) override;
    void CacheTalk(CPackTalkInfo& stTalkInfo, int iDstUin, int iDstGroupID) override;
};

using talk_strategy_sp = std::shared_ptr<CLogicTalkChannelStrategy>;

class CLogicTalk
{
public:
	enum ETalkExParam
	{
		talk_private = 0,				//正常私聊
		talk_gift_xianhua = 1,			//赠送鲜花
		talk_gift_fashion = 1,			//赠送时装
		talk_team_invite = 3,			//组队邀请
		talk_red_bag = 4,				//发送红包
	};

	static bool FilterTalkContent(user_data_table_ptr_type pUser, std::string& strTalkContent);

    static bool CheckUTF8ContentLength(const std::string& strContent, const int32_t iLength);
	
	static int SendWordBroadcastTalk(user_data_table_ptr_type pUser, int32_t iTalkChannel, const std::string& strContent, const std::vector<int>& exParam);

    static talk_strategy_sp CreateTalkChannelStrategy(int32_t iChannelType);
};
