#pragma once

#include "msgpack.hpp"

#include "server_base_protocol.h"
#include "server_friend_protocol.h"
#include "server_common_protocol.h"

class CPackAttackDefenseArea
{
public:
    MSGPACK_DEFINE(m_iUin, m_nGroupID, m_iAreaType, m_uiFightBitmap, m_stPlayer, m_strFightNameVec);

    int32_t m_iUin;
    int16_t m_nGroupID;
    int32_t m_iAreaType = 0;
    uint32_t m_uiFightBitmap = 0;
    CPackPvpChallengerInfo m_stPlayer;
    std::vector<std::string> m_strFightNameVec;
};

class CPackAttackDefense
{
public:
    MSGPACK_DEFINE(m_iGuildID, m_iGuildExp, m_iGuildLevel, m_strGuildName, m_iServerID,
                   m_iOpenDay, m_iDstGuildID, m_iLoseAreaNum, m_iActiveMemberCount,
                   m_iAtkDefScore, m_stAtkDefMatchHistory, m_stAreaMap);

    int32_t m_iGuildID = 0;
    int32_t m_iGuildExp = 0;
    int32_t m_iGuildLevel = 0;
    std::string m_strGuildName;
    int32_t m_iServerID = 0;
    int32_t m_iOpenDay = 0;
    int32_t m_iDstGuildID = 0;	//对手公会ID
    int32_t m_iLoseAreaNum = 0;	//丢失的据点数量
    int32_t m_iActiveMemberCount = 0;	//活跃成员数
    int32_t m_iAtkDefScore = 0;	//攻防分数
    std::set<int32_t> m_stAtkDefMatchHistory;
    std::map<int32_t, CPackAttackDefenseArea> m_stAreaMap;
};

class CPackGuildWar
{
public:
    MSGPACK_DEFINE(m_bAttackDefenseFighting, m_stAttackDefenseList, m_stAttackDefenseTiBu);

    bool m_bAttackDefenseFighting;
    std::list<CPackAttackDefense> m_stAttackDefenseList;
    CPackAttackDefense m_stAttackDefenseTiBu;
};

class CPackMatchPlayer
{
public:
    MSGPACK_DEFINE(m_iUin, m_nGroupID, m_iFightPower);

    int32_t m_iUin;
    int16_t m_nGroupID;
    int32_t m_iFightPower;
};

struct CPackFairPlayer
{
public:
    MSGPACK_DEFINE(m_iUin, m_nGroupID, m_iCardVec);

    int32_t m_iUin;
    int16_t m_nGroupID;
    std::vector<int32_t> m_iCardVec;
};

class CPackLuckyHammerRank
{
public:
    MSGPACK_DEFINE(m_iUid, m_iGroupID, m_strNick, m_iScore);

    int32_t m_iUid = 0;
    int32_t m_iGroupID = 0;
    std::string m_strNick;
    int32_t m_iScore = 0;
};

class CPackLuckyHammerLog
{
public:
    MSGPACK_DEFINE(m_iLogID, m_strContent, m_bLucky, m_iLuckyUin);

    int32_t m_iLogID = 0;
    std::string m_strContent;
    bool m_bLucky = false;
    std::set<int32_t> m_iLuckyUin;
};

class CPackLuckyHammer
{
public:
    MSGPACK_DEFINE(m_iStartTime, m_iEndTime, m_stRank, m_stLog);

    int32_t m_iStartTime = 0;
    int32_t m_iEndTime = 0;
    std::vector<CPackLuckyHammerRank> m_stRank;
    std::list<CPackLuckyHammerLog> m_stLog;
};

class CPackMatchRecovery
{
public:
    MSGPACK_DEFINE(m_stGuildWar, m_stMatchPvpPlayer, m_stFairPvpPlayer, m_stLuckyHammer)

    CPackGuildWar m_stGuildWar;
    std::vector<std::list<CPackMatchPlayer>> m_stMatchPvpPlayer;
    std::vector<CPackFairPlayer> m_stFairPvpPlayer;
    std::map<std::pair<int32_t, int32_t>, CPackLuckyHammer> m_stLuckyHammer;

};

class CPackMatckPvp
{
public:
	MSGPACK_DEFINE(m_iMatchScore, m_iDailyCount, m_iMaxLevel, m_iWinCount, m_iLoseCount, m_strMaxBonusBitmap, m_iStoreBuyCountMap, m_iShowGroupID, m_strGroupName, m_iMaxScore);

	int32_t m_iMatchScore = 0;			//匹配积分
	int32_t m_iDailyCount = 0;			//今日次数
	int32_t m_iMaxLevel = 0;			//最大段位
	int32_t m_iWinCount = 0;			//胜利次数
	int32_t m_iLoseCount = 0;			//失败次数
	std::string m_strMaxBonusBitmap;	//最大段位奖励状态
	std::map<int32_t, int32_t> m_iStoreBuyCountMap;	//商店购买次数
	int32_t m_iShowGroupID = 0;
	std::string m_strGroupName;
	int32_t m_iMaxScore = 0;			//最大积分
};

//UPDATE_MATCH_USER = 12001,				//更新匹配玩家数据[logic->cross]
class CSRequestUpdateMatchUser
{
public:
	MSGPACK_DEFINE(m_stUserVec);

	std::vector<CPackUserMatch> m_stUserVec;
};

//NOTIFY_MATCH_PVP_INFO = 12002,			//通知巅峰对决信息[logic->client]
class CNotifyMatchPvpInfo : public CPackUserSyncData
{
public:
	CNotifyMatchPvpInfo() : m_stPvpInfo() {}

	MSGPACK_DEFINE(m_stSyncData, m_stPvpInfo);

	CPackMatckPvp m_stPvpInfo;
};

//FIGHT_MATCH_PVP = 12003,				//开始巅峰对决[client->logic->cross|logic->cross->logic->client]
class CSRequestFightMatchPvp
{
public:
	MSGPACK_DEFINE(m_iMatchLevel, m_iMaxPvpFightPower, m_iHadMatchUin, m_iBaodiRobotDelta);

