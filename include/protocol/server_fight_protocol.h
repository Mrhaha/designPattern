#pragma once

#include "msgpack.hpp"

#include "server_base_protocol.h"
#include "server_common_protocol.h"

////////////////////////////////////////////////////////////////////////////////////////
//调整站位
class CRequestChangeTeam
{
public:
	MSGPACK_DEFINE(m_iTeamType, m_stTeam, m_iPara);

    CRequestChangeTeam() : m_iTeamType(0), m_stTeam(), m_iPara(0) {}

    int                 m_iTeamType;        // EnumTeamType
	CPackCardTeam       m_stTeam;
	int                 m_iPara;            // 额外参数
};

class CResponseChangeTeam : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData, m_iTeamType, m_stTeam, m_iPara);

    CResponseChangeTeam() : m_iTeamType(0), m_stTeam(), m_iPara(0) {}

    int                 m_iTeamType;        // EnumTeamType
    CPackCardTeam       m_stTeam;
    int                 m_iPara;            // 额外参数
};

class CRequestChangeTeamPreset
{
public:
    CRequestChangeTeamPreset() : m_iTeamIndex(0), m_stTeam() {}

    MSGPACK_DEFINE(m_iTeamIndex, m_stTeam);

    int32_t         m_iTeamIndex;
    CPackCardTeam   m_stTeam;
};

class CResponseChangeTeamPreset : public CPackUserSyncData
{
public:
    CResponseChangeTeamPreset() : m_iTeamIndex(0), m_stTeam() {}

    MSGPACK_DEFINE(m_stSyncData, m_iTeamIndex, m_stTeam);

    int32_t         m_iTeamIndex;
    CPackCardTeam   m_stTeam;
};

class CRequestChangeTeamPresetName
{
public:
    CRequestChangeTeamPresetName() : m_iTeamIndex(0), m_strName() {}

    MSGPACK_DEFINE(m_iTeamIndex, m_strName);

    int32_t         m_iTeamIndex;
    std::string     m_strName;
};

///////////////////////////////////////////////////////////////////////////////////////////////
//获取关卡信息
class CResponseGetLevelInfo : public CPackUserSyncData
{
public:
	CResponseGetLevelInfo() : m_stLevelInfoVec() {}

	MSGPACK_DEFINE(m_stSyncData, m_stLevelInfoVec, m_strNormalChapterBonus1, m_strNormalChapterBonus2,
                   m_strNormalChapterBonus3, m_strEliteChapterBonus1, m_strEliteChapterBonus2, m_strEliteChapterBonus3);

	std::vector<CPackLevelInfo>     m_stLevelInfoVec;
	std::string                     m_strNormalChapterBonus1;
	std::string                     m_strNormalChapterBonus2;
	std::string                     m_strNormalChapterBonus3;
	std::string                     m_strEliteChapterBonus1;
    std::string                     m_strEliteChapterBonus2;
    std::string                     m_strEliteChapterBonus3;
};

//获取关卡信息
class CRequestGetLevelTimesInfo
{
public:
    CRequestGetLevelTimesInfo() : m_stLevelIds() {}

    MSGPACK_DEFINE(m_stLevelIds);

    std::vector<int>            m_stLevelIds;
};

class CResponseGetLevelTimesInfo : public CPackUserSyncData
{
public:
    CResponseGetLevelTimesInfo() : m_stLeveTimesInfoMap() {}

    MSGPACK_DEFINE(m_stSyncData, m_stLeveTimesInfoMap);

    std::vector<CPackLevelInfo>     m_stLeveTimesInfoMap;
};

///////////////////////////////////////////////////////////////////////////////////////////////
//获取关卡通过奖励
class CRequestGetLevelBonus
{
public:
	CRequestGetLevelBonus() : m_iLevelID(0), m_cIndex(0) {}

	MSGPACK_DEFINE(m_iLevelID, m_cIndex);

	int         m_iLevelID;
    int8_t      m_cIndex;           // 成就索引，从0开始   -1表示全部领取
};

class CResponseGetLevelBonus : public CPackUserSyncData
{
public:
	CResponseGetLevelBonus() : m_iLevelID(0), m_cBonusTag(0), m_stItemData() {}

