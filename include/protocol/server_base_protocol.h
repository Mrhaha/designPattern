#pragma once

#include "msgpack.hpp"

typedef uint64_t VecBitmapSubtype;
typedef std::vector<VecBitmapSubtype> CVecBitmap;

class CPackUserSyncData
{
public:
    struct sync_data_t
    {
        sync_data_t() : m_iSyncGold(0), m_iSyncYuanBao(0), m_iSyncPhyPower(0), m_iSyncLastAutoAddTime(0) { }

        MSGPACK_DEFINE(m_iSyncGold, m_iSyncYuanBao, m_iSyncPhyPower, m_iSyncLastAutoAddTime);

        int         m_iSyncGold;
        int         m_iSyncYuanBao;
        int         m_iSyncPhyPower;
        int         m_iSyncLastAutoAddTime;
    };

    sync_data_t     m_stSyncData;
};

class CResponseNotifyClientLogout : public CPackUserSyncData
{
public:
    enum
    {
        ENCLT_Repeat_Login         = 1,    // 重复登录
        ENCLT_AntiAdd_TimeLimit    = 2,    // 防沉迷用户，可登陆时间段限制
        ENCLT_AntiAdd_OnlineTimeLimit = 3, // 防沉迷用户以及游客，在线时间限制
        ENCLT_JustLogout = 4,              // 仅仅是通知客户端主动断开，不做任何提示
    };

public:
    MSGPACK_DEFINE(m_stSyncData, m_iLogoutType);

    int32_t     m_iLogoutType;
};

class CResponseBody : public CPackUserSyncData
{
public:
	MSGPACK_DEFINE(m_stSyncData);
};

class CPackUserKey
{
public:
	MSGPACK_DEFINE(m_iUin, m_iGroupID);

	CPackUserKey(int32_t iUin = 0, int32_t iGroupID = 0) :
		m_iUin(iUin), m_iGroupID(iGroupID) {}

	int32_t m_iUin;
	int32_t m_iGroupID;
};

class CPackGameItem
{
public:
	MSGPACK_DEFINE(m_iItemType, m_iCardID, m_iNum);

	CPackGameItem(int iItemType = 0, int iCardID = 0, int iNum = 0) :
		m_iItemType(iItemType), m_iCardID(iCardID), m_iNum(iNum) {}

	int                     m_iItemType;
	int                     m_iCardID;
	int                     m_iNum;
};

class CPackGameItemRandomNum
{
public:
    MSGPACK_DEFINE(m_stGameItem, m_iNumRange);

    CPackGameItemRandomNum(): m_stGameItem(),m_iNumRange(){}

    CPackGameItem                    m_stGameItem;
    std::vector<int32_t>             m_iNumRange;
};
class CPackRandomGameItem
{
public:
    MSGPACK_DEFINE(m_iItemType, m_iCardID, m_iNum,m_iWeight,m_iRandom);

    CPackRandomGameItem(int iItemType = 0, int iCardID = 0, int iNum = 0 , int iWeight = 0) :
          m_iItemType(iItemType), m_iCardID(iCardID), m_iNum(iNum) ,m_iWeight(iWeight),m_iRandom(0){}

    int                     m_iItemType;
    int                     m_iCardID;
    int                     m_iNum;
    int                     m_iWeight;
    float                   m_iRandom;
};


typedef std::vector<CPackGameItem> CPackItemVec;

class CPackRankBaseCardInfo
{
public:
	CPackRankBaseCardInfo() : m_iCardID(0), m_iColor(0), m_iStar(0), m_iLevel(0) {}

	MSGPACK_DEFINE(m_iCardID, m_iColor, m_iStar, m_iLevel);

	int             m_iCardID;
	int             m_iColor;
	int             m_iStar;
	int             m_iLevel;
};

class CPackItemAttr
{
public:
    CPackItemAttr() : m_iItemID(0), m_iNextOpenTime(0), m_iOpenedCount(0){}

    MSGPACK_DEFINE(m_iItemID, m_iNextOpenTime, m_iOpenedCount)

    int32_t m_iItemID;
    int32_t m_iNextOpenTime;    //下一次可以打开时间
    int32_t m_iOpenedCount;     //已经打开次数
};

class CPackProtoClimbTower
{
public:
	CPackProtoClimbTower() :
		m_iCurrentOrder(0), m_iMaxOrder(0), m_iResetCount(0), m_iSweepEndTime(0) {}

	MSGPACK_DEFINE(m_iCurrentOrder, m_iMaxOrder, m_iResetCount, m_iSweepEndTime)

