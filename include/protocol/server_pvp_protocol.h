#pragma once

#include "msgpack.hpp"

#include "server_base_protocol.h"
////////////////////////////////////////////////////////////////////////////////////////////////////
class CResponseGetPvpChallengerList: public CPackUserSyncData
{
public:
    CResponseGetPvpChallengerList() : m_stChallengerList(),m_stCurrRewardState() {}

    MSGPACK_DEFINE(m_stSyncData, m_stChallengerList,m_stCurrRewardState);

    std::vector<CPackPvpUserInfo> m_stChallengerList;
    std::map<int32_t ,bool> m_stCurrRewardState;
};


class CResponseFlushPvpChallenger : public CPackUserSyncData
{
public:
	CResponseFlushPvpChallenger() : m_stChallengerList(),m_stCurrRewardState() {}

	MSGPACK_DEFINE(m_stSyncData, m_stChallengerList,m_stCurrRewardState);

	std::vector<CPackPvpUserInfo> m_stChallengerList;
    std::map<int32_t ,bool> m_stCurrRewardState;
};

class CResponseGetPvpInfo : public CPackUserSyncData
{
public:
	CResponseGetPvpInfo() : m_iPVPRank(0), m_iPVPScore(0),m_iPVPMaxScore(0),m_iPVPWeeklyQuestID() {}

	MSGPACK_DEFINE(m_stSyncData, m_iPVPRank,m_iPVPScore, m_iPVPMaxScore,m_iPVPWeeklyQuestID);

	int         m_iPVPRank;             //当前排名
    int32_t     m_iPVPScore;            //当前积分
	int32_t		m_iPVPMaxScore;         //最大积分
    std::vector<int32_t> m_iPVPWeeklyQuestID;
};

class CRequestFightPvp
{
public:
	CRequestFightPvp() : m_iUin(0), m_iGroupID(0) {}

	MSGPACK_DEFINE(m_iUin, m_iGroupID);

	int                     m_iUin;
	int                     m_iGroupID;
};

class CResponseFightPvp : public CPackUserSyncData
{
public:
	CResponseFightPvp() : m_stChallengerInfo() {}

	MSGPACK_DEFINE(m_stSyncData, m_stChallengerInfo);

	CPackPvpChallengerInfo     m_stChallengerInfo;
};

class CPackPvpCardFightLog
{
public:
    CPackPvpCardFightLog() : m_iCardID(0), m_iLevel(0), m_iDamage(0), m_iTherapy(0), m_iHurt(0),m_bIsAlive(false) {}

    MSGPACK_DEFINE(m_iCardID, m_iLevel, m_iDamage, m_iTherapy, m_iHurt,m_bIsAlive);

    int16_t     m_iCardID;
    int16_t     m_iLevel;
    int32_t     m_iDamage;
    int32_t     m_iTherapy;
    int32_t     m_iHurt;
    bool        m_bIsAlive;
};

class CPackPvpUserFightLog
{
public:
    CPackPvpUserFightLog() : m_stCardLogVec(), m_strNick(), m_iAvatarID(0), m_iAvatarBorder(0) {}

    MSGPACK_DEFINE(m_stCardLogVec, m_strNick, m_iAvatarID, m_iAvatarBorder);

    std::vector<CPackPvpCardFightLog> m_stCardLogVec;
    std::string     m_strNick;
    int32_t         m_iAvatarID;
    int32_t         m_iAvatarBorder;
};

class CPackPvpFightLog
{
public:
    CPackPvpFightLog() : m_bWin(false), m_iUin(0),m_stAttackLog(), m_stDefendLog(), m_iTime(0),m_iDurationTime(0), m_iAttackRankChange(0),m_iDefendRankChange(0), m_iAttackRank(0),m_iDefendRank(0)
    ,m_iAttackCurrScore(0),m_iDefendCurrScore(0){}

    MSGPACK_DEFINE(m_bWin, m_iUin,m_stAttackLog, m_stDefendLog, m_iTime,m_iDurationTime, m_iAttackRankChange,m_iDefendRankChange,m_iAttackRank,m_iDefendRank,
                   m_iAttackCurrScore,m_iDefendCurrScore);

    bool                    m_bWin;
    int                     m_iUin;                           //进攻方uin
    CPackPvpUserFightLog    m_stAttackLog;
    CPackPvpUserFightLog    m_stDefendLog;
    int                     m_iTime;
    int                     m_iDurationTime;
    int                     m_iAttackRankChange;              //攻击方积分变化
    int                     m_iDefendRankChange;              //防守方方积分变化
    unsigned int            m_iAttackRank;                    //攻击方结算排名
    unsigned int            m_iDefendRank;                    //防守方结算排名
    int                     m_iAttackCurrScore;               //攻击方当前积分
    int                     m_iDefendCurrScore;               //防守方当前积分

};

class CRequestFightPvpResult
{
public:
    CRequestFightPvpResult() : m_iUin(0), m_iGroupID(0), m_stFightLog() {}

    MSGPACK_DEFINE(m_iUin, m_iGroupID, m_stFightLog);

    int                 m_iUin;
    int                 m_iGroupID;
    CPackPvpFightLog    m_stFightLog;
};

class CResponseFightPvpResult : public CPackUserSyncData
{
public:
	CResponseFightPvpResult() : m_iNewRank(0), m_iOldRank(0),m_stPvpReward() {}

	MSGPACK_DEFINE(m_stSyncData, m_iNewRank, m_iOldRank,m_stPvpReward);

	int                 m_iNewRank;
	int                 m_iOldRank;
    CPackTotalItemData  m_stPvpReward;
};

class CResponseGetPvpFightHistory : public CPackUserSyncData
{
public:
	CResponseGetPvpFightHistory() : m_stLogList() {}

	MSGPACK_DEFINE(m_stSyncData, m_stLogList);

	std::vector<CPackPvpFightLog> m_stLogList;
};

class CRequestGetPvpFightReward
{
public:
    CRequestGetPvpFightReward() : m_stRewardIndex() {}

    MSGPACK_DEFINE(m_stRewardIndex);

    std::vector<int32_t> m_stRewardIndex;
};

class CResponseGetPvpFightReward : public CPackUserSyncData
{
public:
    CResponseGetPvpFightReward() : m_stItemData() {}

    MSGPACK_DEFINE(m_stSyncData,m_stItemData);

    CPackTotalItemData  m_stItemData;
};