#pragma once

#include "msgpack.hpp"

#include "server_base_protocol.h"
#include "server_common_protocol.h"
#include "server_friend_protocol.h"

class CPackLimitBossInfo
{
  public:
    CPackLimitBossInfo() : m_iOwnerUin(0), m_iOwnerGroupID(0), m_strNick(), m_cHard(0), m_cIsOpen(0), m_iEndTime(0),
          m_iOpenTime(0), m_iDeathTime(0), m_ulMaxHp(0), m_ulCurHurt(0), m_uMyMaxHurt(0), m_ulMyTotalHurt(0) {}

    MSGPACK_DEFINE(m_iOwnerUin, m_iOwnerGroupID, m_strNick, m_cHard, m_cIsOpen, m_iEndTime, m_iOpenTime, m_iDeathTime,
                   m_ulMaxHp, m_ulCurHurt, m_uMyMaxHurt, m_ulMyTotalHurt);

    int32_t     m_iOwnerUin;            // 发现者
    int32_t     m_iOwnerGroupID;
    std::string m_strNick;
    char        m_cHard;                // 难度
    char        m_cIsOpen;              // 是否已公开
    int32_t     m_iEndTime;             // BOSS消失时间
    int32_t     m_iOpenTime;            // BOSS自动公开时间
    int32_t     m_iDeathTime;           // BOSS死亡判定倒计时
    uint64_t    m_ulMaxHp;              // BOSS最大血量
    uint64_t    m_ulCurHurt;            // BOSS已损失血量
    uint32_t    m_uMyMaxHurt;           // 玩家最大伤害记录
    uint64_t    m_ulMyTotalHurt;        // 玩家累计伤害
};

class CPackEntireWarInfo
{
public:
	MSGPACK_DEFINE(m_iActionCoin, m_iAddActionTime, m_iWarLevel, m_strAwardBitmap, m_iWarContribution, m_iWeekWarContribution
                 , m_stBuffVec, m_iThemeID, m_iSimpleGameDailyTimes, m_iLimitBossAction, m_iLimitBoxTime);

	int32_t m_iActionCoin = 0;
	int32_t m_iAddActionTime = 0;
	int32_t m_iWarLevel = 0;
	std::string m_strAwardBitmap; 
	int32_t m_iWarContribution;
	int32_t m_iWeekWarContribution;
	std::vector<CPackEntireWarBuff> m_stBuffVec;
    int32_t m_iThemeID = 0;
    int32_t m_iSimpleGameDailyTimes = 0;
    int32_t m_iLimitBossAction = 0;
    int32_t m_iLimitBoxTime = 0;
};

class CPackEntireWarMap
{
public:
	MSGPACK_DEFINE(m_iWarID, m_ulTargetProgress, m_ulCurProgress, m_bIsAward);

	int32_t m_iWarID = 0;
	uint64_t m_ulTargetProgress = 0;
	uint64_t m_ulCurProgress = 0;
	bool m_bIsAward = false;
};

//GET_ENTIRE_WAR_INFO = 10400,		//获取全民战役信息
class CResponseGetEntireWarInfo : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_stInfo, m_astMapVec, m_stLimitBoss);

	CPackEntireWarInfo m_stInfo;
	std::vector<CPackEntireWarMap> m_astMapVec;
    CPackLimitBossInfo m_stLimitBoss;
};

//FIGHT_ENTIRE_WAR_START = 10401,		//开始全民战役
class CRequestFightEntireWarStart
{
public:
	MSGPACK_DEFINE(m_iWarID, m_iCardIDVec);

	int32_t m_iWarID = 0;
	CPackCardTeam m_iCardIDVec;
};

class CResponseFightEntireWarStart : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_iWarID, m_iLevelID, m_ulTargetProgress, m_ulCurProgress, m_iCardIDVec);

	int32_t m_iWarID = 0;
	int32_t m_iLevelID = 0;
	uint64_t m_ulTargetProgress = 0;
	uint64_t m_ulCurProgress = 0;
	std::vector<int32_t> m_iCardIDVec;
};

//FIGHT_ENTIRE_WAR_RESULT = 10402,	//结束全民战役
class CRequestFightEntireWarResult
{
public:
	MSGPACK_DEFINE(m_iWarID, m_iLevelID, m_iHurtBossHp);

