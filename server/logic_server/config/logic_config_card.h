#pragma once
#include "logic_config_base.h"
#include "server_base_protocol.h"


////////////////////////////////////////////////////////////////////////////////////////////////
struct TAddAttrElem
{
	TAddAttrElem() : m_stAttrVec() {}

	std::vector<std::pair<int, int>> m_stAttrVec;
};

struct TAddAttrFloatElem
{
	TAddAttrFloatElem(float fHP = 0, float fATK = 0, float fDEF = 0, float fCRI = 0)
		: m_fHP(fHP), m_fATK(fATK), m_fDEF(fDEF) {}

	float     m_fHP;
	float     m_fATK;
	float     m_fDEF;
};

struct TCardTalentElem
{
    TCardTalentElem() : m_iMaxLevel(0), m_iConsumePoint(0), m_iConsumePoint1(0),m_iNeedPoint(0), m_iPreID1(0), m_iPreID2(0), m_stAttrVec() {}

    int32_t     m_iMaxLevel;
    int32_t     m_iConsumePoint;
    int32_t     m_iConsumePoint1;
    int32_t     m_iNeedPoint;
    int32_t     m_iPreID1;
    int32_t     m_iPreID2;
    std::vector<std::pair<int, int>> m_stAttrVec;
};

struct TLogicHeroCardConfigElem
{
	TLogicHeroCardConfigElem() : m_iHeroCardID(0), m_strHeroCardName(), m_iQuality(0), m_iInitStar(1), m_iInitColor(1),
          m_iInitGrade(0), m_iSoulID(0), m_iCardType(1), m_iSexual(1), m_stLevelUpAttr(), m_stStarUpAttr(), m_stRankUpAttr(),
          m_iIsShow(0), m_stTalentMap() {}

	//英雄卡ID
	int                                                         m_iHeroCardID;
	//英雄卡名
	std::string                                                 m_strHeroCardName;
	//英雄品质
	int															m_iQuality;
	//生成卡时的初始星星数
	int                                                         m_iInitStar;
	//生成卡时的初始颜色
	int                                                         m_iInitColor;
	//初始SSR阶级
	int                                                         m_iInitGrade;
    int                                                         m_iSoulID;                  // 碎片道具ID
	//卡的定位
	int                                                         m_iCardType;
	//卡的性别
	int															m_iSexual;

	TAddAttrFloatElem                                           m_stLevelUpAttr;
	std::vector<TAddAttrElem>                                   m_stStarUpAttr;
    // 伙伴进阶专有属性
	std::map<int, std::pair<int, int>>                          m_stRankUpAttr;

	int															m_iIsShow;
    std::unordered_map<int, TCardTalentElem>                    m_stTalentMap;              // 伙伴天赋
};

class CLogicConfigHeroCardParser : public CLogicConfigParserBase
{
public:
    enum
    {
        E_MaxTalentNum = 50,        // 伙伴最大天赋数量
    };

	CLogicConfigHeroCardParser() : CLogicConfigParserBase(), m_stHeroCardMap() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

	const TLogicHeroCardConfigElem* GetConfig(int iID) const;
    const TCardTalentElem* GetTalentConfig(int iCardID, int iTalentID) const;

	std::map<int32_t, TLogicHeroCardConfigElem>       m_stHeroCardMap;
};

///////////////////////////////////////////////////////////////////////////////////////////////
struct TLogicUpgradeHeroCardStarElem
{
	TLogicUpgradeHeroCardStarElem() : m_iNeedHeroSoul(0), m_iConsumeGold(0) {}

	int             m_iNeedHeroSoul;
	int             m_iConsumeGold;
};

struct TLogicCardGradeSoulElem
{
    TLogicCardGradeSoulElem() : m_iExchangeNum(0), m_iRepeatExchange(0) {}

    int             m_iExchangeNum;
    int             m_iRepeatExchange;
};

class CLogicConfigUpgradeHeroCardStarParser : public CLogicConfigParserBase
{
public:
	CLogicConfigUpgradeHeroCardStarParser() : CLogicConfigParserBase(), m_iSoulExchangeItem(0), m_stSoulExchange(), m_stHeroCardStarUpgradeConfig() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

	int UpgradeStarNeedSoul(int iStar) const;
	int UpgradeStarNeedGold(int iStar) const;

	int GetHeroRepeatExchange(int iGrade) const;
    std::string GetGradeName(int iGrade) const;
    int GetSoulExchangeNum(int iGrade) const;

    int32_t             m_iSoulExchangeItem;            // 碎片转换的道具
    std::vector<TLogicCardGradeSoulElem>        m_stSoulExchange;               // 碎片转换
	std::vector<TLogicUpgradeHeroCardStarElem>  m_stHeroCardStarUpgradeConfig;
    std::vector<std::string>                    m_stGradeNameList;
};

