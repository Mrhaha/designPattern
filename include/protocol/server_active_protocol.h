#pragma once

#include "msgpack.hpp"

#include "server_base_protocol.h"
#include "server_recovery_protocol.h"

class CPackActivityTask
{
public:
	explicit CPackActivityTask(int32_t iActivityType = 0, int32_t iActivityID = 0, int32_t iTaskID = 0, int32_t iProgress = 0, int32_t iState = 0)
		: m_iActivityType(iActivityType), m_iActivityID(iActivityID), m_iTaskID(iTaskID), m_iProgress(iProgress), m_iState(iState) {}

	MSGPACK_DEFINE(m_iActivityType, m_iActivityID, m_iTaskID, m_iProgress, m_iState);

	int32_t m_iActivityType;
	int32_t m_iActivityID;
	int32_t m_iTaskID;
	int32_t m_iProgress;
	int32_t m_iState;
};

class CNotifyAllAvyInfo : public CPackUserSyncData
{
public:
    CNotifyAllAvyInfo() : m_stAvyVec() {}

	MSGPACK_DEFINE(m_stSyncData, m_stAvyVec);

	std::vector<CPackAvyInfo> m_stAvyVec;
};

//NOTIFY_UPDATE_ACTIVITY_TASK = 10264,		//通知活动任务更新
class CNotifyUpdateActivityTask : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_stActivityTask);

	CPackActivityTask m_stActivityTask;
};

//AWARD_ACTIVITY_MULTI_TASK = 10267,			//皇叔多重任务悬赏领奖
//rsp之前有NOTIFY_UPDATE_ACTIVITY_TASK
class CRequestAwardActivityMultiTask
{
public:
	CRequestAwardActivityMultiTask() : m_iTaskID(0) {}

	MSGPACK_DEFINE(m_iTaskID);

	int32_t m_iTaskID;
};

class CResponseAwardActivityMultiTask : public CPackUserSyncData
{
public:
	CResponseAwardActivityMultiTask() : m_iTaskID(0), m_stBonusItemVec() {}

	MSGPACK_DEFINE(m_stSyncData, m_iTaskID, m_stBonusItemVec);

	int32_t m_iTaskID;
	std::vector<CPackGameItem> m_stBonusItemVec;
};

//GET_ACTIVITY_TASK_AWARD = 10293,			//获取活动任务就奖励
class CRequestGetActivityTaskAward
{
public:
	CRequestGetActivityTaskAward() : m_iActivityType(0), m_iActivityID(0), m_iTaskID(0) {}

	MSGPACK_DEFINE(m_iActivityType, m_iActivityID, m_iTaskID);

	int32_t m_iActivityType;
	int32_t m_iActivityID;
	int32_t m_iTaskID;
};

class CResponseGetActivityTaskAward : public CPackUserSyncData
{
public:
	CResponseGetActivityTaskAward() : m_iActivityType(0), m_iActivityID(0), m_iTaskID(0), m_stBonusItemVec() {}

	MSGPACK_DEFINE(m_stSyncData, m_iActivityType, m_iActivityID, m_iTaskID, m_stBonusItemVec);

	int32_t m_iActivityType;
	int32_t m_iActivityID;
	int32_t m_iTaskID;
	std::vector<CPackGameItem> m_stBonusItemVec;
};


////////////////////////////////////////////////////////////////////////////////
class CRequestGetActiveInfo
{
public:
	CRequestGetActiveInfo() : m_iActiveID(0) { }

	MSGPACK_DEFINE(m_iActiveID);

	int         m_iActiveID;
};

class CResponseGetActiveInfo : public CPackUserSyncData
{
public:
	CResponseGetActiveInfo()
		: m_iActiveID(0), m_iLastReceiveAwardTime(0), m_strReceivedBitmap(""),
		m_strCompleteBitmap(""), m_iTargetNum(0), m_stParamMap(), m_strParaBitmap("") {}