	MSGPACK_DEFINE(m_stSyncData, m_iLevelID, m_cBonusTag, m_stItemData);

	int                         m_iLevelID;
    int8_t                      m_cBonusTag;
	CPackTotalItemData          m_stItemData;
};

class CResponseGetEquipLevelAffix : public CPackUserSyncData
{
public:
    CResponseGetEquipLevelAffix() : m_stAffixVec() {}

    MSGPACK_DEFINE(m_stSyncData, m_stAffixVec);

    std::vector<int32_t> m_stAffixVec;
};

///////////////////////////////////////////////////////////////////////////////////////////////
//进入关卡
class CRequestFightLevel
{
public:
	CRequestFightLevel() : m_iLevelID(0) ,m_iTodayMaxLevelID(0){}

	MSGPACK_DEFINE(m_iLevelID,m_iTodayMaxLevelID);

	int             m_iLevelID;
    int             m_iTodayMaxLevelID;         //开始公会boss副本,需要知道今日念写卡通关的副本ID;
};

class CResponseFightLevel : public CPackUserSyncData
{
public:
	CResponseFightLevel() : m_iPhyPower(0), m_iLastAutoAddEnergyTime(0), m_stBonusList(), m_stZhuHaiDrop() {}

	MSGPACK_DEFINE(m_stSyncData, m_iPhyPower, m_iLastAutoAddEnergyTime, m_stBonusList, m_stZhuHaiDrop);

	int                                     m_iPhyPower;
	int                                     m_iLastAutoAddEnergyTime;
	std::vector<CPackGameItem>              m_stBonusList;
    std::vector<CPackItemVec>               m_stZhuHaiDrop;
};

////////////////////////////////////////////////////////////////////////////////////////
//获取关卡奖励
class CRequestGetFightGift
{
public:
	struct card_status_t
	{
		card_status_t() : m_iCardID(0), m_iHP(0) {}

		MSGPACK_DEFINE(m_iCardID, m_iHP);

		int         m_iCardID;
		int         m_iHP;
	};

	CRequestGetFightGift() :
		m_iLevelID(0), m_iGetCoinNum(0), m_iGetBoxNum(0),
		m_iBossNum(0), m_iCostBossFightTime(0), m_stCardStatus(),
		m_stBonusList(), m_stKilledMonsterMap(), m_stKilledBossMap() {}

	MSGPACK_DEFINE(m_iLevelID,
		m_iGetCoinNum,
		m_iGetBoxNum,
		m_iBossNum,
		m_iCostBossFightTime,
		m_stCardStatus,
		m_stBonusList,
		m_stKilledMonsterMap,
		m_stKilledBossMap);

	int                                         m_iLevelID;
	int                                         m_iGetCoinNum;
	int                                         m_iGetBoxNum;
	int                                         m_iBossNum;
	int                                         m_iCostBossFightTime;
	std::vector<card_status_t>                  m_stCardStatus;
	std::vector<CPackGameItem>                  m_stBonusList;
	std::map<std::string, int>                  m_stKilledMonsterMap;
	std::map<std::string, int>                  m_stKilledBossMap;
};

class CResponseGetFightGift : public CPackUserSyncData
{
public:
	CResponseGetFightGift() : m_stStarNum(), m_iScore(0), m_iCoinNum(0), m_iPhyPower(0), m_iLastAutoAddEnergyTime(0),
        m_iTeamLevel(0), m_iTeamExp(0), m_stCardLevelExp(), m_stPerfectBonusList(), m_stFirstBonusList(),
        m_stConstelList(), m_stEquipList(), m_iConstelExpPool(0) {}

	MSGPACK_DEFINE(m_stSyncData, m_stStarNum, m_iScore, m_iCoinNum, m_iPhyPower, m_iLastAutoAddEnergyTime, m_iTeamLevel, m_iTeamExp,
                   m_stCardLevelExp, m_stPerfectBonusList, m_stFirstBonusList, m_stConstelList, m_stEquipList, m_iConstelExpPool);