	int				m_iCurrentOrder;	//当前进度
	int				m_iMaxOrder;		//最大进度
	int				m_iResetCount;		//重置次数
	int				m_iSweepEndTime;	//扫荡结束时间
};

class CPackConstellationInfo;
class CPackEquipInfo;
class CPackUserBriefCard
{
public:
    CPackUserBriefCard() : m_iCardID(0), m_iStar(0), m_iLevel(0), m_iColor(0), m_iEquipFashionID(0),m_iPosition(0), m_stTalentMap(),m_stConstelVec(),m_stEquipVec() {}

	MSGPACK_DEFINE(m_iCardID, m_iStar, m_iLevel, m_iColor, m_iEquipFashionID, m_iPosition, m_stTalentMap,m_stConstelVec,m_stEquipVec);

	int32_t m_iCardID;
	int32_t m_iStar;
	int32_t m_iLevel;
	int32_t m_iColor;
    int32_t m_iEquipFashionID;
    int32_t m_iPosition;

    std::map<int32_t,int32_t> m_stTalentMap;            // 天赋列表
    std::vector<CPackConstellationInfo> m_stConstelVec; // 念写卡列表
    std::vector<CPackEquipInfo>         m_stEquipVec;   // 装备列表
};

class CPackFashionInfo
{
public:
	CPackFashionInfo() : m_iFashionID(0), m_iExpiredTime(0) {}

	MSGPACK_DEFINE(m_iFashionID, m_iExpiredTime);

	int32_t m_iFashionID;	//时装ID
	int32_t m_iExpiredTime;	//过期时间
};

class CPackAvatarInfo
{
public:
    CPackAvatarInfo() : m_iAvatarID(0), m_iAvatarType(0), m_iTime(0) {}

    MSGPACK_DEFINE(m_iAvatarID, m_iAvatarType, m_iTime);

    int32_t     m_iAvatarID;
    int32_t     m_iAvatarType;
    int32_t     m_iTime;
};

// 装备星纹
class CPackConstellationInfo
{
  public:
    CPackConstellationInfo() : m_iUniqueID(0), m_iGrade(0), m_iEquipmentTag(0), m_iSuit(0), m_iLevel(0), m_iExp(0)
            , m_iTotalExp(0), m_iEquipCardID(0), m_iIsLocked(0), m_iIsDiscard(0), m_iMainAttrType(0), m_stExAttr() {}

    MSGPACK_DEFINE(m_iUniqueID, m_iGrade, m_iEquipmentTag, m_iSuit, m_iLevel, m_iExp, m_iTotalExp, m_iEquipCardID
            , m_iIsLocked, m_iIsDiscard, m_iMainAttrType, m_stExAttr);

    int32_t     m_iUniqueID;
    int32_t     m_iGrade;
    int32_t     m_iEquipmentTag;
    int32_t     m_iSuit;
    int32_t     m_iLevel;
    int32_t     m_iExp;
    int32_t     m_iTotalExp;
    int32_t     m_iEquipCardID;
    int32_t     m_iIsLocked;
    int32_t     m_iIsDiscard;

    int32_t     m_iMainAttrType;
    std::map<int, std::pair<int, int>> m_stExAttr;
};

class CPackAvyInfo
{
public:
    CPackAvyInfo() : m_iAvyID(0), m_strCompleteBitmap() {}

    MSGPACK_DEFINE(m_iAvyID, m_strCompleteBitmap);

    int32_t         m_iAvyID;
    std::string     m_strCompleteBitmap;
};

// 伙伴装备
class CPackEquipInfo
{
public:
    CPackEquipInfo() : m_iUniqueID(0), m_iGrade(0), m_iEquipmentTag(0), m_iAttrIndex(0), m_iIsLocked(0) {}

    MSGPACK_DEFINE(m_iUniqueID, m_iGrade, m_iEquipmentTag, m_iAttrIndex, m_iIsLocked);

    int32_t     m_iUniqueID;
    int32_t     m_iGrade;
    int32_t     m_iEquipmentTag;
    int32_t     m_iAttrIndex;
    int32_t     m_iIsLocked;
};

// 礼装预设
class CPackConstelPresetInfo
{
public:
    CPackConstelPresetInfo() : m_iPresetID(0), m_strName(""), m_stConstelIDs() {}

    MSGPACK_DEFINE(m_iPresetID, m_strName, m_stConstelIDs);

    int32_t         m_iPresetID;
    std::string     m_strName;
    std::vector<int32_t> m_stConstelIDs;
};

