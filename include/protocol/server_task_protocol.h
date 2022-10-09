#pragma once

#include "msgpack.hpp"

#include "server_base_protocol.h"
#include "server_common_protocol.h"

////////////////////////////////////////////////////////////////////////////////////////
class CRequestCommonDailyAward
{
public:
    CRequestCommonDailyAward() : m_iDailyAwardStatType(0) {}

    MSGPACK_DEFINE(m_iDailyAwardStatType);

    int32_t     m_iDailyAwardStatType;          // EnumDailyAwardStateType 每日奖励通用状态标志位类型
};

class CResponseCommonDailyAward : public CPackUserSyncData
{
public:
    CResponseCommonDailyAward() : m_strDailyAwardState(""), m_stAwardItems() {}

    MSGPACK_DEFINE(m_stSyncData, m_strDailyAwardState, m_stAwardItems);

    std::string                 m_strDailyAwardState;
    std::vector<CPackGameItem>  m_stAwardItems;
};

class CRequestHeroDating
{
public:
    CRequestHeroDating() : m_iCardID(0), m_iPlaceID(0), m_iDialogID(0), m_iAnswerPara(0), m_iConsumeItemID(0) {}

    MSGPACK_DEFINE(m_iCardID, m_iPlaceID, m_iDialogID, m_iAnswerPara, m_iConsumeItemID);

    int32_t     m_iCardID;
    int32_t     m_iPlaceID;
    int32_t     m_iDialogID;
    int32_t     m_iAnswerPara;          // 普通对话的话填这个值
    int32_t     m_iConsumeItemID;       // 特殊对话，发送消耗的道具ID
};

class CResponseHeroDating : public CPackUserSyncData
{
public:
    CResponseHeroDating() : m_iCardID(0), m_iPlaceID(0), m_iDialogID(0), m_iConsumeItemID(0), m_iPlaceDailyTimes(0), m_iHeartLevel(0), m_iHeartExp(0), m_stImpression(), m_stReward() {}

    MSGPACK_DEFINE(m_stSyncData, m_iCardID, m_iPlaceID, m_iDialogID, m_iConsumeItemID, m_iPlaceDailyTimes, m_iHeartLevel, m_iHeartExp, m_stImpression, m_stReward);

    int32_t     m_iCardID;
    int32_t     m_iPlaceID;
    int32_t     m_iDialogID;
    int32_t     m_iConsumeItemID;           // 特殊对话，发送消耗的道具ID
    int32_t     m_iPlaceDailyTimes;         // 地点每日次数
    int32_t     m_iHeartLevel;
    int32_t     m_iHeartExp;
    std::vector<int> m_stImpression;        // 印象分
    std::vector<CPackGameItem> m_stReward;  // 道具奖励
};

class CRequestQuestReward
{
public:
    CRequestQuestReward() : m_stQuestIDVec() {}

    MSGPACK_DEFINE(m_stQuestIDVec);

    std::vector<int32_t> m_stQuestIDVec;
};

class CResponseQuestReward : public CPackUserSyncData
{
public:
    CResponseQuestReward() : m_stQuestVec(), m_stItemData() {}

    MSGPACK_DEFINE(m_stSyncData, m_stQuestVec, m_stItemData);

    std::vector<CPackQuestData> m_stQuestVec;
    CPackTotalItemData          m_stItemData;
};

///////////////////////////////////////////////////////////////////////////////////////////////
//主线任务和成就任务
class CResponseMutiStepTaskStatus : public CPackUserSyncData
{
public:
	CResponseMutiStepTaskStatus() : m_sMutiStepTaskDetailVec() {}

	MSGPACK_DEFINE(m_stSyncData, m_sMutiStepTaskDetailVec);

	struct muti_step_task_detail
	{
		MSGPACK_DEFINE(m_iTaskID, m_iTaskStep, m_iTargetNum);

		muti_step_task_detail() : m_iTaskID(0), m_iTaskStep(0), m_iTargetNum(0) {  }

