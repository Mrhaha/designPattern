#pragma once

#include "common_fmtstring.h"
#include "server_base_protocol.h"
#include "server_home_protocol.h"
#include "logic_config_base.h"
#include "logic_config_define.h"

struct TLogicHomeElem
{
    TLogicHomeElem() : m_bIsInit(false), m_bIsExtend(false), m_bIsCanDress(false), m_iLimitHeroNum(0), m_iInitID(0),
          m_iInitPageID(0), m_iInitFloorPageID(0), m_stBuildConsume() {}

    bool        m_bIsInit;
    bool        m_bIsExtend;
    bool        m_bIsCanDress;
    int         m_iLimitHeroNum;
    int         m_iInitID;
    int         m_iInitPageID;
    int         m_iInitFloorPageID;
    std::vector<int32_t> m_iInitGoodsVec;

    CPackGameItem m_stBuildConsume;
};

struct TLogicHomeGoodsElem
{
    TLogicHomeGoodsElem() : m_iLimitHomeType(0), m_iFuncType(0), m_iFuncPara(0) {}

    int         m_iLimitHomeType;
    int         m_iFuncType;
    int         m_iFuncPara;
};

struct TLogicHomeInitGoodsElem
{
    TLogicHomeInitGoodsElem() : m_iItemId(0),m_iInitPosX(0), m_iInitPosY(0),m_cStat(0),m_cQuadrant(0),m_iSeatId(0) {}

    int         m_iItemId;
    int         m_iInitPosX;
    int         m_iInitPosY;
    char        m_cStat;
    char        m_cQuadrant;
    int         m_iSeatId;

};

struct TLogicHomeGoodBuff
{
    TLogicHomeGoodBuff():m_iBuffID(0){};

    int32_t m_iBuffID;
};


class CLogicConfigHomeGoodsParser : public CLogicConfigParserBase
{
public:
    enum
    {
        E_MaxPosX = 16,
        E_MaxPosY = 16,
        E_MaxStat = 10,
        E_MaxHomeCardNum = 20,      // 单房间最大伙伴数量
        E_MaxFootclothNum = 256,    // 单房间最大地毯数量
        E_MaxHomeGoodsNum = 70,     // 单房间最大家具装饰数量

        E_UniqueIDBase = 100,       // 通过坐标计算ID的组合基数
    };

    CLogicConfigHomeGoodsParser() : m_iDefaultFishingRod(0), m_stHomeMap(), m_stHomeGoodsMap() {}

    std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

    const TLogicHomeElem* GetHomeConfig(int32_t iHomeType) const;
    const TLogicHomeGoodsElem* GetHomeGoodsConfig(int32_t iID) const;
    const TLogicHomeInitGoodsElem* GetHomeInitGoodsConfig(int32_t) const;

    static int32_t GetUniqueIDByPos(int32_t iX, int32_t iY) { return (iX * E_UniqueIDBase + iY); }
    static bool IsValidPos(int32_t iX, int32_t iY) { return (0 < iX && iX <= E_MaxPosX) && (0 < iY && iY <= E_MaxPosY); }
    static bool IsValidStat(char cStat) { return (-10 <= cStat && cStat <= E_MaxStat); }

    int32_t m_iDefaultFishingRod;
    std::map<int32_t, TLogicHomeElem>       m_stHomeMap;
    std::map<int32_t, TLogicHomeGoodsElem>  m_stHomeGoodsMap;
    std::map<int32_t, TLogicHomeInitGoodsElem> m_stHomeInitGoodsMap;
    std::unordered_map<int32_t,TLogicHomeGoodBuff> m_stHomeGoodBuffMap;
    std::unordered_map<int32_t,CPackHomeGoodsBuffEffect> m_stHomeGoodBuffEffectMap;
};

struct TLogicHomeFishElem
{
    TLogicHomeFishElem() : m_iValueMin(0), m_iValueMax(0), m_iItemID(0), m_stValueToGradeMap(), m_stSellPrice() {}

    int32_t     m_iValueMin;
    int32_t     m_iValueMax;
    int32_t     m_iItemID;
    std::map<int, char> m_stValueToGradeMap;
    std::vector<int> m_stSellPrice;
};