	MSGPACK_DEFINE(m_stSyncData, m_iActiveID, m_iLastReceiveAwardTime,
		m_strReceivedBitmap, m_strCompleteBitmap, m_iTargetNum, m_stParamMap, m_strParaBitmap);

	int                                         m_iActiveID;
	int                                         m_iLastReceiveAwardTime;
	std::string                                 m_strReceivedBitmap;
	std::string                                 m_strCompleteBitmap;
	int                                         m_iTargetNum;
	std::map<std::string, std::string>          m_stParamMap;
    std::string                                 m_strParaBitmap;
};

class CPackActiveInfo
{
public:
	CPackActiveInfo()
		: m_iActiveID(0), m_iLastReceiveAwardTime(0), m_strReceivedBitmap("0")
		, m_strCompleteBitmap("0"), m_iTargetNum(0), m_stParamMap(), m_strParaBitmap("0") {}

	MSGPACK_DEFINE(m_iActiveID, m_iLastReceiveAwardTime,
		m_strReceivedBitmap, m_strCompleteBitmap, m_iTargetNum, m_stParamMap, m_strParaBitmap);

	int                                   m_iActiveID;
	int                                   m_iLastReceiveAwardTime;
	std::string                           m_strReceivedBitmap;
	std::string                           m_strCompleteBitmap;
	int                                   m_iTargetNum;
	std::map<std::string, std::string>          m_stParamMap;
    std::string                                 m_strParaBitmap;
};

class CResponseGetAllActiveInfo : public CPackUserSyncData
{
public:
	CResponseGetAllActiveInfo() : m_stActiveInfo() {}

	MSGPACK_DEFINE(m_stSyncData, m_stActiveInfo);

	std::vector<CPackActiveInfo>     m_stActiveInfo;
};

//RECEIVE_ACTIVE_BONUS = 10084,			//获取活动奖励
class CRequestGetActiveGift
{
public:
	CRequestGetActiveGift() : m_iActiveID(0), m_iEntityID(0), m_iTimes(1) { }

	MSGPACK_DEFINE(m_iActiveID, m_iEntityID, m_iTimes);

	int         m_iActiveID;
	int         m_iEntityID;
    int         m_iTimes;       // 一次性领取次数
};

class CResponseGetActiveGift : public CPackUserSyncData
{
public:
	CResponseGetActiveGift() : m_iActiveID(0), m_iLastReceiveAwardTime(0), m_strReceivedBitmap(), m_strCompleteBitmap(),
        m_stGameItemList(), m_stParamMap(), m_strParaBitmap(), m_iEntityID(0), m_iTimes(0), m_stConstelVec() {}

	MSGPACK_DEFINE(m_stSyncData, m_iActiveID, m_iLastReceiveAwardTime, m_strReceivedBitmap, m_strCompleteBitmap,
                   m_stGameItemList, m_stParamMap, m_strParaBitmap, m_iEntityID, m_iTimes, m_stConstelVec);

	int                                                 m_iActiveID;
	int                                                 m_iLastReceiveAwardTime;
	std::string                                         m_strReceivedBitmap;
	std::string                                         m_strCompleteBitmap;
	std::vector<CPackGameItem>                          m_stGameItemList;
	std::map<std::string, std::string>                  m_stParamMap;
	std::string                                         m_strParaBitmap;
    int         m_iEntityID;
    int         m_iTimes;                   // 一次性领取次数
    std::vector<CPackConstellationInfo>                 m_stConstelVec;     // 星纹
};

class CPackActiveStatus
{
public:
	CPackActiveStatus() : m_iActivityID(0), m_iRedPoint(0) {}

	MSGPACK_DEFINE(m_iActivityID, m_iRedPoint);

	int         m_iActivityID;
	int         m_iRedPoint;
};

class CNotifyActiveStatus : public CPackUserSyncData
{
public:
	CNotifyActiveStatus() {}

	MSGPACK_DEFINE(m_stSyncData, m_stActiveStatus);

	std::vector<CPackActiveStatus>  m_stActiveStatus;
};