		int             m_iTaskID;
		int             m_iTaskStep;
		int             m_iTargetNum;
	};

	std::vector<muti_step_task_detail>          m_sMutiStepTaskDetailVec;
};

class CRequestGetMutiStepTaskGift
{
public:
	MSGPACK_DEFINE(m_iTaskID);

	CRequestGetMutiStepTaskGift() : m_iTaskID(0) {  }

	int             m_iTaskID;
};

class CResponseGetMutiStepTaskGift : public CPackUserSyncData
{
public:
	CResponseGetMutiStepTaskGift()
		: m_iTaskID(0), m_iNextStep(0), m_iNextStepTargetNum(0), m_stBonus() {}

	MSGPACK_DEFINE(m_stSyncData, m_iTaskID, m_iNextStep, m_iNextStepTargetNum, m_stBonus);

	int                                         m_iTaskID;
	int                                         m_iNextStep;
	int                                         m_iNextStepTargetNum;

	std::vector<CPackGameItem>                  m_stBonus;
};

////////////////////////////////////////////////////////////////////////////////
class CRequestRecordUserGuide
{
public:
	CRequestRecordUserGuide() : m_iMajorStep(0), m_iMinorStep(0) {  }

	MSGPACK_DEFINE(m_iMajorStep, m_iMinorStep);

	int                                 m_iMajorStep;
	int                                 m_iMinorStep;
};

class CResponseRecordUserGuide : public CPackUserSyncData
{
public:
	CResponseRecordUserGuide()
		: m_iMajorStep(0), m_iMinorStep(0) {  }

	MSGPACK_DEFINE(m_stSyncData, m_iMajorStep, m_iMinorStep);

	int                                 m_iMajorStep;
	int                                 m_iMinorStep;
};

///////////////////////////////////////////////////////////////////////////////////////////////
class CPackGetPvpRankListData
{
public:
	CPackGetPvpRankListData()
		: m_strNick(""), m_iLevel(0), m_iTotalCombat(0), m_iLeaderCardID(0),
		m_iAvatarIcon(0), m_iAvatarBorder(0), m_stCardInfoList(), m_iUid(0), m_iGroupID(0), m_strGuildName() {}

	MSGPACK_DEFINE(m_strNick, m_iLevel, m_iTotalCombat, m_iLeaderCardID,
		m_iAvatarIcon, m_iAvatarBorder, m_stCardInfoList, m_iUid, m_iGroupID, m_strGuildName);

	std::string                                         m_strNick;
	int                                                 m_iLevel;
	int                                                 m_iTotalCombat;
	int                                                 m_iLeaderCardID;
	int                                                 m_iAvatarIcon;
	int                                                 m_iAvatarBorder;
	std::vector<CPackRankBaseCardInfo>    m_stCardInfoList;
	int32_t m_iUid;
	int32_t m_iGroupID;
	std::string m_strGuildName;
};

class CResponseGetPvpRankList : public CPackUserSyncData
{
public:
	CResponseGetPvpRankList()
		: m_stRankList(), m_iPVPElo(0), m_iCurrentSelfRank(0), m_iMaxSelfRank(0) {}

	MSGPACK_DEFINE(m_stSyncData, m_stRankList, m_iPVPElo, m_iCurrentSelfRank, m_iMaxSelfRank);

	std::vector<CPackGetPvpRankListData>     m_stRankList;
	int                                                     m_iPVPElo;
	int                                                     m_iCurrentSelfRank;
	int                                                     m_iMaxSelfRank;
};

class CResponseGetSelfSimpleRankInfo : public CPackUserSyncData
{
public:
    CResponseGetSelfSimpleRankInfo()
        : m_iPVPRankNum(0), m_iPVPRankMaxNum(0), m_iMatchPVPScore(0), m_iUnlimitRankMaxNum(0), m_iUnlimitMaxPass(0)
        , m_iTowerRankCurNum(0), m_iTowerMaxOrder(0), m_iTerritoryRankCurNum(0), m_iTerritoryMaxOrder(0)
        , m_iEntireWarMilitarkLevel(0) {}