	int32_t m_iWarID = 0;
	int32_t m_iLevelID = 0;
	int32_t m_iHurtBossHp = 0;
};

class CResponseFightEntireWarResult : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_iWarID, m_iLevelID, m_stBonusItemVec, m_stWarInfo, m_stMapInfo);

	int32_t m_iWarID = 0;
	int32_t m_iLevelID = 0;
	std::vector<CPackGameItem> m_stBonusItemVec;
	CPackEntireWarInfo m_stWarInfo;
	CPackEntireWarMap m_stMapInfo;
};

//SWEEP_ENTIRE_WAR_CREEPS = 10417,	//扫荡野怪关卡
class CRequestSweepEntireWar
{
public:
	MSGPACK_DEFINE(m_iWarID, m_iCardIDVec);

	int32_t m_iWarID = 0;
	std::vector<int32_t> m_iCardIDVec;
	char	m_cSweepMode;		//1-扫荡一次，2-扫荡全部
};

//GET_ENTIRE_WAR_MARKET = 10403,		//拉取全民战役集市
class CRequestGetEntireWarMarket
{
public:
	MSGPACK_DEFINE(m_iWarID);

	int32_t m_iWarID = 0;
};

class CResponseGetEntireWarMarket : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_iWarID, m_stMarketVec);

	int32_t m_iWarID = 0;
	std::vector<CPackMarket> m_stMarketVec;
};

//BUY_ENTIRE_WAR_MARKET = 10404,		//购买全民战役集市
class CRequestBuyEntireWarMarket
{
public:
	MSGPACK_DEFINE(m_iWarID, m_iMarketIndex);

	int32_t m_iWarID = 0;
	int32_t m_iMarketIndex = 0;
};

class CResponseBuyEntireWarMarket : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_iWarID, m_iMarketIndex, m_stCommodity, m_stConsume);

	int32_t m_iWarID = 0;
	int32_t m_iMarketIndex = 0;
	CPackGameItem   m_stCommodity;
	CPackGameItem   m_stConsume;
};

//DIG_ENTIRE_WAR_TREASURE = 10406,	//挖去全民战役宝箱
class CRequestDigEntireWarTreasure
{
public:
	MSGPACK_DEFINE(m_iWarID);

	int32_t m_iWarID = 0;
};

class CResponseDigEntireWarTreasure : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_iWarID, m_stBonusItems, m_iDigIndex);

	int32_t m_iWarID = 0;
	std::vector<CPackGameItem>	m_stBonusItems;
	int32_t m_iDigIndex = 0;	
};

//GET_ENTIRE_WAR_QUESTION = 10407,	//获取全民战役题目
class CRequestGetEntireWarQuestion
{
public:
	MSGPACK_DEFINE(m_iWarID);

	int32_t m_iWarID = 0;
};

class CResponseGetEntireWarQuestion : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_iWarID, m_iQuestionID);

	int32_t m_iWarID = 0;
	int32_t m_iQuestionID = 0;
};

//ANSWER_ENTIRE_WAR_QUESTION = 10409,	//回答全民战役题目
class CRequestAnswerEntireWarQuestion
{
public:
	MSGPACK_DEFINE(m_iWarID);

	int32_t m_iWarID = 0;
};

class CResponseAnswerEntireWarQuestion : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_iWarID, m_stBonusItemVec, m_stWarInfo, m_stMapInfo);

	int32_t m_iWarID = 0;
	std::vector<CPackGameItem> m_stBonusItemVec;
	CPackEntireWarInfo m_stWarInfo;
	CPackEntireWarMap m_stMapInfo;
};

//GET_ENTIRE_WAR_MAP_AWARD = 10410,	//获取全民战役地图奖励	
class CRequestGetEntireWarMapAward
{
public:
	MSGPACK_DEFINE(m_iWarID);

	int32_t m_iWarID = 0;
};

class CResponseGetEntireWarMapAward : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_iWarID, m_iAddActionCoin, m_stBonusItemVec, m_stWarInfo, m_stMapInfo);

	int32_t m_iWarID = 0;
	int32_t m_iAddActionCoin = 0;
	std::vector<CPackGameItem> m_stBonusItemVec;
	CPackEntireWarInfo m_stWarInfo;
	CPackEntireWarMap m_stMapInfo;
};