	int32_t m_iMatchLevel;
	int32_t m_iMaxPvpFightPower;
	std::list<int32_t> m_iHadMatchUin;
	int32_t m_iBaodiRobotDelta;
};

class CResponseFightMatchPvp : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_stChallengerPlayer);

	CPackPvpChallengerInfo m_stChallengerPlayer;
};

//FIGHT_MATCH_PVP_RESULT = 12004,			//结束巅峰对决[client->logic]
class CRequestFightMatchPvpResult
{
public:
	MSGPACK_DEFINE(m_bResult);

	bool m_bResult = false;
};

class CResponseFightMatchPvpResult : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_bResult, m_iDeltaMatchScore);

	bool m_bResult = false;
	int32_t m_iDeltaMatchScore = 0;
};

//NOTIFY_BE_MATCHED = 12005,				//通知被匹配上[cross->logic]

//BUY_MATCH_STORE_ITEM = 12006,			//购买巅峰对决商店物品[client->logic]
class CRequestBuyMatchStoreItem
{
public:
	MSGPACK_DEFINE(m_iStoreID);

	int32_t m_iStoreID = 0;
};

class CResponseBuyMatchStoreItem : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_stBonusItem, m_stConsumeItem);

	CPackGameItem m_stBonusItem;
	CPackGameItem m_stConsumeItem;
};

//GET_MATCH_MAX_AWARD = 12007,			//获取巅峰对决最高段位奖励[client->logic]
class CRequestGetMatchMaxAward
{
public:
	MSGPACK_DEFINE(m_iIndex);

	int32_t m_iIndex = 0;
};

class CResponseGetMatchMaxAward : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_iIndex, m_stBonusVec);

	int32_t m_iIndex = 0;
	std::vector<CPackGameItem> m_stBonusVec;
};

//NOTIFY_CROSS_BROADCAST = 12008,			//通知跨服公告[cross->logic]
class CSNotifyBroadcast
{
public:
	MSGPACK_DEFINE(m_strBroadcastID, m_stParamMap);

	std::string m_strBroadcastID;
	std::map<std::string, std::string> m_stParamMap;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////
////公平竞技
class CPackFairPvp
{
public:
	MSGPACK_DEFINE(m_iWinCount, m_iLoseCount, m_strBonusBitmap, m_iStoreCardVec, m_iTeamCardVec);

	int32_t m_iWinCount = 0;
	int32_t m_iLoseCount = 0;
	std::string m_strBonusBitmap;
	std::vector<int32_t> m_iStoreCardVec;
	std::vector<int32_t> m_iTeamCardVec;
};

//NOTIFY_FAIR_PVP_INFO = 12010,			//通知公平竞技信息[logic->client]
class CNotifyFairPvpInfo : public CPackUserSyncData
{
public:
	CNotifyFairPvpInfo() : m_stPvpInfo() {}

	MSGPACK_DEFINE(m_stSyncData, m_stPvpInfo);

	CPackFairPvp m_stPvpInfo;
};

class CRequestCrossUserInfo
{
public:
    MSGPACK_DEFINE(m_iDstUin, m_iDstGroup);

    int32_t m_iDstUin;
    int32_t m_iDstGroup;
};

//REFRESH_FAIR_PVP_CARD = 12011,		//刷新公平竞技武将列表[client->logic]

//SELECT_FAIR_PVP_TEAM = 12012,			//选择公平竞技队伍[client->logic]
class CRequestSelectFairPvpTeam
{
public:
	MSGPACK_DEFINE(m_iTeamVec, m_iDstUin, m_iDstGroup);

	std::vector<int32_t> m_iTeamVec;
    int32_t m_iDstUin;
    int32_t m_iDstGroup;
};

class CResponseSelectFairPvpTeam : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_iTeamVec);

	std::vector<int32_t> m_iTeamVec;
};

//FIGHT_FAIR_PVP = 12013,				//开始公平竞技[client->logic->cross|logic->cross->logic->client]
class CRequestFightFairPvp
{
public:
	MSGPACK_DEFINE(m_iTeamVec);

	std::vector<int32_t> m_iTeamVec;
};

class CSRequestFightFairPvp
{
public:
	MSGPACK_DEFINE(m_iTeamVec, m_iNotCardVec);

	std::vector<int32_t> m_iTeamVec;
	std::vector<int32_t> m_iNotCardVec;
};

class CResponseFightFairPvp : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_stChallengerPlayer);

	CPackPvpChallengerInfo m_stChallengerPlayer;
};

//FIGHT_FAIR_PVP_RESULT = 12014,		//结束公平竞技[client->logic]
class CRequestFightFairPvpResult
{
public:
	MSGPACK_DEFINE(m_bResult);

	bool m_bResult = false;
};

class CResponseFightFairPvpResult : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_bResult);

	bool m_bResult = false;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
////公会战：据点攻防
class CPackGuildAttackDefenseArea
{
public:
	MSGPACK_DEFINE(m_iAreaType, m_strAreaFightBitmap, m_stChallengerPlayer, m_strFightNameVec);

	int32_t m_iAreaType = 0;			//据点类型
	std::string m_strAreaFightBitmap;	//据点占领位图
	CPackPvpChallengerInfo m_stChallengerPlayer;
	std::vector<std::string> m_strFightNameVec;
};

//GET_ATTACK_DEFENSE_INFO = 12020,		//拉取据点攻防信息[client->logic->cross->logic->client]
class CSRequestGetAttackDefenseInfo
{
public:
	MSGPACK_DEFINE(m_iGuildID);

	int32_t m_iGuildID;
};