class CPackQuestData
{
public:
    CPackQuestData() : m_iQuestID(0), m_iIndex(0), m_iProgress(0), m_cState(0) {}

    MSGPACK_DEFINE(m_iQuestID, m_iIndex, m_iProgress, m_cState);

    int32_t     m_iQuestID;
    int32_t     m_iIndex;
    int32_t     m_iProgress;
    char        m_cState;
};

class CPackBadgeData
{
public:
    CPackBadgeData() : m_iBadgeID(0), m_iProgress(0), m_iOwnedTime(0), m_stProSet(), m_bIsOwned(false) {}

    MSGPACK_DEFINE(m_iBadgeID, m_iProgress, m_iOwnedTime, m_stProSet, m_bIsOwned);

    int32_t     m_iBadgeID;
    int32_t     m_iProgress;
    int32_t     m_iOwnedTime;
    std::set<int> m_stProSet;
    bool        m_bIsOwned;
};

using CPackCardTeam = std::vector<int>;

class CPackTeamPreset
{
public:
    CPackTeamPreset() : m_iIndex(0), m_strName(), m_stTeam() {}

    MSGPACK_DEFINE(m_iIndex, m_strName, m_stTeam);

    int32_t         m_iIndex;
    std::string     m_strName;
    CPackCardTeam   m_stTeam;
};

class CPackLevelInfo
{
public:
    CPackLevelInfo() : m_iLevelID(0), m_cLevelStar(0), m_cSharedTimes(0), m_cPlayTimes(0), m_cBonusTag(0), m_cResetTimes(0), m_bIsPass(false), m_cAchieveTag(0), m_iCostTime(0) ,m_bLimitBoss(false),m_bAdvRewardTimes(0),m_bCrushBoss(false){}

    MSGPACK_DEFINE(m_iLevelID, m_cLevelStar, m_cSharedTimes, m_cPlayTimes, m_cBonusTag, m_cResetTimes, m_bIsPass, m_cAchieveTag, m_iCostTime,m_bLimitBoss,m_bAdvRewardTimes,m_bCrushBoss);

    int         m_iLevelID;
    int8_t      m_cLevelStar;               // 星数
    int8_t      m_cSharedTimes;             // 已共享次数
    int8_t      m_cPlayTimes;               // 该副本已打次数
    int8_t      m_cBonusTag;                // 副本奖励领取标记
    int8_t      m_cResetTimes;              // 已重置次数
    bool        m_bIsPass;                  // 是否已通关
    int8_t      m_cAchieveTag;              // 副本标记
    int32_t     m_iCostTime;                // 最快副本通关时间
    bool        m_bLimitBoss;               // 是否出现限时Boss
    int32_t     m_bAdvRewardTimes;          // 装备本高额奖励
    bool        m_bCrushBoss;               // 是否出现遭遇Boss
};

class CPackMarket
{
public:
	CPackMarket()
		: m_stCommodity(), m_stConsume(), m_bCanBuy(true), m_iDiscount(0) {}

	MSGPACK_DEFINE(m_stCommodity, m_stConsume, m_bCanBuy, m_iDiscount);

	CPackGameItem   m_stCommodity;
	CPackGameItem   m_stConsume;

	bool        m_bCanBuy;
	int         m_iDiscount;
};

class CPackUserBrief
{
public:
	MSGPACK_DEFINE(m_iUid, m_iGroupID, m_strNick, m_iLevel, m_iAvatarID, m_iAvatarBorder, m_bIsOnline, m_iLastLogoutTime, m_iCombat);

	int32_t m_iUid = 0;
	int32_t m_iGroupID = 0;
	std::string m_strNick;
    int32_t m_iLevel = 0;
	int32_t m_iAvatarID = 0;
	int32_t m_iAvatarBorder = 0;
    bool    m_bIsOnline = false;
    int32_t m_iLastLogoutTime = 0;
    int32_t m_iCombat = 0;
};

class CPackUserDetailInfo
{
public:
    CPackUserDetailInfo() : m_iGuildID(0), m_strGuildName(), m_stBriefInfo(), m_iTeamType(0), m_stTeamBrief(),m_bUnLockHome(false) {}

    MSGPACK_DEFINE(m_iGuildID, m_strGuildName, m_stBriefInfo, m_iTeamType, m_stTeamBrief,m_bUnLockHome);

    int32_t             m_iGuildID;
    std::string         m_strGuildName;
    CPackUserBrief      m_stBriefInfo;
    int32_t             m_iTeamType;
    std::vector<CPackUserBriefCard> m_stTeamBrief;
    bool                m_bUnLockHome;
};

