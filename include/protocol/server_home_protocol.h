#pragma once

#include "msgpack.hpp"

#include "server_base_protocol.h"
#include "server_common_protocol.h"

class CPackHomeElem
{
public:
    CPackHomeElem() : m_iHomeID(0), m_iHomeType(0), m_iPos(0), m_iPageID(0), m_iFloorPageID(0), m_stHeroVec() {}

    MSGPACK_DEFINE(m_iHomeID, m_iHomeType, m_iPos, m_iPageID, m_iFloorPageID, m_stHeroVec);

    int32_t     m_iHomeID;
    int32_t     m_iHomeType;
    int32_t     m_iPos;
    int32_t     m_iPageID;
    int32_t     m_iFloorPageID;
    std::map<int32_t, CPackHomeHeroInfo> m_stHeroVec;
};





class CPackHomeFootclothElem
{
public:
    CPackHomeFootclothElem() : m_iFootclothID(0), m_stPosVec() {}

    MSGPACK_DEFINE(m_iFootclothID, m_stPosVec);

    int32_t     m_iFootclothID;
    std::map<int32_t, char> m_stPosVec;
};

class CPackHomeGardenElem
{
public:
    CPackHomeGardenElem() : m_iLandID(0), m_iSeedID(0), m_iSeedStat(0), m_iStatTime(0) {}

    MSGPACK_DEFINE(m_iLandID, m_iSeedID, m_iSeedStat, m_iStatTime);

    int32_t     m_iLandID;          // 地块ID
    int32_t     m_iSeedID;          // 种子ID
    int32_t     m_iSeedStat;        // 种植状态
    int32_t     m_iStatTime;        // 当前状态时间
};

//homeID=0,代表离开当前拜访家园
class CRequestHomeVisit
{
public:
    CRequestHomeVisit() : m_iUid(0), m_iGroupID(0), m_iHomeID(0) ,m_iCardID(0) , m_iSkinID(0),m_strNickName(){}

    MSGPACK_DEFINE(m_iUid, m_iGroupID, m_iHomeID,m_iCardID,m_iSkinID,m_strNickName);

    int32_t         m_iUid;
    int32_t         m_iGroupID;
    int32_t         m_iHomeID;
    int32_t         m_iCardID;
    int32_t         m_iSkinID;
    std::string     m_strNickName;
};

class CPackHotSpringUserInfo
{
public:
    CPackHotSpringUserInfo():m_iCardID(),m_iEndTime(0),m_iSkinID(0),m_stBrief(){}

    MSGPACK_DEFINE(m_iCardID, m_iEndTime,m_iSkinID,m_stBrief);

    int32_t m_iCardID;
    int32_t m_iEndTime;
    int32_t m_iSkinID;
    CPackUserBrief m_stBrief;
};


class CRequestHomeSaveHomeTopic
{
public:
    CRequestHomeSaveHomeTopic():m_stTopicElem(){};

    MSGPACK_DEFINE(m_stTopicElem)

    CPackHomeTopicElem m_stTopicElem;
};

class CResponseHomeSaveHomeTopic: public CPackUserSyncData
{
public:
    CResponseHomeSaveHomeTopic():m_stTopicElem(){};

    MSGPACK_DEFINE(m_stSyncData,m_stTopicElem)

    CPackHomeTopicElem m_stTopicElem;
};


class CResponseGetHomeInfo : public CPackUserSyncData
{
public:
    CResponseGetHomeInfo() : m_stHomeVec(), m_stFishVec(), m_iFishingRod(0), m_stGardenMap(), m_stEquipGoodsMap(), m_stFootclothMap(), m_stHomeBgm(),m_stUid2HomeVisit(),m_iFishingTimes(0),m_stHomeTopicInfo(),m_stHomeGoodBuff(){}

    MSGPACK_DEFINE(m_stSyncData, m_stHomeVec, m_stFishVec, m_iFishingRod, m_stGardenMap, m_stEquipGoodsMap, m_stFootclothMap, m_stHomeBgm,m_stUid2HomeVisit,m_iFishingTimes,m_stHomeTopicInfo,m_stHomeGoodBuff);