class CResponseGetAttackDefenseInfo : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_iArmyPoint, m_iLastAddTime, m_iCanFightTime, m_iSelfFightStar, m_stSelfAreaVec
		, m_iDstShowGroupID, m_strDstGroupName, m_iDstGuildID, m_strDstGuildName, m_iDstFightStar, m_stDstAreaVec);

	int32_t m_iArmyPoint = 0;					//军令
	int32_t m_iLastAddTime = 0;					//上次加军令时间
	int32_t m_iCanFightTime = 0;				//可以参与战斗时间
	int32_t m_iSelfFightStar = 0;				//自己这方获得星星数
	std::vector<CPackGuildAttackDefenseArea> m_stSelfAreaVec;	//自己这方的据点
	int32_t m_iDstShowGroupID = 0;				//对战方服务器
	std::string m_strDstGroupName;				//对战方服务器名
	int32_t	m_iDstGuildID = 0;					//对战方公会
	std::string m_strDstGuildName;				//对战方公会名
	int32_t m_iDstFightStar = 0;				//对战方获得的星星数
	std::vector<CPackGuildAttackDefenseArea> m_stDstAreaVec;	//对战方的据点
};

//FIGHT_ATTACK_DEFENSE = 12021,			//开始攻防战斗[client->logic->cross->logic->client]
class CRequestFightAttackDefense
{
public:
	MSGPACK_DEFINE(m_iDstUid, m_iDstGroupID, m_iDstDifficult);

	int32_t m_iDstUid;
	int32_t m_iDstGroupID;
	int32_t m_iDstDifficult;
};

class CSRequestFightAttackDefense
{
public:
	MSGPACK_DEFINE(m_strNick, m_iGuildID, m_iDstUid, m_iDstGroupID, m_iDstDifficult);

	std::string m_strNick;
	int32_t m_iGuildID;
	int32_t m_iDstUid;
	int32_t m_iDstGroupID;
	int32_t m_iDstDifficult;
};

class CResponseFightAttackDefense : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_stChallengerPlayer, m_iDstAreaType, m_iDstDifficult);

	CPackPvpChallengerInfo m_stChallengerPlayer;
	int32_t m_iDstAreaType = 0;			//据点类型
	int32_t m_iDstDifficult = 0;		//挑战难度
	
};

//FIGHT_ATTACK_DEFENSE_RESULT = 12022,	//攻防战斗结束[client->logic->cross->logic->client]
class CRequestFightAttackDefenseResult
{
public:
	MSGPACK_DEFINE(m_bResult);

	bool m_bResult = false;
};

class CResponseFightAttackDefenseResult : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_bResult, m_iArmyPoint, m_iLastAddTime, m_iCanFightTime, m_stBonusVec, m_stChallengerPlayer, m_strFightNameVec);

	bool m_bResult = false;
	int32_t m_iArmyPoint = 0;					//军令
	int32_t m_iLastAddTime = 0;					//上次加军令时间
	int32_t m_iCanFightTime = 0;				//可以参与战斗时间
	std::vector<CPackGameItem> m_stBonusVec;
	CPackPvpChallengerInfo m_stChallengerPlayer;
	std::vector<std::string> m_strFightNameVec;
};

class CSResponseFightAttackDefenseResult
{
public:
	MSGPACK_DEFINE(m_bResult, m_bTimeOut, m_iDstAreaType, m_iDstDifficult, m_stChallengerPlayer, m_strFightNameVec);

	bool m_bResult = false;
	bool m_bTimeOut = false;
	int32_t m_iDstAreaType = 0;			//据点类型
	int32_t m_iDstDifficult = 0;		//挑战难度
	CPackPvpChallengerInfo m_stChallengerPlayer;
	std::vector<std::string> m_strFightNameVec;
};

//REPAIR_ATTACK_DEFENSE = 12023,			//修复攻防据点[client->logic->cross->logic->client]
class CRequestRepairAttackDefense
{
public:
	MSGPACK_DEFINE(m_iDstUid, m_iDstGroupID);

	int32_t m_iDstUid;
	int32_t m_iDstGroupID;
};

class CSRequestRepairAttackDefense
{
public:
	MSGPACK_DEFINE(m_iGuildID, m_iDstUid, m_iDstGroupID, m_strNick);

	int32_t m_iGuildID;
	int32_t m_iDstUid;
	int32_t m_iDstGroupID;
	std::string m_strNick;
};

class CResponseRepairAttackDefense : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_iDstUid, m_iDstGroupID, m_iRepairAreaType, m_iRepairDifficult, m_strAreaFightBitmap, m_iArmyPoint, m_iLastAddTime, m_stBonusVec, m_strFightNameVec);

	int32_t m_iDstUid;
	int32_t m_iDstGroupID;
	int32_t m_iRepairAreaType;
	int32_t m_iRepairDifficult;
	std::string m_strAreaFightBitmap;			//据点占领位图
	int32_t m_iArmyPoint = 0;					//剩下的军令
	int32_t m_iLastAddTime = 0;					//上次加军令时间
	std::vector<CPackGameItem> m_stBonusVec;
	std::vector<std::string> m_strFightNameVec;
};

//REMOVE_FIGHT_FAIL_CD = 12024,			//移除挑战失败CD[client->logic->client]
class CResponseRemoveFightFailCd : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_stCostItem);

	CPackGameItem m_stCostItem;
};

//CROSS_ATTACK_DEFENSE_SYNC = 12025,		//跨服攻防数据准备[logic->cross]
class CSRequestAttackDefenseSync
{
public:
	MSGPACK_DEFINE(m_iGuildID, m_iGuildExp, m_iGuildLevel, m_strGuildName, m_stPlayerVec, m_bIsTibu, m_iActiveMemberCount, m_iAtkDefScore, m_stMatchHistory);

	int32_t m_iGuildID;
	int32_t m_iGuildExp;
	int32_t m_iGuildLevel;
	std::string m_strGuildName;
	std::vector<CPackPvpChallengerInfo> m_stPlayerVec;
	bool m_bIsTibu = false;
	int32_t m_iActiveMemberCount = 0;
	int32_t m_iAtkDefScore = 0;
	std::set<int32_t> m_stMatchHistory;
};