//UPGRADE_ENTIRE_MILITARK_LEVEL = 10411,	//升级战役军衔等级
class CResponseUpgradeEntireMilitarkLevel : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_iMilitarkLevel, m_iEntireWarContribution);

	int32_t m_iMilitarkLevel;
	int32_t m_iEntireWarContribution;
};

//GET_ENTIRE_WAR_FINAL_AWARD = 10412,		//获取全民战役宝箱奖励
class CRequestGetEntireWarFinalAward
{
public:
	MSGPACK_DEFINE(m_iFinalID);

	int32_t m_iFinalID = 0;
};

class CResponseGetEntireWarFinalAward : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_iFinalID, m_strEntireWarAwardBitmap, m_stBonusItemVec);

	int32_t m_iFinalID = 0;
	std::string m_strEntireWarAwardBitmap;
	std::vector<CPackGameItem> m_stBonusItemVec;
};

//GET_ENTIRE_WAR_RANK = 10413,		//获取全民战役排行
class CResponseGetEntireWarRank : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_stWarGod, m_iTotalWorshippedCount, m_iTodayWorshippedCount, m_iSelfWeeklyContribution, m_iSelfRank, m_stRank);

	CPackUserBrief m_stWarGod;
	int32_t m_iTotalWorshippedCount = 0;
	int32_t m_iTodayWorshippedCount = 0;
	int32_t m_iSelfWeeklyContribution = 0;
	int32_t m_iSelfRank = 0;
	std::vector<CPackUserRank> m_stRank;
};          

//WORSHIP_ENTIRE_WAR_GOD =10414,		//膜拜战神
class CResponseWorshipEntireWarGod : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_iTotalWorshippedCount, m_iTodayWorshippedCount, m_stBonusItemVec);

	int32_t m_iTotalWorshippedCount = 0;
	int32_t m_iTodayWorshippedCount = 0;
	std::vector<CPackGameItem> m_stBonusItemVec;
};

//BUY_ACTION_COIN = 10415,			//购买行动力
class CResponseBuyActionCoin : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_iActionCoin, m_iAddActionTime);

	int32_t m_iActionCoin = 0;
	int32_t m_iAddActionTime = 0;
};

class CRequestEntireWarSimpleGame
{
  public:
    CRequestEntireWarSimpleGame() : m_iWarID(0), m_iScore(0) {}

    MSGPACK_DEFINE(m_iWarID, m_iScore);

    int32_t     m_iWarID;
    int32_t     m_iScore;
};

class CResponseEntireWarSimpleGame : public CPackUserSyncData
{
  public:
    CResponseEntireWarSimpleGame() : m_iWarID(0), m_iScore(0), m_iSimpleGameDailyTimes(0), m_iMaxScore(0) {}

    MSGPACK_DEFINE(m_stSyncData, m_iWarID, m_iScore, m_iSimpleGameDailyTimes, m_iMaxScore);

    int32_t     m_iWarID;
    int32_t     m_iScore;
    int32_t     m_iSimpleGameDailyTimes;
    int32_t     m_iMaxScore;
};

class CNotifyNewLimitBoss : public CPackUserSyncData
{
  public:
    CNotifyNewLimitBoss() : m_stBossInfo() {}

    MSGPACK_DEFINE(m_stSyncData, m_stBossInfo);

    CPackLimitBossInfo m_stBossInfo;
};

class CNotifyNewLimitBox : public CPackUserSyncData
{
  public:
    CNotifyNewLimitBox() : m_iLimitBoxTime(0) {}

    MSGPACK_DEFINE(m_stSyncData, m_iLimitBoxTime);

    int32_t     m_iLimitBoxTime;
};

class CRequestLimitBossLevelBegin
{
  public:
    CRequestLimitBossLevelBegin() : m_iOwnerUin(0), m_iLevelID(0) {}

    MSGPACK_DEFINE(m_iOwnerUin, m_iLevelID);

    int32_t     m_iOwnerUin;
    int32_t     m_iLevelID;
};

class CResponseLimitBossLevelBegin : public CPackUserSyncData
{
  public:
    CResponseLimitBossLevelBegin() : m_iOwnerUin(0), m_iLevelID(0), m_ulMaxHp(0), m_ulCurHurt(0) {}

    MSGPACK_DEFINE(m_stSyncData, m_iOwnerUin, m_iLevelID, m_ulMaxHp, m_ulCurHurt);