    std::vector<CPackHomeElem>          m_stHomeVec;            // 所有房间的信息
    std::vector<CPackHomeFishElem>      m_stFishVec;            // 鱼背包
    int32_t                             m_iFishingRod;          // 鱼竿
    std::map<int, std::vector<CPackHomeGardenElem>> m_stGardenMap;
    std::map<int, std::vector<CPackHomeGoodsElem>>  m_stEquipGoodsMap;
    std::map<int, std::vector<CPackHomeFootclothElem>> m_stFootclothMap;
    std::list<int32_t>                  m_stHomeBgm;
    std::unordered_map<int32_t,CRequestHomeVisit> m_stUid2HomeVisit;    //各个房间的拜访信息
    int32_t m_iFishingTimes;
    std::vector<CPackHomeTopicElem> m_stHomeTopicInfo;
    std::map<int32_t,CPackHomeGoodsBuffEffect> m_stHomeGoodBuff;
};

class CRequestHomeEnter
{
public:
    CRequestHomeEnter() : m_iHomeID(0) {}

    MSGPACK_DEFINE(m_iHomeID);

    int32_t     m_iHomeID;
};

class CResponseHomeEnter : public CPackUserSyncData
{
public:
    CResponseHomeEnter() : m_stHomeInfo(), m_stEquipGoodsVec(), m_stFootclothVec() {}

    MSGPACK_DEFINE(m_stSyncData, m_stHomeInfo, m_stEquipGoodsVec, m_stFootclothVec);

    CPackHomeElem                       m_stHomeInfo;           // 当前房间信息
    std::vector<CPackHomeGoodsElem>     m_stEquipGoodsVec;
    std::vector<CPackHomeFootclothElem> m_stFootclothVec;
};

class CRequestHomeUnequipGoods
{
public:
    CRequestHomeUnequipGoods() : m_iHomeID(0), m_iItemID(0), m_iPara(0),m_iType(0) {}

    MSGPACK_DEFINE(m_iHomeID, m_iItemID, m_iPara, m_iType);

    int32_t     m_iHomeID;
    int32_t     m_iItemID;
    int32_t     m_iPara;            // 卸下地毯时需要传递位置参数(x * 100 + y), 卸下家具时需要传递家具index
    int32_t     m_iType;
};

class CResponseHomeUnequipGoods : public CPackUserSyncData
{
public:
    CResponseHomeUnequipGoods() : m_iHomeID(0), m_iItemID(0), m_iPara(0) {}

    MSGPACK_DEFINE(m_stSyncData, m_iHomeID, m_iItemID, m_iPara);

    int32_t         m_iHomeID;
    int32_t         m_iItemID;
    int32_t         m_iPara;
};

class CRequestHomeEquipGoods
{
public:
    CRequestHomeEquipGoods() : m_iHomeID(0), m_stGoodsPara(),m_iType(0) {}

    MSGPACK_DEFINE(m_iHomeID, m_stGoodsPara,m_iType);

    int32_t             m_iHomeID;
    CPackHomeGoodsElem  m_stGoodsPara;
    int32_t             m_iType;
};

class CResponseHomeEquipGoods : public CPackUserSyncData
{
public:
    CResponseHomeEquipGoods() : m_iHomeID(0), m_stGoods(){}

    MSGPACK_DEFINE(m_stSyncData, m_iHomeID, m_stGoods);

    int32_t             m_iHomeID;
    CPackHomeGoodsElem  m_stGoods;
};

class CRequestHomeBuild
{
public:
    CRequestHomeBuild() : m_iHomeType(0), m_iPos(0) {}

    MSGPACK_DEFINE(m_iHomeType, m_iPos);

    int32_t     m_iHomeType;
    int32_t     m_iPos;
};

class CResponseHomeBuild : public CPackUserSyncData
{
public:
    CResponseHomeBuild() : m_stHomeInfo() {}

    MSGPACK_DEFINE(m_stSyncData, m_stHomeInfo);

    CPackHomeElem m_stHomeInfo;
};

class CRequestHomeChangeHero
{
public:
    CRequestHomeChangeHero() : m_iHomeID(0), m_iCardID(0) {}

    MSGPACK_DEFINE(m_iHomeID, m_iCardID);

    int32_t     m_iHomeID;
    int32_t     m_iCardID;
};

class CResponseHomeChangeHero : public CPackUserSyncData
{
public:
    CResponseHomeChangeHero() : m_iHomeID(0), m_iCardID(0) {}

    MSGPACK_DEFINE(m_stSyncData, m_iHomeID, m_iCardID);