    MSGPACK_DEFINE(m_stSyncData, m_iPVPRankNum, m_iPVPRankMaxNum, m_iMatchPVPScore, m_iUnlimitRankMaxNum, m_iUnlimitMaxPass
            , m_iTowerRankCurNum, m_iTowerMaxOrder, m_iTerritoryRankCurNum, m_iTerritoryMaxOrder, m_iEntireWarMilitarkLevel);

    int32_t         m_iPVPRankNum;              // 当前PVP排行榜排名
    int32_t         m_iPVPRankMaxNum;           // PVP排行榜最高排名

    int32_t         m_iMatchPVPScore;           // 巅峰对决积分

    int32_t         m_iUnlimitRankMaxNum;       // 无尽关卡最高排行     // TODO
    int32_t         m_iUnlimitMaxPass;          // 无尽关卡最高层数     // TODO

    int32_t         m_iTowerRankCurNum;         // 爬塔当前排名
    int32_t         m_iTowerMaxOrder;           // 爬塔最高层数

    int32_t         m_iTerritoryRankCurNum;     // 步步为营当前排名     // TODO
    int32_t         m_iTerritoryMaxOrder;       // 步步为营最大进度     // TODO

    int32_t         m_iEntireWarMilitarkLevel;  // 全民战役军衔等级

};

class CResponseGetPvpLastDayRankList : public CPackUserSyncData
{
public:
	CResponseGetPvpLastDayRankList()
		: m_stRankList(), m_iPVPElo(0), m_iCurrentSelfRank(0), m_iMaxSelfRank(0) {}

	MSGPACK_DEFINE(m_stSyncData, m_stRankList, m_iPVPElo, m_iCurrentSelfRank, m_iMaxSelfRank);

	std::vector<CPackGetPvpRankListData>     m_stRankList;
	int                                                     m_iPVPElo;
	int                                                     m_iCurrentSelfRank;
    int                                                     m_iMaxSelfRank;
};

class CResponseGetFightPowerSelfRank : public CPackUserSyncData
{
public:
	CResponseGetFightPowerSelfRank() : m_iRankNum(0) {}

	MSGPACK_DEFINE(m_stSyncData, m_iRankNum);

	int             m_iRankNum;
};

class CRequestGetFightPowerRankList
{
public:
	CRequestGetFightPowerRankList() : m_iRankAmount(0) {}

	MSGPACK_DEFINE(m_iRankAmount);

	int             m_iRankAmount;
};

class CPackFightPowerRankListData
{
public:
	CPackFightPowerRankListData()
		: m_strNick("")
		, m_iLevel(0)
		, m_iTotalCombat(0)
		, m_iLeaderCardID(0)
		, m_iAvatarIcon(0)
		, m_iAvatarBorder(0)
		, m_stCardInfoList()
		, m_iUid(0)
		, m_iGroupID(0), m_strGuildName(), m_iEquipHonorID(0){}

	MSGPACK_DEFINE(m_strNick, m_iLevel, m_iTotalCombat, m_iLeaderCardID,
		m_iAvatarIcon, m_iAvatarBorder, m_stCardInfoList, m_iUid, m_iGroupID, m_strGuildName, m_iEquipHonorID);

	std::string                                         m_strNick;
	int                                                 m_iLevel;
	int                                                 m_iTotalCombat;
	int                                                 m_iLeaderCardID;
	int                                                 m_iAvatarIcon;
	int                                                 m_iAvatarBorder;
	std::vector<CPackRankBaseCardInfo>    m_stCardInfoList;
	int32_t m_iUid;
	int32_t m_iGroupID;
	std::string m_strGuildName;
	int32_t m_iEquipHonorID;
};

class CResponseGetFightPowerRankList : public CPackUserSyncData
{
public:
	CResponseGetFightPowerRankList() : m_stRankList(), m_iSelfRankNum(0), m_iSelfHighestRank(0) {}