//CROSS_ATTACK_DEFENSE_SYNC_DONE = 12026,	//跨服攻防数据准备完成[logic->cross->logic]
//CROSS_ATTACK_DEFENSE_HEART = 12027,		//跨服攻防心跳触发[logic->cross]	

//CROSS_ATTACK_DEFENSE_AWARD = 12028,		//跨服攻防发送奖励[cross->logic]
class CSNotifyAttackDefenseAward
{
public:
	MSGPACK_DEFINE(m_iGuildID, m_iStar, m_iDstGuildID, m_strDstGuildName, m_iDstStar);

	int32_t m_iGuildID;
	int32_t m_iStar;
	int32_t m_iDstGuildID;
	std::string m_strDstGuildName;
	int32_t m_iDstStar;
};

//CROSS_ATTACK_DEFENSE_START = 12029,		//跨服攻防开始通知[cross->logic]
class CSNotifyAttackDefenseStart
{
public:
	MSGPACK_DEFINE(m_iGuildMatched);

	std::map<int32_t, int32_t> m_iGuildMatched;
};

//CROSS_GUILD_WAR_REPORT = 12030,	//跨服公会战战报[{logic}->cross->logic]
class CSNotifyGuildWarReport
{
public:
	MSGPACK_DEFINE(m_iGuildID, m_iReportType, m_iReportValue, m_stParamMap, m_stParamVec);

	int32_t m_iGuildID;
	int32_t m_iReportType;
	int32_t m_iReportValue;
	std::map<std::string, std::string> m_stParamMap;
	std::vector<int32_t> m_stParamVec;
};

//////////////////////////////////////////////////////////////////////////////////////////
//MSG_CROSS_SERVICE_ALERT = 12200,                    //游戏服务告警[logic->cross]
class CSRequestServiceAlert
{
public:
    MSGPACK_DEFINE(m_strID, m_stParams);

    std::string m_strID;
    std::map<std::string, std::string> m_stParams;
};

//////////////////////////////////////////////////////////////////////////////////////////
//LUCKY_HAMMER_LOTTERY = 12040,			//幸运魔锤抽奖[client->logic->cross]
class CRequestLuckyHammerLottery
{
public:
	CRequestLuckyHammerLottery() : m_iActivityID(0), m_iCount(0), m_iLotteryIndex(0) {}

	MSGPACK_DEFINE(m_iActivityID, m_iCount, m_iLotteryIndex);

	int32_t m_iActivityID;
	int32_t m_iCount;
	int32_t m_iLotteryIndex;
};

class CResponseLuckyHammerLottery : public CPackUserSyncData
{
public:
	CResponseLuckyHammerLottery() : m_iActivityID(0), m_iCount(0), m_vtAwardItem(), m_stConsume(), m_iLotteryIndex(0) {}

	MSGPACK_DEFINE(m_stSyncData, m_iActivityID, m_iCount, m_vtAwardItem, m_stConsume, m_iLotteryIndex);

	int32_t m_iActivityID;
	int32_t m_iCount;
	std::vector<CPackGameItem> m_vtAwardItem;
	CPackGameItem m_stConsume;
	int32_t m_iLotteryIndex;
};

//GET_LUCKY_HAMMER_RANK = 12041,		//拉取幸运魔锤排行[client->logic->cross->logic->client]
class CRequestGetLuckyHammerRank
{
public:
	CRequestGetLuckyHammerRank() : m_iActivityID(0) {}

	MSGPACK_DEFINE(m_iActivityID);

	int32_t m_iActivityID;
};

class CResponseGetLuckyHammerRank : public CPackUserSyncData
{
public:
	CResponseGetLuckyHammerRank() : m_iActivityID(0), m_iSelfRank(0), m_stRankVec() {}

	MSGPACK_DEFINE(m_stSyncData, m_iActivityID, m_iSelfRank, m_stRankVec);

	int32_t m_iActivityID;
	int32_t m_iSelfRank;
	std::vector<CPackUserRank> m_stRankVec;
};

//GET_LUCKY_HAMMER_LOG = 12042,			//拉取幸运魔锤日志[client->logic->cross->logic->client]
class CRequestGetLuckyHammerLog
{
public:
	CRequestGetLuckyHammerLog() : m_iActivityID(0) {}

	MSGPACK_DEFINE(m_iActivityID);

	int32_t m_iActivityID;
};

class CPackLuckyHammerLogElem
{
public:
	CPackLuckyHammerLogElem() : m_iLogID(0), m_strText(), m_bLucky(false), m_bAward(false) {}
	
	MSGPACK_DEFINE(m_iLogID, m_strText, m_bLucky, m_bAward);

	int32_t m_iLogID;
	std::string m_strText;
	bool m_bLucky;	//为ture代表是好运公告，可以蹭好运
	bool m_bAward;	//当为好运公告时，标志是否蹭好运
};

class CResponseGetLuckyHammerLog : public CPackUserSyncData
{
public:
	CResponseGetLuckyHammerLog() : m_iActivityID(0), m_stLog() {}

	MSGPACK_DEFINE(m_stSyncData, m_iActivityID, m_stLog);

	int32_t m_iActivityID;
	std::vector<CPackLuckyHammerLogElem> m_stLog;
};

//LUCKY_HAMMER_LOG_SCORE = 12043,		//蹭幸运魔锤日志好运[client->logic->cross]
class CRequestLuckyHammerLogScore
{
public:
	CRequestLuckyHammerLogScore() : m_iActivityID(0), m_iLogID(0) {}

	MSGPACK_DEFINE(m_iActivityID, m_iLogID);

	int32_t m_iActivityID;
	int32_t m_iLogID;
};

class CResponseLuckyHammerLogScore : public CPackUserSyncData
{
public:
	CResponseLuckyHammerLogScore() : m_iActivityID(0), m_iLogID(0), m_stLog() {}