    int32_t     m_iHomeID;
    int32_t     m_iCardID;
};

class CRequestHomePutBackHero
{
public:
    CRequestHomePutBackHero() : m_iHomeID(0), m_iCardID(0) {}

    MSGPACK_DEFINE(m_iHomeID, m_iCardID);

    int32_t     m_iHomeID;
    int32_t     m_iCardID;
};

class CRequestHomeFishingBegin
{
public:
    CRequestHomeFishingBegin() : m_iFishpondID(0), m_iCardID(0) {}

    MSGPACK_DEFINE(m_iFishpondID, m_iCardID);

    int32_t     m_iFishpondID;
    int32_t     m_iCardID;
};

class CResponseHomeFishingBegin : public CPackUserSyncData
{
public:
    CResponseHomeFishingBegin() : m_iFishID(0), m_iValue(0), m_iFishingTimes(0) {}

    MSGPACK_DEFINE(m_stSyncData, m_iFishID, m_iValue, m_iFishingTimes);

    int32_t     m_iFishID;
    int32_t     m_iValue;           // 鱼的体型，0表示小鱼
    int32_t     m_iFishingTimes;    // 钓鱼次数
};

class CRequestHomeFishingEnd
{
public:
    enum
    {
        fish_failed = 0,    // 失败
        fish_success = 1,   // 钓起鱼
        bag_success = 2,    // 钓起宝箱
        fish_and_bag = 3,   // 钓起鱼和宝箱
    };

    CRequestHomeFishingEnd() : m_iFishpondID(0), m_iCardID(0), m_iResultType(0) {}

    MSGPACK_DEFINE(m_iFishpondID, m_iCardID, m_iResultType);

    int32_t     m_iFishpondID;
    int32_t     m_iCardID;
    int32_t     m_iResultType;      // enum
};

class CResponseHomeFishingEnd : public CPackUserSyncData
{
public:
    CResponseHomeFishingEnd() : m_iCardID(0), m_iAction(0), m_stFish(), m_stItemData() {}

    MSGPACK_DEFINE(m_stSyncData, m_iCardID, m_iAction, m_stFish, m_stItemData);

    int32_t     m_iCardID;
    int32_t     m_iAction;
    CPackHomeFishElem m_stFish;
    CPackTotalItemData m_stItemData;
};

class CRequestHomeFishingChangeRod
{
public:
    CRequestHomeFishingChangeRod() : m_iItemID(0) {}

    MSGPACK_DEFINE(m_iItemID);

    int32_t     m_iItemID;
};

class CRequestHomeCookBook
{
public:
    CRequestHomeCookBook() : m_iCookBookID(0), m_stConsumeFishIndex(), m_stConsumeFoodID() {}

    MSGPACK_DEFINE(m_iCookBookID, m_stConsumeFishIndex, m_stConsumeFoodID);

    int32_t             m_iCookBookID;
    std::vector<int>    m_stConsumeFishIndex;       // 鱼索引
    std::vector<int>    m_stConsumeFoodID;          // 食材道具ID
};

class CResponseHomeCookBook : public CPackUserSyncData
{
public:
    CResponseHomeCookBook() : m_stConsumeFishIndex(), m_stItemData() {}

    MSGPACK_DEFINE(m_stSyncData, m_stConsumeFishIndex, m_stItemData);

    std::vector<int>    m_stConsumeFishIndex;
    CPackTotalItemData  m_stItemData;
};

class CRequestHomeGoodsMove
{
public:
    CRequestHomeGoodsMove() : m_iHomeID(0), m_bFootcloth(false), m_iPara(0), m_iCurX(0), m_iCurY(0),
          m_iNewX(0), m_iNewY(0), m_cStat(0), m_cQuadrant(0) {}

    MSGPACK_DEFINE(m_iHomeID, m_bFootcloth, m_iPara, m_iCurX, m_iCurY, m_iNewX, m_iNewY, m_cStat, m_cQuadrant);

    int32_t     m_iHomeID;
    bool        m_bFootcloth;           // true: 地毯， false: 家具
    int32_t     m_iPara;                // 地毯ID / 家具索引
    int32_t     m_iCurX;
    int32_t     m_iCurY;

    int32_t     m_iNewX;
    int32_t     m_iNewY;
    char        m_cStat;
    char        m_cQuadrant;
};