    int32_t     m_iOwnerUin;
    int32_t     m_iLevelID;
    uint64_t    m_ulMaxHp;
    uint64_t    m_ulCurHurt;
};

class CRequestLimitBossLevelResult
{
  public:
    CRequestLimitBossLevelResult() : m_iOwnerUin(0), m_iLevelID(0), m_iHurt(0) {}

    MSGPACK_DEFINE(m_iOwnerUin, m_iLevelID, m_iHurt);

    int32_t     m_iOwnerUin;
    int32_t     m_iLevelID;
    int32_t     m_iHurt;
};

class CResponseLimitBossLevelResult : public CPackUserSyncData
{
  public:
    CResponseLimitBossLevelResult() : m_stBossInfo(), m_stReward(), m_iScore(0), m_iHurt(0) {}

    MSGPACK_DEFINE(m_stSyncData, m_stBossInfo, m_stReward, m_iScore, m_iHurt);

    CPackLimitBossInfo  m_stBossInfo;
    std::vector<CPackGameItem> m_stReward;
    int32_t             m_iScore;
    int32_t             m_iHurt;
};

class CRequestLimitBossInvite
{
  public:
    CRequestLimitBossInvite() : m_bOpen(0), m_stInviteList() {}

    MSGPACK_DEFINE(m_bOpen, m_stInviteList);

    bool m_bOpen;
    std::map<int32_t, int32_t> m_stInviteList;          // [uin, group]
};

class CResponseLimitBossOpenInviteList : public CPackUserSyncData
{
  public:
    CResponseLimitBossOpenInviteList() : m_stInviteBossList() {}

    MSGPACK_DEFINE(m_stSyncData, m_stInviteBossList);

    std::vector<CPackLimitBossInfo> m_stInviteBossList;
};

class CRequestLimitBossAccept
{
  public:
    CRequestLimitBossAccept() : m_iAcceptType(0), m_iOwnerUin(0) {}

    MSGPACK_DEFINE(m_iAcceptType, m_iOwnerUin);

    int32_t     m_iAcceptType;      // 0 拒绝, 1 接受, 2 一键拒绝, 3 一键接受
    int32_t     m_iOwnerUin;
};

class CResponseLimitBossAccept : public CPackUserSyncData
{
  public:
    CResponseLimitBossAccept() : m_iAcceptType(0), m_iOwnerUin(0) {}

    MSGPACK_DEFINE(m_stSyncData, m_iAcceptType, m_iOwnerUin);

    int32_t     m_iAcceptType;      // 0 拒绝, 1 接受, 2 一键拒绝, 3 一键接受
    int32_t     m_iOwnerUin;
};

class CResponseLimitBossOpenBossList : public CPackUserSyncData
{
  public:
    CResponseLimitBossOpenBossList() : m_stMyBossInfo(), m_stBossList(), m_stRandBossList() {}

    MSGPACK_DEFINE(m_stSyncData, m_stMyBossInfo, m_stBossList, m_stRandBossList);

    CPackLimitBossInfo m_stMyBossInfo;
    std::vector<CPackLimitBossInfo> m_stBossList;
    std::vector<CPackLimitBossInfo> m_stRandBossList;
};

class CResponseLimitBossGetBox : public CPackUserSyncData
{
  public:
    CResponseLimitBossGetBox() : m_stReward() {}

    MSGPACK_DEFINE(m_stSyncData, m_stReward);

    std::vector<CPackGameItem> m_stReward;
};

class CRequestLimitBossOpenFriendList
{
  public:
    CRequestLimitBossOpenFriendList() : m_bFriend(false) {}

    MSGPACK_DEFINE(m_bFriend);

    bool m_bFriend;         // true: 好友   false: 公会
};

class CResponseLimitBossOpenFirendList : public CPackUserSyncData
{
  public:
    CResponseLimitBossOpenFirendList() : m_bFriend(false), m_stFriendList() {}

    MSGPACK_DEFINE(m_stSyncData, m_bFriend, m_stFriendList);

    bool m_bFriend;
    std::vector<CPackFriendInfo> m_stFriendList;
};

class CResponseLimitBossRefreshOpenBoss : public CPackUserSyncData
{
  public:
    CResponseLimitBossRefreshOpenBoss() : m_stRandBossList() {}

    MSGPACK_DEFINE(m_stSyncData, m_stRandBossList);

    std::vector<CPackLimitBossInfo> m_stRandBossList;
};