	MSGPACK_DEFINE(m_stSyncData, m_iActivityID, m_iLogID, m_stLog);

	int32_t m_iActivityID;
	int32_t m_iLogID;
	std::vector<CPackLuckyHammerLogElem> m_stLog;
};

//CROSS_SYNC_LUCKY_HAMMER_SCORE = 12044,	//同步幸运魔锤积分[logic->cross]
class CSRequestSyncLuckyHammerScore
{
public:
	CSRequestSyncLuckyHammerScore() : m_iActivityID(0), m_iStartTime(0), m_iEndTime(0), m_iScore(0), m_strNick() {}

	MSGPACK_DEFINE(m_iActivityID, m_iStartTime, m_iEndTime, m_iScore, m_strNick);

	int32_t m_iActivityID;
	int32_t m_iStartTime;
	int32_t m_iEndTime;
	int32_t m_iScore;
	std::string m_strNick;
};

//NOTIFY_LUCKY_HAMMER_RANK_AWARD = 12045,	//通知幸运魔锤排行奖励[cross->logic]
class CSNotifyLuckyHammerRank
{
public:
	CSNotifyLuckyHammerRank() : m_iActivityID(0), m_iRank(0), m_iScore(0) {}

	MSGPACK_DEFINE(m_iActivityID, m_iRank, m_iScore);

	int32_t m_iActivityID;
	int32_t m_iRank;
	int32_t m_iScore;
};

//CROSS_SYNC_LUCKY_HAMMER_LOG = 12046,	//同步幸运魔锤日志[logic->cross]
class CSRequestSyncLuckyHammerLog
{
public:
	CSRequestSyncLuckyHammerLog() : m_iActivityID(0), m_iStartTime(0), m_iEndTime(0), m_strContent(), m_iContentNotify(0), m_strBroadcastID(), m_stParamMap() {}

	MSGPACK_DEFINE(m_iActivityID, m_iStartTime, m_iEndTime, m_strContent, m_iContentNotify, m_strBroadcastID, m_stParamMap);

	int32_t m_iActivityID;
	int32_t m_iStartTime;
	int32_t m_iEndTime;
	std::string m_strContent;
	int32_t m_iContentNotify;
	std::string m_strBroadcastID;
	std::map<std::string, std::string> m_stParamMap;
};

class CPackBanquetGuestInfo
{
public:
    CPackBanquetGuestInfo() : m_iItemID(0), m_iRewardEndTime(0), m_iShowCardID(0), m_iSkinID(0),m_stBrief() {}

    MSGPACK_DEFINE(m_iItemID, m_iRewardEndTime, m_iShowCardID, m_stBrief,m_iSkinID);

    int32_t         m_iItemID;                      // 宴会道具
    int32_t         m_iRewardEndTime;               // 发奖时间
    int32_t         m_iShowCardID;
    int32_t         m_iSkinID;                      // 展示皮肤
    CPackUserBrief  m_stBrief;                      // 客人信息
};

class CPackBanquetTeamInfo
{
public:
    CPackBanquetTeamInfo() : m_iUin(0), m_iGroupID(0), m_iHomeID(0),m_iItemID(0), m_iBanquetEndTime(0), m_iRewardTime(0), m_iGuestMaxNum(0), m_stGuestMap(), m_bOpen(false), m_stInviteSet(),m_iTeamID(0) {}

    MSGPACK_DEFINE(m_iUin, m_iGroupID, m_iHomeID, m_iItemID, m_iBanquetEndTime, m_iRewardTime, m_iGuestMaxNum, m_stGuestMap, m_bOpen, m_stInviteSet,m_iTeamID);

    int32_t         m_iUin;                         // 宴会主人UIN
    int32_t         m_iGroupID;                     // 宴会主人区服
    int32_t         m_iHomeID;                      // 宴会房间ID
    int32_t         m_iItemID;                      // 宴会道具
    int32_t         m_iBanquetEndTime;              // 宴会结束时间
    int32_t         m_iRewardTime;                  // 奖励所需时间
    int32_t         m_iGuestMaxNum;                 // 最大座位数量
    std::map<char, CPackBanquetGuestInfo> m_stGuestMap;
    bool            m_bOpen;
    std::set<int>   m_stInviteSet;                  // 已邀请列表
    uint64_t        m_iTeamID;                      // 队伍ID
};

class CPackTeamRecovery
{
public:
    CPackTeamRecovery() : m_stUinToTeamID(), m_stTeamMap(),m_stUin2LimitBossInfo(),m_stUin2LimitBossTeamID() {}

    MSGPACK_DEFINE(m_stUinToTeamID, m_stTeamMap,m_stUin2LimitBossInfo,m_stUin2LimitBossTeamID);

    std::unordered_map<int32_t, CPackBanquetTeamIds> m_stUinToTeamID;
    std::unordered_map<uint64_t , CPackBanquetTeamInfo> m_stTeamMap;

    std::unordered_map<int32_t, CPackLimitBossTeamInfo> m_stUin2LimitBossInfo;
    std::unordered_map<int32_t, std::set<int32_t>> m_stUin2LimitBossTeamID;
};

// 排队服务器玩家登录登出通知
class CQueueUserLoginNotify
{
  public:
    CQueueUserLoginNotify() : m_iUin(0), m_bLogin(false), m_bProtect(true) {}

    MSGPACK_DEFINE(m_iUin, m_bLogin, m_bProtect);

    int32_t     m_iUin;
    bool        m_bLogin;
    bool        m_bProtect;         // 如果是下线，表示是否需要加入保护队列
};

// 排队服务器在线玩家通知
class CQueueUserListOnlineNotify
{
public:
    CQueueUserListOnlineNotify() : m_stUinVec() {}

    MSGPACK_DEFINE(m_stUinVec);

    std::vector<int32_t> m_stUinVec;
};