class CRequestGetActiveLevelRaceRank
{
public:
	CRequestGetActiveLevelRaceRank() : m_iActiveID(0), m_iBeginRank(0), m_iNum(0) { }

	MSGPACK_DEFINE(m_iActiveID, m_iBeginRank, m_iNum);

	int         m_iActiveID;
	int         m_iBeginRank;
	int         m_iNum;
};

class CPackLevelRaceRank
{
public:
	CPackLevelRaceRank() : m_iTime(0), m_strName("") {}

	MSGPACK_DEFINE(m_iTime, m_strName);

	int             m_iTime;
	std::string     m_strName;
};

class CResponseGetActiveLevelRaceRank : public CPackUserSyncData
{
public:
	CResponseGetActiveLevelRaceRank() : m_iActiveID(0), m_iTotalAchieve(0), m_stLevelRaceRankInfo() {}

	MSGPACK_DEFINE(m_stSyncData, m_iActiveID, m_iTotalAchieve, m_stLevelRaceRankInfo);

	int                                 m_iActiveID;
	int                                 m_iTotalAchieve;
	std::vector<CPackLevelRaceRank>		m_stLevelRaceRankInfo;
};

////////////////////////////////////////////////////////////////////////////////////////
class CPackUserStatInfoElem
{
public:
	MSGPACK_DEFINE(m_iStatDate, m_iPayMoney);

    CPackUserStatInfoElem() {}

	int m_iStatDate;
	int m_iPayMoney;
};

class CResponseGetUserStatList : public CPackUserSyncData
{
public:
	CResponseGetUserStatList() : m_stUserStatVec() {}

	MSGPACK_DEFINE(m_stSyncData, m_stUserStatVec);

	std::vector<CPackUserStatInfoElem>       m_stUserStatVec;
};

////////////////////////////////////////////////////////////////////////////////////////
class CRequestGetSerialPayAward
{
public:
	CRequestGetSerialPayAward() {}

	MSGPACK_DEFINE(m_iSerialID, m_iBoxID);

	int                 m_iSerialID;
	int                 m_iBoxID;
};

class CResponseGetSerialPayAward : public CPackUserSyncData
{
public:
	CResponseGetSerialPayAward() {}

	MSGPACK_DEFINE(m_stSyncData, m_iSerialID, m_iBoxID, m_stBonusVec, m_iSerialPayAwarded);

	int                                             m_iSerialID;
	int												m_iBoxID;
	std::vector<CPackGameItem>  m_stBonusVec;
	int                                             m_iSerialPayAwarded;
};

////////////////////////////////////////////////////////////////////////////////////////
class CRequestCheckSerialPayAward
{
public:
	CRequestCheckSerialPayAward() {}

	MSGPACK_DEFINE(m_iSerialID, m_iBoxID);

	int	m_iSerialID;
	int	m_iBoxID;
};

class CResponseCheckSerialPayAward : public CPackUserSyncData
{
public:
	CResponseCheckSerialPayAward() {}

	MSGPACK_DEFINE(m_stSyncData, m_iSerialID, m_iBoxID, m_iAwardeStatus, m_bIsTodayPay, m_aiBoxStatus);

	int	m_iSerialID;
	int	m_iBoxID;
	int	m_iAwardeStatus;
	bool m_bIsTodayPay;
	std::vector<int32_t> m_aiBoxStatus;
};


//LUCKY_BOX_LOTTERY = 10248,					//幸运魔盒抽奖
class CRequestLuckyBoxLottery
{
public:
	CRequestLuckyBoxLottery() : m_iActivityID(0), m_iCount(0) {}

	MSGPACK_DEFINE(m_iActivityID, m_iCount);

	int32_t m_iActivityID;
	int32_t m_iCount;
};

class CResponseLuckyBoxLottery : public CPackUserSyncData
{
public:
	CResponseLuckyBoxLottery() : m_iActivityID(0), m_iCount(0), m_vtAwardItem(), m_stConsume() {}