struct TLogicHomeFishpond
{
    TLogicHomeFishpond() : m_strBag(), m_iBagTimes(), m_stFishWeight(), m_stBigWeight() {}

    std::string         m_strBag;
    int32_t             m_iBagTimes;
    TLogicWeightElem    m_stFishWeight;
    std::map<int, int>  m_stBigWeight;
};

struct TLogicHomeCookBookElem
{
    TLogicHomeCookBookElem() : m_iUnlockItem(0), m_stNeedFish(), m_stFoodTypeVec(), m_stRewardGradeToItemID() {}

    int32_t                     m_iUnlockItem;
    std::vector<std::pair<int32_t, char>> m_stNeedFish;
    std::vector<std::pair<int32_t, char>> m_stFoodTypeVec;
    std::map<int, int>          m_stRewardGradeToItemID;
};

struct TLogicFishRodElem
{
    TLogicFishRodElem() : m_stRodFuncMap() {}

    std::multimap<int, std::pair<int, int>> m_stRodFuncMap;         // [type, <weight, fish_id>]
};

struct TLogicBanquetItemElem
{
    TLogicBanquetItemElem() : m_iBanquetTime(0), m_iRewardTime(0), m_stOwnerRewards(), m_stRewards() {}

    int32_t     m_iBanquetTime;
    int32_t     m_iRewardTime;
    std::vector<CPackGameItem> m_stOwnerRewards;
    std::vector<CPackGameItem> m_stRewards;
};

class CLogicConfigHomeFuncParser : public CLogicConfigParserBase
{
public:
    enum
    {
        E_MaxFishBag = 1000,            // 鱼背包最大数量
    };

    enum EnumRodFunType
    {
        ERFT_Invalid = 0,
        ERFT_BigFishWeight = 1,         // 大鱼概率
        ERFT_FishWeight = 2,            // 指定鱼概率
        ERFT_FishKingWeight = 3,        // 鱼王概率

        ERFT_Max,
    };
    IS_VALID_ENUM_TYPE(EnumRodFunType, ERFT);

public:
    CLogicConfigHomeFuncParser() : m_iHotSpringDuration(0),m_iFriendLimit(0),m_iHotSpringSelfReward(0),m_iHotSpringFriendReward(0),
          m_iHotSpringInvitedFriendReward(0),m_strHotSpringEmailSender(""),m_strHotSpringEmailContent(""),m_strHotSpringEmailTitle(""),
          m_iHotSpringEmailReward(0),m_fStartTime(0),m_fEndTime(0),m_stFishMap(),m_stFishpondMap(), m_stFishRodMap(), m_stCookBookMap(), m_stBanquetItemMap() {}

    std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

    const TLogicHomeFishElem* GetFishConfig(int iFishID) const;
    int GetFishSellPrice(int iFishID, int iGrade) const;
    const TLogicHomeFishpond* GetFishpondConfig(int iPos) const;
    const TLogicFishRodElem*  GetFishRodConfig(int iRodID) const;
    const TLogicHomeCookBookElem* GetCookBookConfig(int iCookID) const;
    const TLogicBanquetItemElem* GetBanquetItemConfig(int iItemID) const;

public:
    std::string     m_strBanquetMailSender;
    std::string     m_strBanquetMailTitle;
    std::string     m_strBanquetMailContent;
    std::string     m_strBanquetMailOtherContent;

    int32_t         m_iHotSpringDuration;
    int32_t         m_iFriendLimit;
    int32_t         m_iHotSpringSelfReward;
    int32_t         m_iHotSpringFriendReward;
    int32_t         m_iHotSpringInvitedFriendReward;
    //参与他人奖励邮件
    std::string     m_strHotSpringEmailSender;
    std::string     m_strHotSpringEmailContent;
    std::string     m_strHotSpringEmailTitle;
    //自己参与奖励邮件
    std::string     m_strHotSpringEmailSenderSelf;
    std::string     m_strHotSpringEmailContentSelf;
    std::string     m_strHotSpringEmailTitleSelf;
    //邀请他人参与奖励邮件
    std::string     m_strHotSpringEmailSenderOther;
    std::string     m_strHotSpringEmailContentOther;
    std::string     m_strHotSpringEmailTitleOther;