	std::vector<int>                                m_stStarNum;
	int                                             m_iScore;
	int                                             m_iCoinNum;
	int                                             m_iPhyPower;
	int                                             m_iLastAutoAddEnergyTime;
	int                                             m_iTeamLevel;
	int                                             m_iTeamExp;
	std::vector<std::pair<int, int> >               m_stCardLevelExp;
	std::vector<CPackGameItem>                      m_stPerfectBonusList;
	std::vector<CPackGameItem>                      m_stFirstBonusList;
	std::vector<CPackConstellationInfo>             m_stConstelList;
    std::vector<CPackEquipInfo>                     m_stEquipList;
	int                                             m_iConstelExpPool;
};

// 副本战斗参数，用于评价
class CPackLevelFightPara
{
public:
    CPackLevelFightPara() : m_iGetCoinNum(0), m_iGetBoxNum(0), m_iBossCostTime(0), m_ulPara(0), m_stCardStatus(),
          m_stKillMonsterMap(), m_stAchieveTagVec(), m_stKillBossMap(), m_iWipeFightNum(0), m_iCostTime(0),m_stCardUseSkill() {}

    MSGPACK_DEFINE(m_iGetCoinNum, m_iGetBoxNum, m_iBossCostTime, m_ulPara, m_stCardStatus, m_stKillMonsterMap, m_stAchieveTagVec, m_stKillBossMap, m_iWipeFightNum, m_iCostTime,m_stCardUseSkill);

    int32_t         m_iGetCoinNum;                      // 收集金币数量
    int32_t         m_iGetBoxNum;                       // 收集宝箱数量
    int32_t         m_iBossCostTime;                    // 击杀BOSS消耗时间
    uint64_t        m_ulPara;                           // 其他参数，跑马距离，美酒副本解救人质数量，伤害量等
    std::map<int32_t, int32_t>      m_stCardStatus;     // 伙伴状态 [card_id, 剩余血量]
    std::map<std::string, int32_t>  m_stKillMonsterMap; // 击杀小怪数量 [怪物名, 数量]
    std::vector<bool>               m_stAchieveTagVec;  // 副本成就达成情况
    std::map<std::string, int32_t>  m_stKillBossMap;    // 击杀BOSS [BOSS名, 是否未被BOSS击中]
    int32_t         m_iWipeFightNum;                    // 擦弹次数
    int32_t         m_iCostTime;                        // 通关副本消耗时间
    std::map<int32_t,int32_t>   m_stCardUseSkill;       // 各个card使用奥义次数
};

//获取关卡奖励
class CRequestGetFightLevelGift
{
public:
    CRequestGetFightLevelGift() : m_iLevelID(0), m_stFightPara() {}

    MSGPACK_DEFINE(m_iLevelID, m_stFightPara);

    int32_t             m_iLevelID;
    CPackLevelFightPara m_stFightPara;
};

class CResponseGetFightLevelGift : public CPackUserSyncData
{
public:
    CResponseGetFightLevelGift() : m_stLevelInfo(), m_stTeamLevelVec(), m_stItemData(), m_stFirstBonusMap(), m_cLevelStar(0),m_stExtraItemData() {}

    MSGPACK_DEFINE(m_stSyncData, m_stLevelInfo, m_stTeamLevelVec, m_stItemData, m_stFirstBonusMap, m_cLevelStar,m_stExtraItemData);

    CPackLevelInfo                      m_stLevelInfo;
    std::vector<std::pair<int, int>>    m_stTeamLevelVec;
    CPackTotalItemData                  m_stItemData;
    std::map<int, int>                  m_stFirstBonusMap;
    int8_t                              m_cLevelStar;               //本次星数
    CPackTotalItemData                  m_stExtraItemData;
};

////////////////////////////////////////////////////////////////////////////////////////
//关卡扫荡
class CRequestSweepLevel
{
public:
	CRequestSweepLevel() : m_iLevelID(0), m_iTimes(0), m_iNeedItemType(0), m_iNeedItemID(0), m_iNeedNum(0) {}

	MSGPACK_DEFINE(m_iLevelID, m_iTimes, m_iNeedItemType, m_iNeedItemID, m_iNeedNum);