class CPackUserMatch
{
public:
	MSGPACK_DEFINE(m_iUin, m_iGroupID, m_iMatchLevel, m_iPvpFightPower);

	int32_t m_iUin = 0;
	int32_t m_iGroupID = 0;
	int32_t m_iMatchLevel = 0;
	int32_t m_iPvpFightPower = 0;
};

class CPackUserRank
{
public:
	MSGPACK_DEFINE(m_iUid, m_iGroupID, m_iRank, m_strNick, m_iLevel, m_iAvatarIcon
		, m_iAvatarBorder, m_iRankScore, m_iFightPower, m_iVipLevel, m_iShowGroupID, m_strGroupName);

	int32_t m_iUid = 0;
	int32_t m_iGroupID = 0;
	int32_t m_iRank = 0;
	std::string m_strNick;
	int32_t m_iLevel = 0;
	int32_t m_iAvatarIcon = 0;
	int32_t m_iAvatarBorder = 0;
	int32_t m_iRankScore = 0;
	int32_t m_iFightPower = 0;
	int32_t m_iVipLevel = 0;
	int32_t m_iShowGroupID = 0;
	std::string m_strGroupName;
};

class CPackHonorInfo
{
public:
	MSGPACK_DEFINE(m_iHonorID, m_iHonorStatus, m_iExpireTime, m_iProgressVec);

	int32_t m_iHonorID = 0;				//称号ID
	int32_t m_iHonorStatus = 0;			//称号状态 0未解锁  1已解锁  2佩戴中
	int32_t m_iExpireTime = 0;			//过期时间，为0表示永久
	std::vector<int32_t> m_iProgressVec;//进度
};

class CPackPvpChallengerCardInfo
{
public:
	MSGPACK_DEFINE(m_iCardID, m_iColor, m_iStar, m_iLevel, m_iRobotSkillLevelIndex, m_iRobotDefPara, m_stTalent
                   , m_iEquipFashionID, m_iPosition, m_iSkillCopyCard, m_stConstelVec, m_stEquipVec);

	CPackPvpChallengerCardInfo() : m_iCardID(0), m_iColor(0), m_iStar(0), m_iLevel(1), m_iRobotSkillLevelIndex(0)
          , m_iRobotDefPara(0), m_stTalent(), m_iEquipFashionID(0), m_iPosition(0)
          , m_iSkillCopyCard(0), m_stConstelVec(), m_stEquipVec() {}

	int             m_iCardID;
	int             m_iColor;               // 机器人 atk_para
	int             m_iStar;                // 机器人 hp_para
	int             m_iLevel;               // 机器人 diffculty
	int             m_iRobotSkillLevelIndex;    // 机器人 skillLevelIndex
	int             m_iRobotDefPara;        // 机器人 def_para
	std::map<int, int> m_stTalent;
	int32_t         m_iEquipFashionID;
	int32_t         m_iPosition;
	int32_t         m_iSkillCopyCard;       // TODO
    std::vector<CPackConstellationInfo> m_stConstelVec; // 装备的星纹
    std::vector<CPackEquipInfo>         m_stEquipVec;   // 装备列表
};

class CPackPvpChallengerInfo
{
public:
	CPackPvpChallengerInfo() : m_stBriefInfo(), m_stCardInfoList() {}

	MSGPACK_DEFINE(m_stBriefInfo, m_stCardInfoList);

	CPackUserBrief                              m_stBriefInfo;
	std::vector<CPackPvpChallengerCardInfo>     m_stCardInfoList;
};

class CPackPvpUserInfo
{
public:
    CPackPvpUserInfo() : m_stInfo(), m_iCardID(0), m_iEquipFashionID(0), m_iPvpRank(0),m_bHasChallenge(false){}

    MSGPACK_DEFINE(m_stInfo, m_iCardID, m_iEquipFashionID, m_iPvpRank,m_bHasChallenge);

    CPackUserDetailInfo m_stInfo;
    int32_t     m_iCardID;
    int32_t     m_iEquipFashionID;      // 装备的时装
    int         m_iPvpRank;             // pvp积分
    bool        m_bHasChallenge;        // 是否战胜
};

class CPackHomeFishElem
{
public:
    CPackHomeFishElem() : m_iFishIndex(0), m_iFishID(0), m_iValue(0), m_cGrade(0) {}

    MSGPACK_DEFINE(m_iFishIndex, m_iFishID, m_iValue, m_cGrade);

    int32_t     m_iFishIndex;
    int32_t     m_iFishID;
    int32_t     m_iValue;
    char        m_cGrade;
};