	MSGPACK_DEFINE(m_stSyncData, m_iActivityID, m_iCount, m_vtAwardItem, m_stConsume);

	int32_t m_iActivityID;
	int32_t m_iCount;
	std::vector<CPackGameItem> m_vtAwardItem;
	CPackGameItem m_stConsume;
};

//TURN_TABLE_LOTTERY = 10286,					//幸运转盘抽奖
class CRequestTurnTableLottery
{
public:
	CRequestTurnTableLottery() : m_iActivityID(0), m_iCount(0) {}

	MSGPACK_DEFINE(m_iActivityID, m_iCount);

	int32_t m_iActivityID;
	int32_t m_iCount;
};

class CResponseTurnTableLottery : public CPackUserSyncData
{
public:
	CResponseTurnTableLottery() : m_iActivityID(0), m_iCount(0), m_vtAwardItem(), m_stConsume() {}

	MSGPACK_DEFINE(m_stSyncData, m_iActivityID, m_iCount, m_vtAwardItem, m_stConsume);

	int32_t m_iActivityID;
	int32_t m_iCount;
	std::vector<CPackGameItem> m_vtAwardItem;
	CPackGameItem m_stConsume;
};

//抽取限时武将
class CRequsetActivityCountdownHero
{
public:
	CRequsetActivityCountdownHero() : m_iActivityID(0), m_iTenExtract(0), m_bIsConsumeItem(false) {}

	MSGPACK_DEFINE(m_iActivityID, m_iTenExtract, m_bIsConsumeItem);

	int32_t m_iActivityID;//活动ID
	int32_t m_iTenExtract;//0表示单抽 1表示10抽
	bool m_bIsConsumeItem;  // false消耗钻石  true消耗道具
};

class CResponseActivityCountdownHero : public CPackUserSyncData
{
public:
	CResponseActivityCountdownHero() : m_iActivityID(0), m_iTenExtract(0), m_stBonusItemVec(), m_stConsume() {}

	MSGPACK_DEFINE(m_stSyncData, m_iActivityID, m_iTenExtract, m_stBonusItemVec, m_stConsume);

	int32_t m_iActivityID;//活动ID
	int32_t m_iTenExtract;//0表示单抽 1表示10抽
	std::vector<CPackGameItem> m_stBonusItemVec;
	CPackGameItem m_stConsume;
};

//限时武将排行信息
class CPackRankCountdownHero
{
public:
	CPackRankCountdownHero() : m_iGroupID(0), m_iUID(0), m_iRank(0), m_iScrore(0), m_strName("") {}

	MSGPACK_DEFINE(m_iGroupID, m_iUID, m_iRank, m_iScrore, m_strName);

	int32_t m_iGroupID;//服
	int32_t m_iUID;//uid
	int32_t m_iRank;//排行
	int32_t m_iScrore;//积分
	std::string m_strName;//名称
};

//获取限时武将排行
class CRequsetActivityCountdownHeroRank
{
public:
	CRequsetActivityCountdownHeroRank() : m_iActivityID(0) {}

	MSGPACK_DEFINE(m_iActivityID);

	int32_t m_iActivityID;//活动ID
};

class CResponseActivityCountdownHeroRank : public CPackUserSyncData
{
public:
	CResponseActivityCountdownHeroRank() : m_iActivityID(0), m_iRank(0), m_iScrore(0), m_vecRankInfo() {}

	MSGPACK_DEFINE(m_stSyncData, m_iActivityID, m_iRank, m_iScrore, m_vecRankInfo);

	int32_t m_iActivityID;//活动ID
	int32_t m_iRank;//我的排行
	int32_t m_iScrore;//积分
	std::vector<CPackRankCountdownHero> m_vecRankInfo;//排行信息
};

//招财猫历史记录
class CResponseActivityLuckyCatHistory : public CPackUserSyncData
{
public:
	CResponseActivityLuckyCatHistory() : m_listHistory() {}

	MSGPACK_DEFINE(m_stSyncData, m_listHistory);