	int             m_iLevelID;
	int             m_iTimes;

    int             m_iNeedItemType;
    int             m_iNeedItemID;
    int             m_iNeedNum;         // 填剩余需要的数量
};

class CResponseSweepLevel : public CPackUserSyncData
{
public:
	CResponseSweepLevel() : m_iLevelID(0), m_iTimes(0), m_bLimitBoss(false),m_stItemDataVec(),m_stTeamLevelVec(),m_stExtraItemData(),m_bCrushBoss(false) {}

	MSGPACK_DEFINE(m_stSyncData, m_iLevelID, m_iTimes, m_bLimitBoss,m_stItemDataVec, m_stTeamLevelVec,m_stExtraItemData,m_bCrushBoss);

    int             m_iLevelID;
    int             m_iTimes;
    bool            m_bLimitBoss;                           // 是否出现限时Boss
	std::vector<CPackTotalItemData>     m_stItemDataVec;
	std::vector<std::pair<int, int>>    m_stTeamLevelVec;
    CPackTotalItemData          m_stExtraItemData;          // 额外掉落展示
    bool            m_bCrushBoss;                           // 是否出现遭遇Boss
};

class CRequestChapterStarGift
{
public:
	CRequestChapterStarGift() : m_iChapterID(0), m_iPosition(0) { }

	MSGPACK_DEFINE(m_iChapterID, m_iPosition);

	int             m_iChapterID;
	int             m_iPosition;
};

class CResponseChapterStarGift : public CPackUserSyncData
{
public:
	CResponseChapterStarGift() : m_iChapterID(0), m_iPosition(0) { }

	MSGPACK_DEFINE(m_stSyncData, m_iChapterID, m_iPosition, m_strChapterBonus, m_stBonusListVec);

	int                                         m_iChapterID;
	int                                         m_iPosition;
	std::string                                 m_strChapterBonus;
	std::vector<CPackGameItem>   m_stBonusListVec;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
class CPackChallengeUiPvpInfo
{
public:
	CPackChallengeUiPvpInfo() : m_iLeftTimes(0) {}

	MSGPACK_DEFINE(m_iLeftTimes);

	int         m_iLeftTimes;
};

class CResponseChallengeUiInfo : public CPackUserSyncData
{
public:
	CResponseChallengeUiInfo() : m_stPVPInfo() {}

	MSGPACK_DEFINE(m_stSyncData, m_stPVPInfo);

	CPackChallengeUiPvpInfo     m_stPVPInfo;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
class CRequestGetCrossFightResult
{
public:
	CRequestGetCrossFightResult() : m_iResult(0), m_iTeamID(0), m_stCostHP(), m_stUserCostHP() {}

	MSGPACK_DEFINE(m_iResult, m_iTeamID, m_stCostHP, m_stUserCostHP);

	int                                 m_iResult;
	int                                 m_iTeamID;
	std::vector<int>                    m_stCostHP;
	std::vector<std::pair<int, int>>    m_stUserCostHP;
};

class CResponseGetCrossFightResult : public CPackUserSyncData
{
public:
	CResponseGetCrossFightResult() : m_iTeamID(0), m_stRewards() {}

	MSGPACK_DEFINE(m_stSyncData, m_iTeamID, m_stRewards);

	int                         m_iTeamID;
    std::vector<CPackGameItem>  m_stRewards;
};

class CPackCrossFightChallenger
{
public:
	MSGPACK_DEFINE(m_iPassed, m_stPlayer);

	int32_t m_iPassed;
	CPackPvpChallengerInfo m_stPlayer;
};

//GET_CROSS_FIGHT_CHALLENGER = 10139,			//获取过关斩将挑战信息
class CResponseGetCrossFightChallenger : public CPackUserSyncData
{
public:
	CResponseGetCrossFightChallenger() : m_iCurProgress(0), m_stChallengerList() {}

	MSGPACK_DEFINE(m_stSyncData, m_iCurProgress, m_stChallengerList);