    int32_t         m_iHotSpringEmailReward;
    float           m_fStartTime;
    float           m_fEndTime;
    int32_t         m_iExpiredTimeDay;

    std::map<int32_t, TLogicHomeFishElem> m_stFishMap;
    std::map<int32_t, TLogicHomeFishpond> m_stFishpondMap;
    std::map<int32_t, TLogicFishRodElem>  m_stFishRodMap;

    std::unordered_map<int32_t, TLogicHomeCookBookElem> m_stCookBookMap;
    std::unordered_map<int32_t, TLogicBanquetItemElem> m_stBanquetItemMap;
};

struct TLogicExploreCardLimit
{
    TLogicExploreCardLimit() : m_iLimitType(0), m_iLimitPara(0), m_iLimitParaEx(0) {}

    int32_t     m_iLimitType;
    int32_t     m_iLimitPara;
    int32_t     m_iLimitParaEx;
};

struct TLogicExploreRecommend
{
    TLogicExploreRecommend() : m_iCardID(0), m_stRewards(), m_strCardBag() {}

    int32_t     m_iCardID;
    std::vector<CPackGameItem> m_stRewards;
    std::string m_strCardBag;
};

struct TLogicExploreQuestElem
{
    TLogicExploreQuestElem() : m_bIsCbt(false), m_iTime(0), m_iCardLevel(0), m_iCardGrade(0), m_iCardColor(0), m_stCardLimitVec(), m_stRecommendVec(), m_stRewards(), m_strCardBag() {}

    bool        m_bIsCbt;
    int32_t     m_iTime;
    int32_t     m_iCardLevel;
    int32_t     m_iCardGrade;
    int32_t     m_iCardColor;
    std::vector<TLogicExploreCardLimit> m_stCardLimitVec;
    std::vector<TLogicExploreRecommend> m_stRecommendVec;
    std::vector<CPackGameItem> m_stRewards;
    std::string m_strCardBag;
};

struct TLogicSeedStatElem
{
    TLogicSeedStatElem() : m_iTime(0), m_iNeedOp(0) {}

    int32_t     m_iTime;
    int32_t     m_iNeedOp;
};

struct TLogicGardenElem
{
    TLogicGardenElem() : m_stStatVec(), m_stRewards() {}

    std::vector<TLogicSeedStatElem> m_stStatVec;
    std::vector<CPackGameItem> m_stRewards;
};

class CLogicConfigHomeGardenExploreParser : public CLogicConfigParserBase
{
public:
    enum EnumGardenOp
    {
        EOT_Invalid = 0,
        EOT_Watering = 1,       // 浇水
        EOT_Fertilization = 2,  // 施肥
        EOT_Harvest = 3,        // 收获

        EOT_Max,
    };
    IS_VALID_ENUM_TYPE(EnumGardenOp, EOT);

    enum EnumExploreLimit
    {
        EEL_Invalid = 0,
        EEL_CardNum = 1,
        EEL_CardTypeNum = 2,
        EEL_CardID = 3,

        EEL_Max,
    };
    IS_VALID_ENUM_TYPE(EnumExploreLimit, EEL);

    enum EnumQusetState
    {
        EQS_CanAccept = 0,      // 可接受任务
        EQS_Accepted = 1,       // 已接受任务
        EQS_Received = 2,       // 已领取奖励
    };

    CLogicConfigHomeGardenExploreParser() : m_strGardenOpBag(""), m_iFarmlandNum(0), m_iOpConsumeAction(0),
          m_iOpRewardPoint(0), m_iExploreNumMin(0), m_iExploreNumMax(0), m_strExploreMailSender(), m_strExploreMailTitle(),
          m_strExploreMailContent(), m_iMaxExploreExp(0), m_stExploreLevelMap(), m_stExploreMap(), m_stExploreStarQuestMap(), m_stGardenMap() {}

    std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

    const TLogicExploreQuestElem* GetExploreQuestConfig(int32_t iID) const;
    const TLogicWeightElem* GetExploreLevelConfig(int32_t iExp) const;
    const TLogicWeightElem* GetExploreStarRandConfig(int32_t iStar) const;
    const TLogicGardenElem* GetGardenConfig(int32_t iID) const;