// 排队服务器玩家排队信息
class CQueueUserQueueHeartbeat
{
public:
    CQueueUserQueueHeartbeat() : m_bCanLogin(false), m_iBeforeNum(0), m_iRemainWaitTime(0), m_strSession("") {}

    MSGPACK_DEFINE(m_bCanLogin, m_iBeforeNum, m_iRemainWaitTime, m_strSession);

    bool        m_bCanLogin;            // 是否可登陆
    int32_t     m_iBeforeNum;           // 前面还有多少人
    int32_t     m_iRemainWaitTime;      // 剩余等待时间
    std::string m_strSession;           // ConnSessionKey
};

// 排队服务器玩家重连检查
class CQueueRequestUserReconnectCheck
{
  public:
    CQueueRequestUserReconnectCheck() : m_iUin(0) {}

    MSGPACK_DEFINE(m_iUin);

    int32_t     m_iUin;
};

class CQueueResponseUserReconnectCheck
{
  public:
    CQueueResponseUserReconnectCheck() : m_iUin(0), m_bCanLogin(false) {}

    MSGPACK_DEFINE(m_iUin, m_bCanLogin);

    int32_t     m_iUin;
    bool        m_bCanLogin;    // true:可登录，false:不可登陆(离线超时且需要排队)
};

// 公会申请缓存信息
class CPackGuildApplyInfo
{
public:
    CPackGuildApplyInfo() : m_iGroupID(0), m_iApplyTime(0) {}

    MSGPACK_DEFINE(m_iGroupID, m_iApplyTime);

    int32_t     m_iGroupID;
    int32_t     m_iApplyTime;
};

// 公会BOSS伤害
class CPackGuildBossHurtInfo
{
public:
    CPackGuildBossHurtInfo() : m_iGroupID(0), m_ulHurt(0) {}

    MSGPACK_DEFINE(m_iGroupID, m_ulHurt);

    int32_t     m_iGroupID;
    uint64_t    m_ulHurt;
};

// 公会日志
class CPackGuildLogInfo
{
public:
    CPackGuildLogInfo() : m_iTime(0), m_strLog() {}

    MSGPACK_DEFINE(m_iTime, m_strLog);

    int32_t     m_iTime;
    std::string m_strLog;
};

// 公会祈愿
class CPackGuildWishInfo
{
public:
    CPackGuildWishInfo() : m_iGroupID(0), m_iCardID(0), m_iCurNum(0), m_bCanSwap(false),m_bIsFull(false) {}

    MSGPACK_DEFINE(m_iGroupID, m_iCardID, m_iCurNum, m_bCanSwap,m_bIsFull);

    int32_t     m_iGroupID;
    int32_t     m_iCardID;
    int32_t     m_iCurNum;
    bool        m_bCanSwap;
    bool        m_bIsFull;
};

//公会吧台玩家信息
class CPackGuildUserBarInfo
{
public:
    CPackGuildUserBarInfo() : m_iUid(0), m_iGroupID(0), m_iCardID(0),m_iEndTime(0),m_iIndex(0),m_strName("") {}

    MSGPACK_DEFINE(m_iUid, m_iGroupID, m_iCardID,m_iEndTime,m_iIndex,m_strName);

    int32_t  m_iUid;
    int32_t  m_iGroupID;
    int32_t  m_iCardID;
    int32_t  m_iEndTime;
    int32_t  m_iIndex;
    std::string m_strName;
};

using guild_apply_map = std::unordered_map<int32_t, CPackGuildApplyInfo>;
using guild_boss_hurt_map = std::unordered_map<int32_t, CPackGuildBossHurtInfo>;
using guild_log_list = std::list<CPackGuildLogInfo>;
using guild_wish_map = std::unordered_map<int32_t, CPackGuildWishInfo>;
using guild_bar_map = std::unordered_map<int32_t,CPackGuildUserBarInfo>;

// 公会缓存
class CPackCentreCacheGuild
{
public:
    CPackCentreCacheGuild() : m_stGuildApplyMap(), m_stGuildBossHurtMap(), m_stGuildLogMap(), m_stGuildWishMap() {}

    MSGPACK_DEFINE(m_stGuildApplyMap, m_stGuildBossHurtMap, m_stGuildLogMap, m_stGuildWishMap);

    std::unordered_map<int32_t, guild_apply_map>        m_stGuildApplyMap;      // [guildid, <uid, info>]
    std::unordered_map<int32_t, guild_boss_hurt_map>    m_stGuildBossHurtMap;   // [guildid, <uid, hurt>]
    std::unordered_map<int32_t, guild_log_list>         m_stGuildLogMap;        // 公会日志 [guildid, 日志列表]
    std::unordered_map<int32_t, guild_wish_map>         m_stGuildWishMap;       // 公会祈愿 [guildid, <uid, info>]
    std::unordered_map<int32_t, guild_bar_map>          m_stGuildBarInfo;       // 公会吧台信息[guildID,<uid,info>]
};

class CPackCentreDataRecovery
{
public:
    CPackCentreDataRecovery() : m_stGuildCache() {}

    MSGPACK_DEFINE(m_stGuildCache);

    CPackCentreCacheGuild       m_stGuildCache;
};

class CRequestCreateBanquetTeam
{
public:
    CRequestCreateBanquetTeam() : m_iItemID(0), m_iHomeID(0), m_iDiningTableIndex(0), m_bOpen(false), m_iShowCardID(0), m_cPos(0),m_iSkinID(0) {}

    MSGPACK_DEFINE(m_iItemID, m_iHomeID, m_iDiningTableIndex, m_bOpen, m_iShowCardID, m_cPos,m_iSkinID);

    int32_t     m_iItemID;                      // 宴会所用食材道具ID
    int32_t     m_iHomeID;                      // 房间ID
    int32_t     m_iDiningTableIndex;            // 房间内餐桌家具index
    bool        m_bOpen;                        // 是否开放
    int32_t     m_iShowCardID;                  // 展示伙伴ID
    char        m_cPos;                         // 餐桌座位编号
    int32_t     m_iSkinID;                      // 展示的皮肤ID
};