	std::list<CPackLuckyCatHistory> m_listHistory;//历史信息
};

//NOTIFY_CANDLE_ADD = 10298,					//通知获得蜡烛(红点)

//GET_FLOWER_LANTER_LOG = 10299,				//获取花灯记录
class CPackFlowerLanternLog
{
public:
	CPackFlowerLanternLog() : m_iLogTime(0), m_iEvent(0), m_iEventValue(0), m_iCandle(0), m_iTotalValue(0), m_iTotalCandle(0) {}

	MSGPACK_DEFINE(m_iLogTime, m_iEvent, m_iEventValue, m_iCandle, m_iTotalValue, m_iTotalCandle);

	int32_t m_iLogTime;	//日志时间
	int32_t m_iEvent;	//事件  1登录 2充值
	int32_t m_iEventValue;//时间数值
	int32_t m_iCandle;	//获得蜡烛
	int32_t m_iTotalValue;	//累计数值
	int32_t m_iTotalCandle;	//累计蜡烛
};

class CRequestFlowerLanternLog
{
public:
	CRequestFlowerLanternLog() : m_iActivityID(0) {}

	MSGPACK_DEFINE(m_iActivityID);

	int32_t m_iActivityID;//活动ID
};

class CResponseFlowerLanternLog : public CPackUserSyncData
{
public:
	CResponseFlowerLanternLog() : m_iActivityID(0), m_stLogVec() {}

	MSGPACK_DEFINE(m_stSyncData, m_iActivityID, m_stLogVec);

	int32_t m_iActivityID;//活动ID
	std::vector<CPackFlowerLanternLog> m_stLogVec;//日志信息
};

////////////////////////////////////////////////////////////////////////////////////////////////
//合服
class CPackMergeActivityInfo
{
public:
	CPackMergeActivityInfo() : m_iTaskID(0), m_iProgress(0), m_byAward(0) {}

	MSGPACK_DEFINE(m_iTaskID, m_iProgress, m_byAward);

	int32_t m_iTaskID;		//任务ID
	int32_t m_iProgress;	//任务进度或购买进度
	int8_t m_byAward;		//任务奖励状态
};

//NOTIFY_MERGE_ACTIVITY_INFO = 10371,		//通知合服活动数据
class CNotifyMergeActivityInfo : public CPackUserSyncData
{
public:
	CNotifyMergeActivityInfo() : m_stInfoVec() {}

	MSGPACK_DEFINE(m_stSyncData, m_stInfoVec);

	std::vector<CPackMergeActivityInfo> m_stInfoVec;
};

//NOTIFY_MERGE_ACTIVITY_CHANGE = 10372,		//通知合服活动数据变更
class CNotifyMergeActivityChange : public CPackUserSyncData
{
public:
	CNotifyMergeActivityChange() : m_stInfo() {}

	MSGPACK_DEFINE(m_stSyncData, m_stInfo);

	CPackMergeActivityInfo m_stInfo;
};

//MERGE_ACTIVITY_TASK_AWARD = 10373,	//获取合服活动任务奖励
class CRequestMergeActivityTaskAward
{
public:
	CRequestMergeActivityTaskAward() : m_iTaskID(0) {}

	MSGPACK_DEFINE(m_iTaskID);

	int32_t m_iTaskID;
};

class CResponseMergeActivityTaskAward : public CPackUserSyncData
{
public:
	CResponseMergeActivityTaskAward() : m_iTaskID(0), m_stBonusVec() {}

	MSGPACK_DEFINE(m_stSyncData, m_iTaskID, m_stBonusVec);

	int32_t m_iTaskID;
	std::vector<CPackGameItem> m_stBonusVec;
};

//MERGE_ACTIVITY_EXCHANGE = 10374,		//购买合服活动兑换物品
class CRequestMergeActivityExchange
{
public:
	CRequestMergeActivityExchange() : m_iTaskID(0) {}

	MSGPACK_DEFINE(m_iTaskID);

	int32_t m_iTaskID;
};