class CPackTotalBonusData
{
public:
    CPackTotalBonusData() : m_stItems(), m_stConstelVec(), m_stEquipVec(), m_stFashionVec(), m_stAvatarVec() {}

    MSGPACK_DEFINE(m_stItems, m_stConstelVec, m_stEquipVec, m_stFashionVec, m_stAvatarVec);

    std::vector<CPackGameItem>          m_stItems;              // 道具
    std::vector<CPackConstellationInfo> m_stConstelVec;         // 礼装
    std::vector<CPackEquipInfo>         m_stEquipVec;           // 装备
    std::vector<CPackFashionInfo>       m_stFashionVec;         // 时装
    std::vector<CPackAvatarInfo>        m_stAvatarVec;          // 头像，头像框
};

class CPackTotalConsumeData
{
public:
    CPackTotalConsumeData() : m_stItems() {}

    MSGPACK_DEFINE(m_stItems);

    std::vector<CPackGameItem>          m_stItems;
};

// 道具综合数据
class CPackTotalItemData
{
public:
    CPackTotalItemData() : m_stBonus(), m_stConsume() {}

    MSGPACK_DEFINE(m_stBonus, m_stConsume);

    CPackTotalBonusData     m_stBonus;              // 奖励
    CPackTotalConsumeData   m_stConsume;            // 消耗
};

class CResponseCacheItemBody : public CPackUserSyncData
{
public:
    MSGPACK_DEFINE(m_stSyncData, m_stCacheItem);

    CPackTotalItemData  m_stCacheItem;
};

class CPackTalkInfo
{
public:
    CPackTalkInfo() : m_stUserBrief(), m_strTalkData(), m_iTalkChannel(0), m_iTalkTime(0), m_iDstUin(0) {}

    MSGPACK_DEFINE(m_stUserBrief, m_strTalkData, m_iTalkChannel, m_iTalkTime, m_iDstUin);

    CPackUserBrief      m_stUserBrief;              // 聊天发起人
    std::string         m_strTalkData;              // 聊天内容
    int32_t             m_iTalkChannel;
    int32_t             m_iTalkTime;                // 聊天时间
    int32_t             m_iDstUin;                  // 私聊目标玩家
};

class CPackBanquetTeamIds{

public:
    CPackBanquetTeamIds():m_iCreateId(0),m_iJoinId(0) {}

    MSGPACK_DEFINE(m_iCreateId, m_iJoinId);

    uint64_t m_iCreateId;
    uint64_t m_iJoinId;

};

using talk_cache_list = std::list<CPackTalkInfo>;

class CPackConstelShopInfo
{
public:
    CPackConstelShopInfo():m_iIndex(0),m_iHasBuy(false),m_iHalfPrice(false) {}

    MSGPACK_DEFINE(m_iIndex, m_iHasBuy,m_iHalfPrice);

    int32_t m_iIndex;
    bool    m_iHasBuy;
    bool    m_iHalfPrice;
};

class CPackGashaponInfo
{
public:
    CPackGashaponInfo():m_iIndex(0),m_iHasBuyCount(0){}

    MSGPACK_DEFINE(m_iIndex, m_iHasBuyCount);

    int32_t m_iIndex;
    int32_t m_iHasBuyCount;
};

class CPackHomeGoodsElem
{
public:
    CPackHomeGoodsElem() : m_iGoodsIndex(0), m_iGoodsID(0), m_iPosX(0), m_iPosY(0), m_cStat(0), m_cQuadrant(0),m_iSeatIndex(0) {}

    MSGPACK_DEFINE(m_iGoodsIndex, m_iGoodsID, m_iPosX, m_iPosY, m_cStat, m_cQuadrant,m_iSeatIndex);

    int32_t     m_iGoodsIndex;
    int32_t     m_iGoodsID;
    int32_t     m_iPosX;
    int32_t     m_iPosY;
    char        m_cStat;
    char        m_cQuadrant;
    int32_t     m_iSeatIndex;
};

class CPackHomeTopicElem
{
public:
    CPackHomeTopicElem():m_iTopicIndex(0),homeTopicName(""),wallID(0),floorID(0),m_stHomeGoods(){};

    MSGPACK_DEFINE(m_iTopicIndex,homeTopicName,wallID,floorID,m_stHomeGoods)

    int32_t m_iTopicIndex;
    std::string homeTopicName;

    int32_t wallID;
    int32_t floorID;
    std::vector<CPackHomeGoodsElem> m_stHomeGoods;
};