class CCrossRequestCreateBanquetTeam
{
public:
    CCrossRequestCreateBanquetTeam() : m_iHomeID(0),m_iItemID(0), m_iDiningTableIndex(0), m_iGuestNum(0), m_iBanquetTime(0),
          m_iRewardTime(0), m_iShowCardID(0), m_stBrief(), m_bOpen(false), m_cPos(0),m_iSkinID(0) {}

    MSGPACK_DEFINE(m_iHomeID,m_iItemID, m_iDiningTableIndex, m_iGuestNum, m_iBanquetTime, m_iRewardTime, m_iShowCardID, m_stBrief, m_bOpen, m_cPos,m_iSkinID)

    int32_t     m_iHomeID;
    int32_t     m_iItemID;
    int32_t     m_iDiningTableIndex;
    int32_t     m_iGuestNum;
    int32_t     m_iBanquetTime;
    int32_t     m_iRewardTime;
    int32_t     m_iShowCardID;
    CPackUserBrief m_stBrief;
    bool        m_bOpen;
    char        m_cPos;
    int32_t     m_iSkinID;
};

class CCrossResponseCreateBanquetTeam
{
public:
    CCrossResponseCreateBanquetTeam() : m_iItemID(0),m_uiTeamID(0) {}

    MSGPACK_DEFINE(m_iItemID,m_uiTeamID);

    int32_t     m_iItemID;
    uint64_t    m_uiTeamID;
};

class CNotifyBanquetInfo : public CPackUserSyncData
{
public:
    CNotifyBanquetInfo() : m_stBanquetInfo() {}

    MSGPACK_DEFINE(m_stSyncData, m_stBanquetInfo);

    CPackBanquetTeamInfo m_stBanquetInfo;
};

class CRequestJoinBanquetTeam
{
public:
    CRequestJoinBanquetTeam() : m_iTeamID(0), m_iShowCardID(0), m_stBrief(), m_cPos(0),m_iSkinID(0) {}

    MSGPACK_DEFINE(m_iTeamID, m_iShowCardID, m_stBrief, m_cPos,m_iSkinID);

    uint64_t    m_iTeamID;
    int32_t     m_iShowCardID;
    CPackUserBrief m_stBrief;
    char        m_cPos;
    int32_t     m_iSkinID;
};
class CResponseJoinBanquetTeam: public CPackUserSyncData
{
public:
    CResponseJoinBanquetTeam() : m_iTeamID(0) {}

    MSGPACK_DEFINE(m_stSyncData,m_iTeamID);

    uint64_t     m_iTeamID;
};



class CRequestGetBanquetInfo
{
public:
    CRequestGetBanquetInfo() : m_iTeamID(0) {}

    MSGPACK_DEFINE(m_iTeamID);

    uint64_t    m_iTeamID;
};

class CRequestBanquetInvite
{
public:
    CRequestBanquetInvite() : m_iHomeID(0), m_stUinMap() {}

    MSGPACK_DEFINE(m_iHomeID, m_stUinMap);

    int     m_iHomeID;
    std::map<int, int> m_stUinMap;      // <uin, group_id>
};

class CCrossBanquetInvite
{
public:
    CCrossBanquetInvite() : m_iHomeID(0), m_stUinMap(), m_stBrief() {}

    MSGPACK_DEFINE(m_iHomeID, m_stUinMap, m_stBrief);

    int     m_iHomeID;
    std::map<int, int> m_stUinMap;      // <uin, group_id>
    CPackUserBrief m_stBrief;           // 服务器使用
};

class CCrossNotifyBanquetInvite : public CPackUserSyncData
{
public:
    CCrossNotifyBanquetInvite() : m_iTeamID(0), m_stOwnerBrief(), m_iHomeID(0), m_iItemID(0), m_iEndTime(0) {}

    MSGPACK_DEFINE(m_stSyncData, m_iTeamID, m_stOwnerBrief, m_iHomeID, m_iItemID, m_iEndTime);

    uint64_t            m_iTeamID;
    CPackUserBrief      m_stOwnerBrief;
    int                 m_iHomeID;
    int                 m_iItemID;
    int                 m_iEndTime;
};

class CResponseGetBanquetInviteList : public CPackUserSyncData
{
public:
    struct invite_data
    {
        MSGPACK_DEFINE(m_iTeamID, m_stOwnerBrief, m_iHomeID, m_iItemID, m_iEndTime);

        uint64_t        m_iTeamID;          // 宴会队伍ID
        CPackUserBrief  m_stOwnerBrief;     // 宴会主办人
        int             m_iHomeID;          // 宴会房间ID
        int             m_iItemID;          // 宴会道具
        int             m_iEndTime;         // 宴会结束时间
    };

    CResponseGetBanquetInviteList() : m_stInviteVec() {}

    MSGPACK_DEFINE(m_stSyncData, m_stInviteVec);

    std::vector<invite_data> m_stInviteVec;
};

class CCrossNotifyBanquetReward
{
public:
    CCrossNotifyBanquetReward() : m_iItemID(0),m_iCreateID(0),m_iJoinID(0),m_iEndTeamID(0) {}

    MSGPACK_DEFINE(m_iItemID,m_iCreateID,m_iJoinID,m_iEndTeamID);

    int32_t     m_iItemID;
    uint64_t    m_iCreateID;
    uint64_t    m_iJoinID;
    uint64_t    m_iEndTeamID;
};

class CNotifyBanquetUpdateLogin : public CPackUserSyncData
{
public:
    CNotifyBanquetUpdateLogin() : m_iCreateId(0),m_iJoinId(0) {}

    MSGPACK_DEFINE(m_stSyncData, m_iCreateId,m_iJoinId);

    uint64_t    m_iCreateId;
    uint64_t    m_iJoinId;
};