class CPackHomeExploreInfo
{
public:
    CPackHomeExploreInfo() : m_iQuestID(0), m_iEndTime(0), m_stTeam(), m_cState(0), m_bIsCbt(false) {}

    MSGPACK_DEFINE(m_iQuestID, m_iEndTime, m_stTeam, m_cState, m_bIsCbt);

    int32_t     m_iQuestID;
    int32_t     m_iEndTime;
    CPackCardTeam m_stTeam;
    char        m_cState;
    bool        m_bIsCbt;
};

class CResponseHomeExploreInfo : public CPackUserSyncData
{
public:
    CResponseHomeExploreInfo() : m_stInfoVec() {}

    MSGPACK_DEFINE(m_stSyncData, m_stInfoVec);

    std::vector<CPackHomeExploreInfo> m_stInfoVec;
};

class CNotifyHomeExploreQuest : public CPackUserSyncData
{
public:
    CNotifyHomeExploreQuest() : m_stQuestInfo() {}

    MSGPACK_DEFINE(m_stSyncData, m_stQuestInfo);

    CPackHomeExploreInfo m_stQuestInfo;
};

class CRequestHomeExploreStart
{
public:
    CRequestHomeExploreStart() : m_iQuestID(0), m_stTeam() {}

    MSGPACK_DEFINE(m_iQuestID, m_stTeam);

    int32_t         m_iQuestID;
    CPackCardTeam   m_stTeam;
};

class CResponseHomeExploreStart : public CPackUserSyncData
{
public:
    CResponseHomeExploreStart() : m_stExploreInfo() {}

    MSGPACK_DEFINE(m_stSyncData, m_stExploreInfo);

    CPackHomeExploreInfo m_stExploreInfo;
};

class CRequestHomeExploreReward
{
public:
    CRequestHomeExploreReward() : m_iQuestID(0) {}

    MSGPACK_DEFINE(m_iQuestID);

    int32_t         m_iQuestID;
};

class CResponseHomeExploreReward : public CPackUserSyncData
{
public:
    CResponseHomeExploreReward() : m_iQuestID(0), m_stItemData() {}

    MSGPACK_DEFINE(m_stSyncData, m_iQuestID, m_stItemData);

    int32_t             m_iQuestID;
    CPackTotalItemData  m_stItemData;
};

class CRequestHomeGardenPlant
{
public:
    CRequestHomeGardenPlant() : m_iHomeID(0), m_iIndex(0), m_iSeedID(0) {}

    MSGPACK_DEFINE(m_iHomeID, m_iIndex, m_iSeedID);

    int32_t         m_iHomeID;
    int32_t         m_iIndex;
    int32_t         m_iSeedID;
};

class CResponseHomeGardenPlant : public CPackUserSyncData
{
public:
    CResponseHomeGardenPlant() : m_iHomeID(0), m_stPlant() {}

    MSGPACK_DEFINE(m_stSyncData, m_iHomeID, m_stPlant);

    int32_t         m_iHomeID;
    CPackHomeGardenElem m_stPlant;
};

class CRequestHomeGardenOp
{
public:
    CRequestHomeGardenOp() : m_iHomeID(0), m_iIndex(0) {}

    MSGPACK_DEFINE(m_iHomeID, m_iIndex);

    int32_t                      m_iHomeID;
    std::vector<int32_t>         m_iIndex;
};

class CResponseHomeGardenOp : public CPackUserSyncData
{
public:
    CResponseHomeGardenOp() : m_iHomeID(0), m_stRewards(),m_stSuccessIndex() {}

    MSGPACK_DEFINE(m_stSyncData, m_iHomeID, m_stRewards,m_stSuccessIndex);

    int32_t         m_iHomeID;
    std::vector<CPackGameItem> m_stRewards;
    std::vector<int32_t> m_stSuccessIndex;
};

class CPackGardenHeroData
{
public:
    CPackGardenHeroData() : m_iCardID(0), m_iAction(0), m_iPoint(0) {}

    MSGPACK_DEFINE(m_iCardID, m_iAction, m_iPoint);

    int32_t         m_iCardID;          // 伙伴ID
    int32_t         m_iAction;          // 已消耗体力
    int32_t         m_iPoint;           // 属性点数
};

class CPackGardenAutoOp
{
public:
    CPackGardenAutoOp() : m_iCardID(0), m_iLandID(0), m_iOp(0) {}

