#pragma once

#include "msgpack.hpp"
#include "server_base_protocol.h"
#include "server_item_protocol.h"
#include "server_common_protocol.h"


//SCORE_LEVEL_INFO_NOTICE = 10831,					//信息通知
class CNotifyScoreLevelInfo : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_iActivityID, m_iXingdongli, m_iLastXingdongliTime, m_iBuyXingdongliTime, m_iScore
			, m_strScoreAwardBitmap, m_strDepositAwardBitmap, m_strScoreLevelBitmap, m_strChallengeLevelBitmap);

	int32_t m_iActivityID = 0;
	int32_t m_iXingdongli = 0;
	int32_t m_iLastXingdongliTime = 0;
	int32_t m_iBuyXingdongliTime = 0;
	int32_t m_iScore = 0;
	std::string m_strScoreAwardBitmap;
	std::string m_strDepositAwardBitmap;
	std::string m_strScoreLevelBitmap;
	std::string m_strChallengeLevelBitmap;
};

//SCORE_LEVEL_FIGHT_START = 10832,				//战斗开始
class CRequestScoreLevelFightStart
{
public:
	MSGPACK_DEFINE(m_iIndex, m_iCount, m_bChallenge);

	int32_t m_iIndex = 0;
	int32_t m_iCount = 0;
	bool m_bChallenge = false;
};

class CResponseScoreLevelFightStart : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_iIndex, m_iCount, m_bChallenge);

	int32_t m_iIndex = 0;
	int32_t m_iCount = 0;
	bool m_bChallenge = false;
};

//SCORE_LEVEL_FIGHT_RESULT = 10833,				//战斗结果通知
class CRequestScoreLevelFightResult
{
public:
	MSGPACK_DEFINE(m_iIndex, m_iCount, m_bChallenge);

	int32_t m_iIndex = 0;
	int32_t m_iCount = 0;
	bool m_bChallenge = false;
};

class CResponseScoreLevelFightResult : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_iIndex, m_iCount, m_bChallenge, m_iAddScore, m_iCostXingdongli, m_stBonusItemVec);

	int32_t m_iIndex = 0;
	int32_t m_iCount = 0;
	bool m_bChallenge = false;
	int32_t m_iAddScore = 0;
	int32_t m_iCostXingdongli = 0;
	std::vector<CPackGameItem> m_stBonusItemVec;
};

//SCORE_LEVEL_FIGHT_SWEEP = 10834,				//关卡扫荡


//SCORE_LEVEL_GET_SCORE_AWARD = 10835,			//领取积分宝箱奖励请求
class CRequestScoreLevelScoreAward
{
public:
	MSGPACK_DEFINE(m_iIndex, m_bDeposit);

	int32_t m_iIndex = 0;
	bool m_bDeposit = false;
};

class CResponseScoreLevelScoreAward : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_iIndex, m_bDeposit, m_stBonusItemVec);

	int32_t m_iIndex = 0;
	bool m_bDeposit = false;
	std::vector<CPackGameItem> m_stBonusItemVec;
};

//SCORE_LEVEL_BUY_XINGDONGLI = 10836,		        //购买行动力
class CResponseScoreLevelBuyXingdongli : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_iCostYuanbao);

	int32_t m_iCostYuanbao = 0;
};