    int32_t m_iCurProgress;             // 当前已通关进度
	std::map<int32_t, CPackPvpChallengerInfo> m_stChallengerList;
};

//CROSS_FIGHT_AUTO_FIGHT = 10416,		//过关斩将自动战斗
class CResponseCrossFightAutoFight : public CPackUserSyncData
{
public:
	CResponseCrossFightAutoFight() : m_stBonusItemVec(), m_iCurProgress(0) {}

	MSGPACK_DEFINE(m_stSyncData, m_stBonusItemVec, m_iCurProgress);

	std::vector<CPackGameItem>  m_stBonusItemVec;			// 获得奖励
    int32_t                     m_iCurProgress;             // 当前已通关进度
};

//FIGHT_CLIMB_TOWER_RESULT = 10175,	//镇魂之塔战斗结束
class CRequestFightClimbTowerResult
{
public:
	CRequestFightClimbTowerResult() : m_iOrder(0), m_iGetCoinNum(), m_stBonusList() {}

	MSGPACK_DEFINE(m_iOrder, m_iGetCoinNum, m_stBonusList);

	int		m_iOrder;
	int		m_iGetCoinNum;
	std::vector<CPackGameItem>   m_stBonusList;
};

class CResponseFightClimbTowerResult : public CPackUserSyncData
{
public:
	CResponseFightClimbTowerResult() :
		m_iOrder(0), m_iCoinNum(0), m_stPerfectBonusList(), m_stFirstBonusList(), m_stClimbTowerInfo(), m_iTeamLevel(0), m_iTeamExp(0), m_stCardLevelExp() {}

	MSGPACK_DEFINE(m_stSyncData, m_iOrder, m_iCoinNum, m_stPerfectBonusList, m_stFirstBonusList, m_stClimbTowerInfo
		, m_iTeamLevel, m_iTeamExp, m_stCardLevelExp);

	int		m_iOrder;
	int		m_iCoinNum;
	std::vector<CPackGameItem>      m_stPerfectBonusList;
	std::vector<CPackGameItem>      m_stFirstBonusList;
	CPackProtoClimbTower				m_stClimbTowerInfo;
	int													m_iTeamLevel;
	int													m_iTeamExp;
	std::vector<std::pair<int, int> >					m_stCardLevelExp;
};

//RESET_CLIMB_TOWER_ORDER = 10176,	//重置镇魂之塔进度
class CResponseResetClimbTowerOrder : public CPackUserSyncData
{
public:
	CResponseResetClimbTowerOrder() :
		m_iResetClimbTowerCount(0), m_iClimbTowerCurrentOrder(0) {}

	MSGPACK_DEFINE(m_stSyncData, m_iResetClimbTowerCount, m_iClimbTowerCurrentOrder);

	int		m_iResetClimbTowerCount;
	int     m_iClimbTowerCurrentOrder;
};

//FIGHT_CLIMB_TOWER_START = 10177,	//开始镇魂之塔战斗
class CRequestFightClimbTowerStart
{
public:
	CRequestFightClimbTowerStart() : m_iOrder(0) {}

	MSGPACK_DEFINE(m_iOrder);

	int             m_iOrder;
};

class CResponseFightClimbTowerStart : public CPackUserSyncData
{
public:
	CResponseFightClimbTowerStart() : m_stBonusList() {}

	MSGPACK_DEFINE(m_stSyncData, m_stBonusList);

	std::vector<CPackGameItem>       m_stBonusList;
};

//GET_CLIMB_TOWER_RANK_LIST = 10179,	//获取爬塔排行列表
class CRequestGetClimbTowerRankList
{
public:
	CRequestGetClimbTowerRankList() : m_iRankAmount(0) {}

	MSGPACK_DEFINE(m_iRankAmount);

	int             m_iRankAmount;
};

class CPackClimbTowerRankListData
{
public:
	CPackClimbTowerRankListData()
		: m_strNick(""), m_iLevel(0), m_iTeamScore(0), m_iTotalCombat(0),
		m_iLeaderCardID(0), m_iAvatarIcon(0), m_iAvatarBorder(0), m_stCardInfoList(), m_iUid(0), m_iGroupID(0), m_strGuildName(){}