class CResponseMergeActivityExchange : public CPackUserSyncData
{
public:
	CResponseMergeActivityExchange() : m_iTaskID(0), m_stConsumeVec(), m_stBonusVec() {}

	MSGPACK_DEFINE(m_stSyncData, m_iTaskID, m_stConsumeVec, m_stBonusVec);

	int32_t m_iTaskID;
	std::vector<CPackGameItem> m_stConsumeVec;
	std::vector<CPackGameItem> m_stBonusVec;
};

//FIGHT_MERGE_ACTIVITY = 10375,				//合服活动副本开始战斗
class CRequestFightMergeActivity
{
public:
	CRequestFightMergeActivity() : m_iLevelID(0) {}

	MSGPACK_DEFINE(m_iLevelID);

	int32_t m_iLevelID;
};

class CResponseFightMergeActivity : public CPackUserSyncData
{
public:
	CResponseFightMergeActivity() : m_iLevelID(0), m_stDropVec() {}

	MSGPACK_DEFINE(m_stSyncData, m_iLevelID, m_stDropVec);

	int32_t m_iLevelID;
	std::vector<CPackGameItem> m_stDropVec;
};

//FIGHT_MERGE_ACTIVITY_RESULT = 10376,		//合服活动副本结束战斗
class CRequestFightMergeActivityResult
{
public:
	CRequestFightMergeActivityResult() : m_iLevelID(0), m_iGetCoinNum(0) {}

	MSGPACK_DEFINE(m_iLevelID, m_iGetCoinNum, m_stDropVec);

	int32_t m_iLevelID;
	int32_t m_iGetCoinNum;
	std::vector<CPackGameItem> m_stDropVec;
};

class CResponseFightMergeActivityResult : public CPackUserSyncData
{
public:
	CResponseFightMergeActivityResult() : m_iLevelID(0), m_iGetCoinNum(0), m_stDropVec(), m_stPerfectBonusList() {}

	MSGPACK_DEFINE(m_stSyncData, m_iLevelID, m_iGetCoinNum, m_stDropVec, m_stPerfectBonusList);

	int32_t m_iLevelID;
	int32_t m_iGetCoinNum;
	std::vector<CPackGameItem> m_stDropVec;
	std::vector<CPackGameItem> m_stPerfectBonusList;
};

////////////////////////////////////////////////////////////////////////////////////////////////
//每日单笔
class CPackDaySingleRechargeTask
{
public:
	MSGPACK_DEFINE(m_iActivityID, m_iTaskID, m_iRewardCount, m_iCanRewardCount);

	int32_t m_iActivityID;
	int32_t m_iTaskID;
	int32_t m_iRewardCount;
	int32_t m_iCanRewardCount;
};

//NOTIFY_DAY_SINGLE_RECHARGE_INFO = 10846,		//信息通知
class CNotifyDaySingleRecharge : public CPackUserSyncData
{
public:
	CNotifyDaySingleRecharge() : m_stTaskVec(){}

	MSGPACK_DEFINE(m_stSyncData, m_stTaskVec);

	std::vector<CPackDaySingleRechargeTask>			m_stTaskVec;
};

//NOTIFY_DAY_SINGLE_RECHARGE_TASK_CHANGE = 10847,	//单个任务信息修改通知
class CNotifyDaySingleRechargeTaskChange : public CPackUserSyncData
{
public:
	CNotifyDaySingleRechargeTaskChange() : m_stTask(){}

	MSGPACK_DEFINE(m_stSyncData, m_stTask);

	CPackDaySingleRechargeTask				m_stTask;
};

class CPackUserBuff
{
public:
    CPackUserBuff() : m_iEffectIndex(0), m_iBuffType(0), m_iBuffPara(0), m_iBuffParaEx(0), m_iEndTime(0), m_iEffectTimes(0) {}

    MSGPACK_DEFINE(m_iEffectIndex, m_iBuffType, m_iBuffPara, m_iBuffParaEx, m_iEndTime, m_iEffectTimes);