    std::string m_strGardenOpBag;
    int32_t     m_iFarmlandNum;
    int32_t     m_iOpConsumeAction;
    int32_t     m_iOpRewardPoint;
    int32_t     m_iExploreNumMin;
    int32_t     m_iExploreNumMax;
    std::string m_strExploreMailSender;
    std::string m_strExploreMailTitle;
    std::string m_strExploreMailContent;
    int32_t     m_iMaxExploreExp;
    std::map<int32_t, TLogicWeightElem> m_stExploreLevelMap;
    std::unordered_map<int32_t, TLogicExploreQuestElem> m_stExploreMap;         // 探索任务map
    std::unordered_map<int32_t, TLogicWeightElem> m_stExploreStarQuestMap;  // 探索任务星级对应任务  [star, <id, weight>]
    std::map<int32_t, TLogicGardenElem> m_stGardenMap;
};

struct TLogicDatingPlace
{
    TLogicDatingPlace() : m_ucDailyDatingTimes(0), m_iHeartLevelLimitMin(0), m_iHeartLevelLimitMax(0), m_iHeartExpMin(0), m_iHeartExpMax(0), m_stDatingItems() {}

    uint8_t     m_ucDailyDatingTimes;
    int32_t     m_iHeartLevelLimitMin;
    int32_t     m_iHeartLevelLimitMax;
    int32_t     m_iHeartExpMin;
    int32_t     m_iHeartExpMax;
    std::vector<int32_t>    m_stDatingItems;
};

// 印象分
struct TLogicHeroImpression
{
    TLogicHeroImpression(int32_t iImpressionType = 0, int32_t iImpressionMinNum = 0, int32_t iImpressionMaxNum = 0)
    	: m_iImpressionType(iImpressionType), m_iImpressionMinNum(iImpressionMinNum), m_iImpressionMaxNum(iImpressionMaxNum) {}

    int32_t m_iImpressionType;
    int32_t m_iImpressionMinNum;
    int32_t m_iImpressionMaxNum;
};

struct TLogicDatingItem
{
    TLogicDatingItem() : m_iHeartExp(0), m_stRewards(), m_stImpression() {}

    int32_t                     m_iHeartExp;
    std::vector<CPackGameItem>  m_stRewards;
    std::vector<TLogicHeroImpression> m_stImpression;
};

// 约会
class CLogicConfigHeroDatingParser : public CLogicConfigParserBase
{
public:
    CLogicConfigHeroDatingParser() : CLogicConfigParserBase(), m_iImpressionMaxNum(0) {}

    std::pair<bool, std::string> Load(const std::string& strFileName, const std::string& strXMLContent);

    const TLogicDatingPlace* GetDatingPlaceConfig(uint8_t ucPlaceID) const;
    const TLogicDatingItem* GetDatingItemConfig(int32_t iItemID) const;
    const std::vector<CPackGameItem>* GetDatingTimesReward(int32_t iCardID, int32_t iTimes) const;

    int32_t m_iImpressionMaxNum;    // 伙伴印象分最大值
    std::map<uint8_t, TLogicDatingPlace> m_stDatingPlaceMap;        // 约会地点配置
    std::map<int32_t, TLogicDatingItem> m_stDatingItemMap;          // 约会道具
    std::map<int32_t, std::map<int32_t, std::vector<CPackGameItem>>> m_stDatingTimesReward; // 约会指定次数奖励
};

struct TLogicDatingDialog
{
    TLogicDatingDialog() : m_stOptionImpression() {}

    std::map<int32_t, std::vector<TLogicHeroImpression>> m_stOptionImpression;
};

// 约会对话
class CLogicConfigHeroDatingDialogParser : public CLogicConfigParserBase
{
public:
    CLogicConfigHeroDatingDialogParser() : CLogicConfigParserBase() {}

    std::pair<bool, std::string> Load(const std::string& strFileName, const std::string& strXMLContent);

    const TLogicDatingDialog* GetConfig(int32_t iHeroID, int32_t iPlaceID, int32_t iDialogID) const;