	MSGPACK_DEFINE(m_strNick, m_iLevel, m_iTeamScore, m_iTotalCombat,
		m_iLeaderCardID, m_iAvatarIcon, m_iAvatarBorder, m_stCardInfoList, m_iUid, m_iGroupID, m_strGuildName);

	std::string                                         m_strNick;
	int                                                 m_iLevel;
	int                                                 m_iTeamScore;
	int                                                 m_iTotalCombat;
	int                                                 m_iLeaderCardID;
	int                                                 m_iAvatarIcon;
	int                                                 m_iAvatarBorder;
	std::vector<CPackRankBaseCardInfo>    m_stCardInfoList;
	int32_t m_iUid;
	int32_t m_iGroupID;
	std::string m_strGuildName;
};

class CResponseGetClimbTowerRankList : public CPackUserSyncData
{
public:
	CResponseGetClimbTowerRankList() : m_stRankList(), m_iSelfRankNum(0), m_iSelfHighestClimbTowerRank(0) {}

	MSGPACK_DEFINE(m_stSyncData, m_stRankList, m_iSelfRankNum, m_iSelfHighestClimbTowerRank);

	std::vector<CPackClimbTowerRankListData>  m_stRankList;
	int32_t m_iSelfRankNum;
	int32_t m_iSelfHighestClimbTowerRank;
};

//GET_CLIMB_TOWER_SELF_RANK = 10180,	//获取爬塔自己排行
class CResponseGetClimbTowerSelfRank : public CPackUserSyncData
{
public:
	CResponseGetClimbTowerSelfRank() : m_iRankNum(0), m_iHighestClimbTowerRank(0) {}

	MSGPACK_DEFINE(m_stSyncData, m_iRankNum, m_iHighestClimbTowerRank);

	int             m_iRankNum;
	int             m_iHighestClimbTowerRank;
};

//SWEEP_CLIMB_TOWER_START = 10181,	//扫荡镇魂之塔开始
class CResponseSweepClimbTowerStart : public CPackUserSyncData
{
public:
	CResponseSweepClimbTowerStart() : m_iSweepEndTime(0) {}

	MSGPACK_DEFINE(m_stSyncData, m_iSweepEndTime);

	int		m_iSweepEndTime;
};

//SWEEP_CLIMB_TOWER_AWARD = 10182,	//扫荡镇魂之塔领奖
class CRequestSweepClimbTowerAward
{
public:
	CRequestSweepClimbTowerAward() : m_iCostYuanBao(0) {}

	MSGPACK_DEFINE(m_iCostYuanBao);

	int			m_iCostYuanBao;
};

class CResponseSweepClimbTowerAward : public CPackUserSyncData
{
public:
	CResponseSweepClimbTowerAward() :
		m_iTeamLevel(0), m_iTeamExp(0), m_stBonusList(), m_stCardLevelExp(), m_stClimbTowerInfo() {}

	MSGPACK_DEFINE(m_stSyncData,
		m_iTeamLevel,
		m_iTeamExp,
		m_stBonusList,
		m_stCardLevelExp,
		m_stClimbTowerInfo);

	int                                                     m_iTeamLevel;
	int                                                     m_iTeamExp;
	std::vector<CPackGameItem>			m_stBonusList;
	std::vector<std::pair<int, int>>                        m_stCardLevelExp;
	CPackProtoClimbTower					m_stClimbTowerInfo;
};

//SWEEP_CLIMB_TOWER_YUANBAO = 10183,	//扫荡镇魂之塔完成需要的元宝
class CResponseSweepClimbTowerYuanbao : public CPackUserSyncData
{
public:
	CResponseSweepClimbTowerYuanbao() :
		m_iCostYuanBao(0) {}

	MSGPACK_DEFINE(m_stSyncData, m_iCostYuanBao);

	int			m_iCostYuanBao;
};

class CResponseZhuHaiGetInfo : public CPackUserSyncData
{
public:
    CResponseZhuHaiGetInfo() : m_iLevelID(0), m_iGress(0), m_stBuffVec(), m_stCardHp(),m_stBless(),m_stRoleBless(),m_bSaveTeam(0) {}