    int                     m_iEffectIndex;
    int                     m_iBuffType;
    int                     m_iBuffPara;
    int                     m_iBuffParaEx;
    int                     m_iEndTime;
    int                     m_iEffectTimes;
};

class CNotifyUserBuffData : public CPackUserSyncData
{
public:
    CNotifyUserBuffData() : m_stBuffMap() {}

    MSGPACK_DEFINE(m_stSyncData, m_stBuffMap);

    std::vector<CPackUserBuff>    m_stBuffMap;
};

class CPackQuestionnaireProblemElem
{
public:
    CPackQuestionnaireProblemElem() : m_iType(0), m_iOtherIndex(0), m_iMaxChoice(0), m_strTitle(""), m_stAnswerList(), m_bNotMustAnswer(false) {}

    MSGPACK_DEFINE(m_iType, m_iOtherIndex, m_iMaxChoice, m_strTitle, m_stAnswerList, m_bNotMustAnswer);

    int32_t     m_iType;                // 题目类型
    int32_t     m_iOtherIndex;          // "其他"选项索引
    int32_t     m_iMaxChoice;           // 多选题最多选项个数
    std::string m_strTitle;             // 题目
    std::vector<std::string> m_stAnswerList;    // 选项列表
    bool        m_bNotMustAnswer;       // true 非必答题
};

class CPackQuestionnaireElem
{
public:
    CPackQuestionnaireElem() : m_iID(0),m_strName(),m_strContent(), m_iDurationTime(0), m_stRewards(), m_stProblemList() {}

    MSGPACK_DEFINE(m_iID,m_strName,m_strContent, m_iDurationTime,m_stRewards, m_stProblemList);

    int32_t     m_iID;                      // 问卷ID
    std::string m_strName;                  // 问卷名
    std::string m_strContent;               // 问卷标题内容
    int32_t     m_iDurationTime;            // 失效时间
    std::vector<CPackGameItem> m_stRewards; // 奖励
    std::vector<CPackQuestionnaireProblemElem> m_stProblemList;    // 题目列表
};

class CNotifyQuestionnaire : public CPackUserSyncData
{
public:
    CNotifyQuestionnaire() : m_stQuestionnaireList() {}

    MSGPACK_DEFINE(m_stSyncData, m_stQuestionnaireList);

    std::vector<CPackQuestionnaireElem> m_stQuestionnaireList;
};

//GET_DAY_SINGLE_RECHARGE_REWARD = 10848,		//领取奖励请求
class CRequestGetDaySingleRechargeReward
{
public:
	MSGPACK_DEFINE(m_iActivityID, m_iTaskID);

	int32_t m_iActivityID = 0;
	int32_t m_iTaskID = 0;
};

class CResponseGetDaySingleRechargeReward : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_iActivityID, m_iTaskID, m_stBonusItemVec);

	int32_t m_iActivityID = 0;
	int32_t m_iTaskID = 0;
	std::vector<CPackGameItem> m_stBonusItemVec;
};

class CResponseGetWorldQuestInfo : public CPackUserSyncData
{
public:
    CResponseGetWorldQuestInfo() : m_stQuestInfo() {}

    MSGPACK_DEFINE(m_stSyncData, m_stQuestInfo);

    std::map<int32_t, int32_t> m_stQuestInfo;       // [任务ID， 进度]
};

class CRequestGetAvyReward
{
public:
    CRequestGetAvyReward() : m_iAvyID(0), m_iIndex(0) {}

    MSGPACK_DEFINE(m_iAvyID, m_iIndex);

    int32_t         m_iAvyID;           // 活动ID
    int32_t         m_iIndex;           // 索引
};

class CResponseGetAvyReward : public CPackUserSyncData
{
public:
    CResponseGetAvyReward() : m_stItemData(), m_stAvyInfo() {}

    MSGPACK_DEFINE(m_stSyncData, m_stItemData, m_stAvyInfo);

    CPackTotalItemData  m_stItemData;
    CPackAvyInfo        m_stAvyInfo;
};