    std::map<int32_t, std::map<int32_t, std::map<int32_t, TLogicDatingDialog>>> m_stDatingDialogMap;        // [伙伴ID，[地点ID，[对话ID，选项奖励内容]]]
};

struct TLogicSimpleGameInfo
{
    TLogicSimpleGameInfo() : m_iLevelLimit(0), m_iFreeTimes(0), m_iRewardGroupType(0), m_iRewardGroupID(0), m_bIsAddupReward(false), m_stConsumeItem() {}

    int32_t     m_iLevelLimit;
    int32_t     m_iFreeTimes;
    int32_t     m_iRewardGroupType;
    int32_t     m_iRewardGroupID;
    bool        m_bIsAddupReward;
    CPackGameItem m_stConsumeItem;
};

// 小游戏积分奖励
struct TLogicSimpleGamePointReward
{
    TLogicSimpleGamePointReward() : m_iPointMin(0), m_iPointMax(0), m_stRewards() {}

    int32_t     m_iPointMin;
    int32_t     m_iPointMax;
    std::vector<CPackGameItem> m_stRewards;
};

// 小游戏胜负奖励
struct TLogicSimpleGameVictoryReward
{
    TLogicSimpleGameVictoryReward() : m_stVictoryRewards(), m_stFailedRewards() {}

    std::vector<CPackGameItem> m_stVictoryRewards;
    std::vector<CPackGameItem> m_stFailedRewards;
};

// 小游戏
class CLogicConfigSimpleGameParser : public CLogicConfigParserBase
{
public:
    CLogicConfigSimpleGameParser() : CLogicConfigParserBase(), m_iFlyArrayRewardMaxNum(0) {}

    std::pair<bool, std::string> Load(const std::string& strFileName, const std::string& strXMLContent);

    const TLogicSimpleGameInfo* GetSimpleGameConfig(int32_t iGameID) const;
    const TLogicSimpleGameVictoryReward* GetVictoryRewardConfig(int32_t iGroupID) const;
    std::vector<CPackGameItem> GetGameReward(int32_t iGameID, int32_t iPara) const;

    int32_t     m_iFlyArrayRewardMaxNum;            // 飞天大炮阵列奖励的物品最大数量
    std::map<int32_t, TLogicSimpleGameInfo> m_stSimpleGameMap;
    std::map<int32_t, std::vector<TLogicSimpleGamePointReward>> m_stPointRewardMap;
    std::map<int32_t, TLogicSimpleGameVictoryReward> m_stVictoryRewardMap;
    std::vector<std::vector<CPackGameItem>> m_stFlyArrayRewardBag;
};


// 家园奖励配置
struct TLogicHomeLeveReward
{
    TLogicHomeLeveReward() : m_iLevel(0), m_iNeedExp(0),m_levelReward() {}

    int32_t m_iLevel;
    int32_t m_iNeedExp;
    std::vector<CPackGameItem> m_levelReward;
};

// 家园物品经验配置
struct TLogicHomeItemExp
{
    TLogicHomeItemExp() : m_itemID(0), m_iHomeExp(0) {}

    int32_t m_itemID;
    int32_t m_iHomeExp;
};

// 家园鱼经验配置
struct TLogicHomeFishExp
{
    TLogicHomeFishExp() : m_iFishID(0), m_iNeedGrade(0),m_iHomeExp(0) {}

    int32_t m_iFishID;
    int32_t m_iNeedGrade;
    int32_t m_iHomeExp;
};


// 家园经验
class CLogicConfigHomeLevelExpParser : public CLogicConfigParserBase
{
public:
    CLogicConfigHomeLevelExpParser() : CLogicConfigParserBase() {}

    std::pair<bool, std::string> Load(const std::string& strFileName, const std::string& strXMLContent);

    const TLogicHomeLeveReward* GetHomeLevelRewardConfig(int32_t level) const;


    std::map<int32_t,TLogicHomeLeveReward> m_stLevel2Reward;
    std::map<int32_t,TLogicHomeItemExp> m_stLevel2ItemExp;
    std::map<int32_t,TLogicHomeFishExp> m_stLevel2FishExp;

};