    MSGPACK_DEFINE(m_stSyncData, m_iLevelID, m_iGress, m_stBuffVec, m_stCardHp,m_stBless,m_stRoleBless,m_bSaveTeam);

    int32_t     m_iLevelID;
    int32_t     m_iGress;           // 当前进度，进度为0，可以调整阵容，大于0后不可调整阵容，奖励发放完后，会设置为负数做标记
    std::vector<int> m_stBuffVec;
    std::map<int, int> m_stCardHp;
    std::map<int,CPackZhuHaiBlessInfo> m_stBless;
    std::map<int,std::vector<int>> m_stRoleBless;    //角色祝福
    char        m_bSaveTeam;
};

class CRequestZhuHaiChangeLevel
{
public:
    CRequestZhuHaiChangeLevel() : m_iLevelID(0) {}

    MSGPACK_DEFINE(m_iLevelID);

    int32_t     m_iLevelID;         // 发0表示重置，否则发选择的难度副本ID
};

class CRequestZhuHaiUpdateGress
{
public:
    CRequestZhuHaiUpdateGress() : m_iGress(0), m_stCardHp() {}

    MSGPACK_DEFINE(m_iGress, m_stCardHp);

    int32_t     m_iGress;
    std::map<int, int> m_stCardHp;
};

class CRequestZhuHaiAddBuff
{
public:
    CRequestZhuHaiAddBuff() : m_iBuffID(0),m_iReplaceBuffID(0){}

    MSGPACK_DEFINE(m_iBuffID,m_iReplaceBuffID);

    int32_t     m_iBuffID;
    int32_t     m_iReplaceBuffID;
};

class CRequestZhuHaiSetBless
{
public:
    CRequestZhuHaiSetBless():m_iPos(0),m_iBlessID(0),m_iBackBlessID(0){}

    MSGPACK_DEFINE(m_iPos, m_iBlessID,m_iBackBlessID);

    int32_t m_iPos;         //替换的祝福位置
    int32_t m_iBlessID;     //主祝福id
    int32_t m_iBackBlessID; //副祝福id
};

class CRequestZhuHaiBuyStore
{
public:
    CRequestZhuHaiBuyStore():m_iStoreID(0),m_iIndex(0),m_iReplaceBlessPos(0),m_iCardID(0),m_iReplaceBuffID(0){}

    MSGPACK_DEFINE(m_iStoreID, m_iIndex,m_iReplaceBlessPos,m_iCardID,m_iReplaceBuffID);

    int32_t  m_iStoreID;    //商品id
    int32_t  m_iIndex;      //商品Index
    int32_t  m_iReplaceBlessPos; //替换的祝福位置（购买的只能替换副祝福）
    int32_t  m_iCardID;     //如果是角色Buff,需要发送绑定的角色id
    int32_t  m_iReplaceBuffID;  //替换的BuffID
};

class CResponseZhuHaiBuyStore:public CPackUserSyncData
{
public:
    CResponseZhuHaiBuyStore():m_iStoreID(0),m_iIndex(0),m_iCardID(0),m_iReplaceBlessPos(0),m_iReplaceBuffID(0),m_stItemData(){}

    MSGPACK_DEFINE(m_stSyncData,m_iStoreID, m_iIndex,m_iCardID,m_iReplaceBlessPos,m_iReplaceBuffID,m_stItemData);

    int32_t m_iStoreID;
    int32_t m_iIndex;
    int32_t m_iCardID;
    int32_t m_iReplaceBlessPos;
    int32_t m_iReplaceBuffID;
    CPackTotalItemData  m_stItemData;
};


class CRequestZhuHaiRefreshStore
{
public:
    CRequestZhuHaiRefreshStore():m_bReset(false){}

    MSGPACK_DEFINE(m_bReset);

    bool m_bReset;
};

class CResponseZhuHaiRefreshStore:public CPackUserSyncData
{
public:
    CResponseZhuHaiRefreshStore():m_iCurRefreshCount(0){}

    MSGPACK_DEFINE(m_stSyncData,m_iCurRefreshCount);

    int32_t m_iCurRefreshCount; //当前的刷新次数
};