struct TLogicCardLevelElem
{
    TLogicCardLevelElem() : m_iCardExp(0) {}

    int32_t     m_iCardExp;
};

///////////////////////////////////////////////////////////////////////////////////////////////
class CLogicConfigUpgradeHeroCardLevelParser : public CLogicConfigParserBase
{
public:
	CLogicConfigUpgradeHeroCardLevelParser() : CLogicConfigParserBase(), m_stConfig() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

    const TLogicCardLevelElem* GetConfig(int32_t iLevel) const;
	std::pair<int, int> UpgradeCardLevel(int iCurrentRoleLevel, int iCurrentCardLevel, int iCurrentCardExp, int iAddExp) const;

	std::vector<TLogicCardLevelElem>        m_stConfig;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
struct TLogicUpgradeHeroCardColorConfigElem
{
	TLogicUpgradeHeroCardColorConfigElem() : m_iLevelLimit(0), m_iConsumeGold(0), m_stColorItemVec(), m_stItemAttrVec(), m_stAttrVec() {}

	int32_t     m_iLevelLimit;
    int32_t     m_iConsumeGold;
	std::vector<std::pair<int, int>> m_stColorItemVec;
	std::vector<std::pair<int, int>> m_stItemAttrVec;    // 道具加成属性
    std::vector<std::pair<int, int>> m_stAttrVec;        // 进阶加成属性
};

class CLogicConfigUpgradeHeroCardColorParser : public CLogicConfigParserBase
{
public:
	CLogicConfigUpgradeHeroCardColorParser() : CLogicConfigParserBase(), m_iUpgradeTalentNum(0), m_iUpgradeTalentNum1(0), m_stCommonTalent(),m_stConfig() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

	const TLogicUpgradeHeroCardColorConfigElem* GetConfig(int iCardType, int iColor) const;

    int32_t         m_iUpgradeTalentNum;
    int32_t         m_iUpgradeTalentNum1;
    CPackGameItem   m_stCommonTalent;

	std::map<int, std::map<int, TLogicUpgradeHeroCardColorConfigElem>> m_stConfig;
};

// 好感度道具表
struct TLogicHeartGiftElem
{
    int32_t         m_iHeartExp;        // 赠送可获得基础经验
};

// 传记馆
struct TLogicHeartHistoryElem
{
    TLogicHeartHistoryElem() : m_iUnlockHeartLevel(0), m_iUnlockStar(0), m_iUnlockDatingTimes(0), m_iContentNum(0), m_stRewards() {}

    int32_t     m_iUnlockHeartLevel;
    int32_t     m_iUnlockStar;
    int32_t     m_iUnlockDatingTimes;
    int32_t     m_iContentNum;
    std::vector<CPackGameItem> m_stRewards;
};

struct TLogicHeartLevelReward
{
    TLogicHeartLevelReward() : m_stDialogAward(), m_stLevelUpAward() {}

    std::vector<CPackGameItem>  m_stDialogAward;
    std::vector<CPackGameItem>  m_stLevelUpAward;
};

struct TLogicMainCardLevelAttr
{
    TLogicMainCardLevelAttr() : m_iExpValue(0), m_stAddAttr() {}

    int32_t     m_iExpValue;
    std::vector<std::pair<int32_t, double>> m_stAddAttr;
};

class CLogicConfigHeroHeartParser : public CLogicConfigParserBase
{
public:
    CLogicConfigHeroHeartParser() : CLogicConfigParserBase(), m_iDialogCardRandomNum(0) {}

    std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

    const TLogicHeartGiftElem*      GetGiftItemInfo(int32_t iItemID) const;
    const TLogicMainCardLevelAttr*  GetMainCardLevelConfig(int32_t iLevel) const;
    void                            ReCalcHeartLevelExp(int32_t& iLevel, int32_t& iExp) const;
    void                            ReCalcMainCardHeartLevelExp(int32_t& iLevel, int32_t& iExp) const;
    int32_t                         GetHeartGiftLimitNumByHeartLevel(int32_t iHeartLevel) const;

    int32_t                                         m_iDialogCardRandomNum; // 互动伙伴随机时选取最大好感度伙伴的数量
    std::vector<std::pair<int32_t, int32_t>>        m_stLevelList;  // 好感度等级经验表 m_stLevelList[好感度等级] = <升级所需经验，当前等级可赠送礼物数量>>
    std::map<int32_t, TLogicHeartLevelReward>       m_stLevelAward;   // 好感度伙伴等级奖励
    std::map<int32_t, TLogicHeartGiftElem>          m_stGifts;      // 好感度礼物信息  <道具ID， 礼物信息>
    std::map<int32_t, TLogicHeartHistoryElem>       m_stHistoryMap; // 好感度传记馆

    std::vector<TLogicMainCardLevelAttr>            m_stMainCardLevelList;  // 主角好感度等级经验表
};