    MSGPACK_DEFINE(m_iCardID, m_iLandID, m_iOp);

    int32_t         m_iCardID;          // 伙伴ID
    int32_t         m_iLandID;          // 操作的地块
    int32_t         m_iOp;              // 操作类型
};

class CPackGardenUpdateElem
{
public:
    CPackGardenUpdateElem() : m_stPlantVec(), m_stCardVec(), m_stOpVec() {}

    MSGPACK_DEFINE(m_stPlantVec, m_stCardVec, m_stOpVec);

    std::vector<CPackHomeGardenElem> m_stPlantVec;
    std::vector<CPackGardenHeroData> m_stCardVec;
    std::vector<CPackGardenAutoOp> m_stOpVec;
};

class CResponseHomeGardenUpdate : public CPackUserSyncData
{
public:
    CResponseHomeGardenUpdate() : m_stUpdateData(), m_stRewards() {}

    MSGPACK_DEFINE(m_stSyncData, m_stUpdateData, m_stRewards);

    std::map<int32_t, CPackGardenUpdateElem> m_stUpdateData;
    std::vector<CPackGameItem> m_stRewards;
};



class CResponseHomeVisit : public CPackUserSyncData
{
public:
    CResponseHomeVisit() : m_iHomeID(0), m_stHomeInfoVec(), m_stEquipGoodsVec(), m_stFootclothVec(), m_stHomeBgm(), m_iBanquetTeamID(0) ,m_stUid2HomeVisit() {}

    MSGPACK_DEFINE(m_stSyncData, m_iHomeID, m_stHomeInfoVec, m_stEquipGoodsVec, m_stFootclothVec, m_stHomeBgm, m_iBanquetTeamID,m_stUid2HomeVisit);

    int32_t         m_iHomeID;
    std::vector<CPackHomeElem>          m_stHomeInfoVec;        // 当前房间信息
    std::vector<CPackHomeGoodsElem>     m_stEquipGoodsVec;
    std::vector<CPackHomeFootclothElem> m_stFootclothVec;
    std::list<int32_t>                  m_stHomeBgm;
    uint64_t        m_iBanquetTeamID;                           // 对方玩家当前主办的宴会
    std::unordered_map<int32_t,CRequestHomeVisit> m_stUid2HomeVisit;    //各个房间的拜访信息
};

class CResponseGetSimpleGameState : public CPackUserSyncData
{
public:
    CResponseGetSimpleGameState() : m_stSimpleGamePoint(), m_strFlyArrayRewardState(""), m_stFlyArrayReward() {}

    MSGPACK_DEFINE(m_stSyncData, m_stSimpleGamePoint, m_strFlyArrayRewardState, m_stFlyArrayReward);

    std::map<unsigned char, int32_t>    m_stSimpleGamePoint;        // 小游戏分数
    std::string                         m_strFlyArrayRewardState;   // 飞天大炮弹奖励获取状态
    std::vector<int32_t>                m_stFlyArrayReward;         // 飞天大炮弹奖励阵列分布
};

class CRequestPlaySimpleGame
{
public:
    CRequestPlaySimpleGame() : m_iGameID(0), m_iGamePara(0), m_bIsConsumeItem(false), m_stPosList() {}

    MSGPACK_DEFINE(m_iGameID, m_iGamePara, m_bIsConsumeItem, m_stPosList);

    int32_t     m_iGameID;
    int32_t     m_iGamePara;        // 积分类发送积分，胜负类 大于0胜利，等于0失败
    bool        m_bIsConsumeItem;   // 是否消耗游戏币
    std::vector<int32_t> m_stPosList;   // 飞天大炮弹阵列奖励
};

class CResponsePlaySimpleGame : public CPackUserSyncData
{
public:
    CResponsePlaySimpleGame() : m_iGameID(0), m_iGamePara(0), m_bIsConsumeItem(false), m_iGameTimes(0), m_strFlyArrayRewardState(""), m_stRewards() {}

    MSGPACK_DEFINE(m_stSyncData, m_iGameID, m_iGamePara, m_bIsConsumeItem, m_iGameTimes, m_strFlyArrayRewardState, m_stRewards);

    int32_t     m_iGameID;
    int32_t     m_iGamePara;
    bool        m_bIsConsumeItem;   // 是否消耗游戏币
    int32_t     m_iGameTimes;               // 小游戏次数
    std::string m_strFlyArrayRewardState;   // 飞天大炮弹奖励获取状态
    std::vector<CPackGameItem> m_stRewards;     // 奖励
};