class CRequestBanquetQuitTeam
{
public:
    CRequestBanquetQuitTeam() : m_iTeamId(0){}

    MSGPACK_DEFINE(m_iTeamId);

    uint64_t m_iTeamId;          // 队伍id
};

class CCrossRequestBanquetQuitTeam
{
public:
    CCrossRequestBanquetQuitTeam() :m_iTeamId(0){}

    MSGPACK_DEFINE(m_iTeamId);

    uint64_t m_iTeamId;          // 队伍id
};

class CCrossResponseQuitBanquetTeam: public CPackUserSyncData
{
public:
    CCrossResponseQuitBanquetTeam() :m_iQuitId(0) {}
    MSGPACK_DEFINE(m_stSyncData,m_iQuitId);

    uint64_t m_iQuitId;          //退出队伍ID
};

class CCrossRequestFriendBanquet
{
public:
    struct user_key
    {
        MSGPACK_DEFINE(m_iUin, m_iGroupID);

        int32_t m_iUin;
        int32_t m_iGroupID;
    };

    CCrossRequestFriendBanquet():m_stUserVec(){}

    MSGPACK_DEFINE(m_stUserVec);

    std::vector<user_key> m_stUserVec;
};



class CCrossResponseOpenBanquetTeam: public CPackUserSyncData
{
public:
    CCrossResponseOpenBanquetTeam() :m_stOpenBanquetInfo() {}
    MSGPACK_DEFINE(m_stSyncData,m_stOpenBanquetInfo);

    std::vector<CPackBanquetTeamInfo> m_stOpenBanquetInfo;
};


///////////////////////////////////////////////////////////////////////////////////////////
class CCrossRequestModifyGuild
{
public:
    CCrossRequestModifyGuild() : m_iGuildID(0), m_iGuildExp(0), m_iFd(0) {}

    MSGPACK_DEFINE(m_iGuildID, m_iGuildExp, m_iFd);

    int32_t m_iGuildID;
    int32_t m_iGuildExp;
    int32_t m_iFd;
};

class CCrossResponseModifyGuild
{
public:
    CCrossResponseModifyGuild() : m_iCode(0), m_iFd(0) {}

    MSGPACK_DEFINE(m_iCode, m_iFd);

    int32_t m_iCode;
    int32_t m_iFd;
};

class CCrossRequestDismissGuild
{
public:
    CCrossRequestDismissGuild() : m_iGuildID(0), m_iChiefUid(0), m_strSender(), m_strTitle(), m_strText(), m_iFd(0) {}

    MSGPACK_DEFINE(m_iGuildID, m_iChiefUid, m_strSender, m_strTitle, m_strText, m_iFd);

    int32_t m_iGuildID;
    int32_t m_iChiefUid;
    std::string m_strSender;
    std::string m_strTitle;
    std::string m_strText;
    int32_t m_iFd;
};

class CCrossRequestSetGuildChief
{
public:
    CCrossRequestSetGuildChief() : m_iGuildID(0), m_iFd(0) {}

    MSGPACK_DEFINE(m_iGuildID, m_iFd);

    int32_t m_iGuildID;
    int32_t m_iFd;
};

class CCrossRequestChangeGuildName
{
public:
    CCrossRequestChangeGuildName() : m_iGuildID(0), m_strGuildName(""), m_strSender(""), m_strTitle(""), m_strText(""), m_iFd(0) {}

    MSGPACK_DEFINE(m_iGuildID, m_strGuildName, m_strSender, m_strTitle, m_strText, m_iFd);

    int32_t     m_iGuildID;
    std::string m_strGuildName;
    std::string m_strSender;
    std::string m_strTitle;
    std::string m_strText;
    int32_t m_iFd;
};

class CCrossGuildBossHurtNotify
{
public:
    CCrossGuildBossHurtNotify() : m_ulHurt(0) {}

    MSGPACK_DEFINE(m_ulHurt);

    uint64_t    m_ulHurt;
};

class CCrossGuildBossRewardNotify
{
public:
    CCrossGuildBossRewardNotify() : m_iGroupID(0), m_stUinVec(), m_ulTotalHurt(0), m_iWeek(0) {}

    MSGPACK_DEFINE(m_iGroupID, m_stUinVec, m_ulTotalHurt, m_iWeek);

    int32_t                 m_iGroupID;
    std::vector<int32_t>    m_stUinVec;
    uint64_t                m_ulTotalHurt;
    int32_t                 m_iWeek;
};

class CNotifyGuildChange : public CPackUserSyncData
{
public:
    CNotifyGuildChange() : m_iGuildID(0), m_strGuildName(), m_iGuildExp(0), m_iGuildPost(0), m_bIsExpel(false) {}

    MSGPACK_DEFINE(m_stSyncData, m_iGuildID, m_strGuildName, m_iGuildExp, m_iGuildPost, m_bIsExpel);

    int32_t         m_iGuildID;
    std::string     m_strGuildName;
    int32_t         m_iGuildExp;
    int32_t         m_iGuildPost;
    bool            m_bIsExpel;
};

class CSNotifyUserTalk
{
public:
    CSNotifyUserTalk() : m_stTalkInfo(), m_iDstUin(0), m_iDstGroupID(0), m_stUinVec() {}

    MSGPACK_DEFINE(m_stTalkInfo, m_iDstUin, m_iDstGroupID, m_stUinVec);

    CPackTalkInfo   m_stTalkInfo;
    int32_t         m_iDstUin;              // 接收聊天的目标玩家(公会聊天里填公会ID)
    int32_t         m_iDstGroupID;          // 接收聊天的目标玩家
    std::vector<int32_t> m_stUinVec;        // 接收聊天的目标玩家
};

class CNotifyLimitBossInfo: public CPackUserSyncData
{
public:
    CNotifyLimitBossInfo():m_bIsExist(false){}

    MSGPACK_DEFINE(m_stSyncData,m_bIsExist);

    bool m_bIsExist;
};