	MSGPACK_DEFINE(m_stSyncData, m_stRankList, m_iSelfRankNum, m_iSelfHighestRank);

	std::vector<CPackFightPowerRankListData>  m_stRankList;
	int32_t m_iSelfRankNum;
	int32_t m_iSelfHighestRank;
};

//GET_MILITARK_LEVEL_RANK = 10294,			//获取军衔等级排行
class CResponseGetMilitarkLevelRank : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_iSelfRankNum, m_stRank, m_iSelfMilitarkLevel);

	int32_t m_iSelfRankNum;
	std::vector<CPackUserRank> m_stRank;
	int32_t m_iSelfMilitarkLevel;
};

//GET_CHARM_RANK_LIST = 10309,				//获取魅力值排行榜
class CResponseGetCharmRank : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_iSelfRankNum, m_stRank, m_iCharm);

	int32_t m_iSelfRankNum;
	std::vector<CPackUserRank> m_stRank;
	int32_t m_iCharm;
};

//GET_MATCH_PVP_RANK_LIST = 10323,			//获取巅峰对决本服排行
class CResponseGetMatchPvpRank : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_iSelfRankNum, m_stRank, m_iSelfPvpScore);

	int32_t m_iSelfRankNum;
	std::vector<CPackUserRank> m_stRank;
	int32_t m_iSelfPvpScore;
};

//IBUSY_GET_SCORE_RANK_LIST = 10812,				//获取排行榜
class CResponseGetIbusyScoreRank : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_iSelfRankNum, m_stRank, m_iScore);

	int32_t m_iSelfRankNum;
	std::vector<CPackUserRank> m_stRank;
	int32_t m_iScore;
};

// 战力排行榜活动阶段奖励
class CRequestFightPowerRankActiveStageBonus
{
public:
	CRequestFightPowerRankActiveStageBonus() {}

	MSGPACK_DEFINE(m_iIndex);

	int             m_iIndex;
};

class CResponseFightPowerRankActiveStageBonus : public CPackUserSyncData
{
public:
	CResponseFightPowerRankActiveStageBonus() {}

	MSGPACK_DEFINE(m_stSyncData, m_iIndex, m_stBonusVec);

	int                                             m_iIndex;
	std::vector<CPackGameItem>  m_stBonusVec;
};

//////////////////////////////////////////////////////////////////////////////////////////////
class CRequestTalk
{
public:
    // 聊天频道类型
    enum
    {
        ETCT_World = 1,             // 世界频道
        ETCT_Guild = 2,             // 公会频道
        ETCT_Private = 3,           // 私聊频道
    };

    CRequestTalk() : m_strTalkData(), m_iTalkChannel(0), m_iDstUin(0), m_iDstGroupID(0) {}

    MSGPACK_DEFINE(m_strTalkData, m_iTalkChannel, m_iDstUin, m_iDstGroupID);

    std::string     m_strTalkData;
    int32_t         m_iTalkChannel;
    int32_t         m_iDstUin;
    int32_t         m_iDstGroupID;
};

class CNotifyPushTalk : public CPackUserSyncData
{
public:
    CNotifyPushTalk() : m_stTalkInfo() {}

    MSGPACK_DEFINE(m_stSyncData, m_stTalkInfo);

    CPackTalkInfo   m_stTalkInfo;
};

class CNotifyOfflineMessage : public CPackUserSyncData
{
public:
    CNotifyOfflineMessage() : m_stMessageVec(), m_stPrivateTalkMap() {}

    MSGPACK_DEFINE(m_stSyncData, m_stMessageVec, m_stPrivateTalkMap);

    std::vector<CPackTalkInfo>                          m_stMessageVec;
    std::unordered_map<int, CPackPrivateTalkOneUser>    m_stPrivateTalkMap;
};

////////////////////////////////////////////////////////////////////////////////////////////
class CPackBroadcastMsg
{
public:
	CPackBroadcastMsg()
		: m_iPriority(0), m_strContent(){}