class CNotifyHomeVisit : public CPackUserSyncData
{
public:
    CNotifyHomeVisit(): m_iUid(0),m_iGroupID(0),m_iHomeID(0),m_iCardID(0),m_iSkinID(0),m_strNickName(){}

    MSGPACK_DEFINE(m_stSyncData, m_iUid,m_iGroupID,m_iHomeID,m_iCardID,m_iSkinID,m_strNickName)

    int32_t         m_iUid;
    int32_t         m_iGroupID;
    int32_t         m_iHomeID;
    int32_t         m_iCardID;
    int32_t         m_iSkinID;
    std::string     m_strNickName;
};

class CRequestHomeSave
{
public:
    CRequestHomeSave(): m_stUnEquipGoods(),m_stEquipGood(),m_stMoveGoods(){}

    MSGPACK_DEFINE(m_stUnEquipGoods, m_stEquipGood,m_stMoveGoods)

    std::vector<CRequestHomeUnequipGoods> m_stUnEquipGoods;
    std::vector<CRequestHomeEquipGoods> m_stEquipGood;
    std::vector<CRequestHomeGoodsMove> m_stMoveGoods;
};

class CResponseHomeSave: public CPackUserSyncData
{
    
public:
    CResponseHomeSave(): m_stUnEquipGoods(),m_stEquipGood(),m_stMoveEquipGoods(){}

    MSGPACK_DEFINE(m_stSyncData,m_stUnEquipGoods, m_stEquipGood,m_stMoveEquipGoods)

    std::vector<CResponseHomeUnequipGoods> m_stUnEquipGoods;
    std::vector<CResponseHomeEquipGoods> m_stEquipGood;
    std::vector<CRequestHomeGoodsMove> m_stMoveEquipGoods;
};

class CResponseHomeLevelInfo: public CPackUserSyncData
{
public:
    CResponseHomeLevelInfo(): m_iLevelExp(),m_stLevelRewardStatus(){}

    MSGPACK_DEFINE(m_stSyncData,m_iLevelExp, m_stLevelRewardStatus)

    int32_t m_iLevelExp;
    std::vector<int32_t> m_stLevelRewardStatus;
};

class CRequestHomeGetLevelReward
{
public:
    CRequestHomeGetLevelReward(): m_stGetLevel(){}

    MSGPACK_DEFINE(m_stGetLevel)

    std::vector<int32_t> m_stGetLevel;

};

class CResponseHomeGetLevelReward: public CPackUserSyncData
{
public:
    CResponseHomeGetLevelReward(): m_stGetReward(){}

    MSGPACK_DEFINE(m_stSyncData,m_stGetReward)

    CPackTotalItemData      m_stGetReward;
};

class CNotifyHomeLevelExp: public CPackUserSyncData
{
public:

    CNotifyHomeLevelExp(): m_iExpLevel(),m_iHomeItemID() {}

    MSGPACK_DEFINE(m_stSyncData,m_iExpLevel,m_iHomeItemID)

    int32_t m_iExpLevel;
    int32_t m_iHomeItemID;
};


class CNotifyHomeBuff: public CPackUserSyncData
{
public:

    CNotifyHomeBuff(): m_iHomeItem(){}

    MSGPACK_DEFINE(m_stSyncData,m_iHomeItem)

    CPackGameItem m_iHomeItem;
};

class CResponseHomeHotSpringInfo: public CPackUserSyncData
{
public:
    CResponseHomeHotSpringInfo(): m_bReward(false),m_bOtherReward(false),m_bJoinOtherReward(false),m_stFirstHotSpringInfo(),m_stHotSpringInfo(){}

    MSGPACK_DEFINE(m_stSyncData,m_bReward,m_bOtherReward,m_bJoinOtherReward,m_stFirstHotSpringInfo,m_stHotSpringInfo)

    bool m_bReward;
    bool m_bOtherReward;
    bool m_bJoinOtherReward;
    CPackHotSpringUserInfo     m_stFirstHotSpringInfo;                  //好友信息
    std::vector<CPackHotSpringUserInfo> m_stHotSpringInfo;              //温泉信息
};


class CRequestHomeHotSpringCreate
{
public:
    CRequestHomeHotSpringCreate(): m_iHomeID(0),m_iCardID(0),m_iSkinID(0){}

    MSGPACK_DEFINE(m_iHomeID,m_iCardID,m_iSkinID)

    int32_t m_iHomeID;
    int32_t m_iCardID;
    int32_t m_iSkinID;
};


class CResponseHomeHotSpringCreate: public CPackUserSyncData
{
public:
    CResponseHomeHotSpringCreate(): m_iHomeID(0),m_iCardID(0),m_iSkinID(0),m_iEndTime(0){}

    MSGPACK_DEFINE(m_stSyncData,m_iHomeID,m_iCardID,m_iSkinID,m_iEndTime)

    int32_t m_iHomeID;
    int32_t m_iCardID;
    int32_t m_iSkinID;
    int32_t m_iEndTime;
};


class CRequestHomeHotSpringInvite
{
public:

    struct userKey
    {
        MSGPACK_DEFINE(m_iUid,m_iGroupID)

        int32_t m_iUid;
        int32_t m_iGroupID;
    };

    CRequestHomeHotSpringInvite(): m_iHomeID(0),m_stInvitedUser(){}

    MSGPACK_DEFINE(m_iHomeID,m_stInvitedUser)

    int32_t m_iHomeID;
    std::vector<userKey> m_stInvitedUser;
};

//转发给各个被邀请人的信息
class CCrossHomeHotSpringInvite: public CPackUserSyncData
{
public:

    CCrossHomeHotSpringInvite(): m_iEndTime(0),m_iHomeID(0),m_stOwnerBrief(){}

    MSGPACK_DEFINE(m_stSyncData,m_iEndTime,m_iHomeID,m_stOwnerBrief)

    int32_t m_iEndTime;
    int32_t m_iHomeID;
    CPackUserBrief m_stOwnerBrief;

};

class CRequestHomeHotSpringJoin
{
public:

    CRequestHomeHotSpringJoin(): m_iUid(0),m_iGroupID(0),m_iCardID(0),m_iSkinID(0){}

    MSGPACK_DEFINE(m_iUid,m_iGroupID,m_iCardID,m_iSkinID)

    int32_t m_iUid;
    int32_t m_iGroupID;
    int32_t m_iCardID;
    int32_t m_iSkinID;
};

class CCrossHomeHotSpringJoin : public CPackUserSyncData
{
public:

    CCrossHomeHotSpringJoin(): m_iCardID(0),m_iEndTime(0),m_iUid(0),m_iGroupID(0),m_iSkinID(0),m_stUser(){}

    MSGPACK_DEFINE(m_stSyncData,m_iCardID,m_iEndTime,m_iUid,m_iGroupID,m_iSkinID,m_stUser)

    int32_t m_iCardID;
    int32_t m_iEndTime;
    int32_t m_iUid;
    int32_t m_iGroupID;
    int32_t m_iSkinID;
    CPackUserBrief m_stUser;
};

class CRequestHomeHotSpringGetReward
{
public:
    CRequestHomeHotSpringGetReward(): m_iUid(0),m_iGroupID(0),m_iCardID(0){}

    MSGPACK_DEFINE(m_iUid,m_iGroupID,m_iCardID)

    int32_t m_iUid;
    int32_t m_iGroupID;
    int32_t m_iCardID;

};


class CResponseHomeHotSpringGetReward: public CPackUserSyncData
{
public:
    CResponseHomeHotSpringGetReward(): m_iUid(0),m_iGroupID(0),m_iCardID(0),m_stHotSpringReward(){}

    MSGPACK_DEFINE(m_stSyncData,m_iUid,m_iGroupID,m_iCardID,m_stHotSpringReward)

    int32_t m_iUid;
    int32_t m_iGroupID;
    int32_t m_iCardID;
    CPackTotalItemData  m_stHotSpringReward;
};

class CResponseHomeHotSpringJoinOther: public CPackUserSyncData
{
public:
    CResponseHomeHotSpringJoinOther():m_iUid(0),m_iCardID(0),m_iSkinID(0),m_iRet(0){};

    MSGPACK_DEFINE(m_stSyncData,m_iUid,m_iCardID,m_iSkinID,m_iRet)

    int32_t m_iUid;
    int32_t m_iCardID;
    int32_t m_iSkinID;
    int32_t m_iRet;
};