	MSGPACK_DEFINE(m_iPriority, m_strContent);

	int                 m_iPriority;
	std::string         m_strContent;
};

class CResponseBroadcast : public CPackUserSyncData
{
public:
	CResponseBroadcast() : m_stMsgList() {}

	MSGPACK_DEFINE(m_stSyncData, m_stMsgList);

	std::vector<CPackBroadcastMsg>      m_stMsgList;
};

class CPackBroadcastTalkMsg
{
public:
	CPackBroadcastTalkMsg()
		: m_iSendTime(0), m_iPriority(0), m_strContent(), m_iJump(0), m_vecJumpParam(){}

	MSGPACK_DEFINE(m_iSendTime, m_iPriority, m_strContent, m_iJump, m_vecJumpParam);
	int                 m_iSendTime;
	int                 m_iPriority;
	std::string         m_strContent;
	int					m_iJump;
	std::vector<int>	m_vecJumpParam;
};

class CResponseBroadcastTalk : public CPackUserSyncData
{
public:
	CResponseBroadcastTalk() : m_stMsgList() {}

	MSGPACK_DEFINE(m_stSyncData, m_stMsgList);

	std::vector<CPackBroadcastTalkMsg>      m_stMsgList;
};

//////////////////////////////////////////////////////////////////////////////////////////////
class CRequestJubaoUser
{
public:
	MSGPACK_DEFINE(m_iDesGroupID, m_iDesUin, m_iType, m_strContent, m_strFromUI);

	int						m_iDesGroupID;
	int						m_iDesUin;
	int                     m_iType;
	std::string             m_strContent;
	std::string				m_strFromUI;
};

//添加、删除、屏蔽玩家聊天请求
class CRequestOperateUser
{
public:
	CRequestOperateUser() : m_iUid(0), m_iGroupID(0) {}

	MSGPACK_DEFINE(m_iUid, m_iGroupID);

	int32_t         m_iUid;
	int32_t         m_iGroupID;
};

class CRequestOpenCommonRank
{
  public:
    CRequestOpenCommonRank() : m_iRankID(0) {}

    MSGPACK_DEFINE(m_iRankID);

    int32_t     m_iRankID;
};

class CResponseOpenCommonRank : public CPackUserSyncData
{
  public:
    CResponseOpenCommonRank() : m_iRankID(0), m_iSelfRankNum(0), m_stRank(), m_iValue(0) {}

    MSGPACK_DEFINE(m_stSyncData, m_iRankID, m_iSelfRankNum, m_stRank, m_iValue);

    int32_t     m_iRankID;
    int32_t     m_iSelfRankNum;
    std::vector<CPackUserRank> m_stRank;
    int32_t     m_iValue;
};

class CResponseGetCommonRankSelfInfo : public CPackUserSyncData
{
  public:
    CResponseGetCommonRankSelfInfo() : m_iRankID(0), m_iSelfRankNum(0), m_iValue(0) {}

    MSGPACK_DEFINE(m_stSyncData, m_iRankID, m_iSelfRankNum, m_iValue);

    int32_t     m_iRankID;
    int32_t     m_iSelfRankNum;
    int32_t     m_iValue;
};

//////////////////////////////////////////////////////////////////////////
class CRequestRecordUserSpecialGuide
{
public:
    CRequestRecordUserSpecialGuide() : m_ilSpecialStep(0) {  }

    MSGPACK_DEFINE(m_ilSpecialStep);
    uint64_t    m_ilSpecialStep;
};

class CResponseRecordUserSpecialGuide : public CPackUserSyncData
{
public:
    CResponseRecordUserSpecialGuide()
        : m_ilSpecialStep(0) , m_ilRecordSpecialStep(0){  }

    MSGPACK_DEFINE(m_stSyncData,m_ilSpecialStep,m_ilRecordSpecialStep);

    uint64_t    m_ilSpecialStep;
    uint64_t    m_ilRecordSpecialStep;
};