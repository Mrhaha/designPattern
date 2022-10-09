#pragma once

#include "logic_config_base.h"
#include "server_base_protocol.h"
#include "common_fmtstring.h"
#include "logic_config_define.h"

struct TLogicItemConfig
{
    TLogicItemConfig() : m_iItemID(0), m_strName(""), m_cGrade(0), m_iSubType(0), m_iTypePara1(0), m_iTypePara2(0), m_iUseType(0),
          m_iUsePara1(0), m_iUsePara2(0), m_bAutoUse(false), m_iUniqueCount(0), m_stRepeatExchangeItem(), m_iFoodType(0), m_iSellPrice(0),m_iHasCountLimit(0),m_iHomeBagCountLimit(0),m_bIsNotShowInFight(false) {}

	int             	m_iItemID;
	std::string     	m_strName;
    char                m_cGrade;
    int                 m_iSubType;
    int                 m_iTypePara1;
    int                 m_iTypePara2;
	int32_t 			m_iUseType;
	int32_t 			m_iUsePara1;
    int32_t 			m_iUsePara2;
    bool                m_bAutoUse;
    int32_t             m_iUniqueCount;
    CPackGameItem       m_stRepeatExchangeItem;
    int                 m_iFoodType;            // 家园专用
    int             	m_iSellPrice;
    int                 m_iHasCountLimit;
    int                 m_iHomeBagCountLimit;   // 家园和背包总和限制
    bool                m_bIsNotShowInFight;    // 战斗中不显示（默认都是展示）
};

class CLogicConfigItemParser : public CLogicConfigParserBase
{
public:
    CLogicConfigItemParser() : CLogicConfigParserBase(), m_stConfigMap() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

	const TLogicItemConfig* GetConfig(int iID) const;

	std::unordered_map<int, TLogicItemConfig> m_stConfigMap;
};

///////////////////////////////////////////////////////////////////////////////////////////////
struct TLogicFashionConfigElem
{
	int32_t m_iFashionID;
	std::string m_strName;
	int32_t m_iBelongCardID;
    int32_t m_iYuanBao;
};

struct TLogicFashionItemConfigElem
{
	int32_t m_iItemID;
	int32_t m_iFashionID;
	int32_t m_iYuanbao;
	int32_t m_iDay;
};

///////////////////////////////////////////////////////////////////////////////////////////////
class CLogicConfigFashionParser : public CLogicConfigParserBase
{
public:
	CLogicConfigFashionParser() : m_strSendTitle(), m_stSendContent(), m_stFashionMap(), m_stFashionItemMap() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

    const TLogicFashionConfigElem* GetFashionConfig(int32_t iFashionID) const;

	std::string m_strSendTitle;
	CFormatString m_stSendContent;

	std::map<int32_t, TLogicFashionConfigElem> m_stFashionMap;
	std::map<int32_t, TLogicFashionItemConfigElem> m_stFashionItemMap;
};

////////////////////////////////////////////////////////////////////////////////
struct TLogicVipConfigElem
{
	TLogicVipConfigElem() :
		m_iNeedChargeAmount(0), m_iDoubleRechargeCount(0), m_iCanBuyActionCoinCount(0), m_iCanBuyGoldTimes(0),
		m_iCanBuyEnergyTimes(0), m_iCanBuyVitalityTimes(0), m_iCanBuyPVPTimes(0), m_iCanBuyRefreshResetCrossFightTimesTimes(0),
		m_iCanRefreshClimbTowerCount(0), m_stShakeGoldRate(), m_stRefreshLevelMap() {}

	int                 m_iNeedChargeAmount;
	int32_t				m_iDoubleRechargeCount;
	int32_t				m_iCanBuyActionCoinCount;
	int                 m_iCanBuyGoldTimes;
	int                 m_iCanBuyEnergyTimes;
	int                 m_iCanBuyVitalityTimes;
	int                 m_iCanBuyPVPTimes;
	int                 m_iCanBuyRefreshResetCrossFightTimesTimes;
	int					m_iCanRefreshClimbTowerCount;
	std::vector<int>    m_stShakeGoldRate;
	std::vector<CPackGameItem> m_stDailyAwardVec;
    std::map<int, int>  m_stRefreshLevelMap;
};

class CLogicConfigVipParser : public CLogicConfigParserBase
{
public:
	CLogicConfigVipParser() : m_stVipConfig() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

	int GetShakeGoldRate(int iVip) const;
	int GetVipLevel(int iVipExp) const;
	int GetVipExp(int iVipLevel) const;
	int GetMaxVipLevel() const;
	const TLogicVipConfigElem* GetVipInfo(int iVip) const;
    int GetRefreshLevelTimes(int iVipLevel, int iLevelType) const;

	std::vector<TLogicVipConfigElem>     m_stVipConfig;
};

////////////////////////////////////////////////////////////////////////////////
class CLogicConfigVipUnlockParser : public CLogicConfigParserBase
{
public:
	CLogicConfigVipUnlockParser() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

	int             m_iBuyGold;
	int             m_iBuyEnergy;
	int             m_iBuyVitality;
	int             m_iRefreshEliteRefresh;
	int             m_iRefreshPVP;
	int             m_iRuhuaBonus;
	int             m_iPaomaBonus;
	int             m_iAddRuhuaTime;
	int             m_iAddPaomaTime;
};

////////////////////////////////////////////////////////////////////////////////
struct TLogicAvatarElem
{
	int             m_iAvatarID;
	int             m_iAvatarType;
	int             m_iUnlockValue;
	bool            m_bIsFree;
};

class CLogicConfigAvatarParser : public CLogicConfigParserBase
{
public:
	CLogicConfigAvatarParser() : m_iDefaultIconID(0), m_iDefaultBorderID(0), m_stAvatarList() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

    const TLogicAvatarElem* GetConfig(int32_t iID) const;

	int                             m_iDefaultIconID;
	int                             m_iDefaultBorderID;
	std::map<int, TLogicAvatarElem> m_stAvatarList;
};

///////////////////////////////////////////////////////////////////////////////////////////////
struct TLogicFragmentCombineConfigElem
{
	TLogicFragmentCombineConfigElem() : m_iItemType(0), m_stConsumeItem() {}

	int32_t     m_iItemType;
	std::vector<CPackGameItem> m_stConsumeItem;
};

class CLogicConfigFragmentCombineParser : public CLogicConfigParserBase
{
public:
	CLogicConfigFragmentCombineParser() : CLogicConfigParserBase(), m_stConfigMap() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

	const TLogicFragmentCombineConfigElem* GetConfig(int iID) const;

private:
    bool CheckCombineLoop(int32_t iItemID, std::set<int32_t> stIDList) const;

public:
	std::map<int, TLogicFragmentCombineConfigElem> m_stConfigMap;
};

///////////////////////////////////////////////////////////////////////////////////////////////
struct TLogicHonorElem
{
	struct honor_task
	{
		int32_t m_iType = 0;
		int32_t m_iTarget = 0;
		int32_t m_iTargetAmount = 0;
	};

	struct honor_attr
	{
		int32_t m_iCardType = 0;
		int32_t m_iNumType = 0;
		int32_t m_iAttrType = 0;
		double m_dAttrValue = 0;
	};

	TLogicHonorElem() : m_strName(), m_strBroadcast(), m_stTaskVec(), m_stConsumeItem(), m_stAttrVec() {}

	std::string m_strName;
	std::string m_strBroadcast;
	std::vector<honor_task> m_stTaskVec;
	CPackGameItem m_stConsumeItem;
	std::vector<honor_attr> m_stAttrVec;
};

class CLogicConfigHonorParser : public CLogicConfigParserBase
{
public:
	CLogicConfigHonorParser() = default;

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

	const TLogicHonorElem* GetHonorConfig(int32_t iHonorID) const;

	std::map<int32_t, TLogicHonorElem> m_stConfigMap;
	std::multimap<int32_t, int32_t> m_stTaskType2HonorIDMap;
};

struct TLogicCurrencyItem
{
    TLogicCurrencyItem() : m_iItemID(0), m_strName("") {}

    int32_t         m_iItemID;
    std::string     m_strName;
};

class CLogicConfigCurrencyParser : public CLogicConfigParserBase
{
public:
    std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

    const TLogicCurrencyItem* GetConfig(int32_t iID) const;

    std::unordered_map<int32_t, TLogicCurrencyItem> m_stConfigMap;
};

struct TLogicGiftBagElem
{
    TLogicGiftBagElem() : m_iLevel(0), m_iOpenDate(0), m_bIsPerDay(false), m_iIntervalTime(0), m_iOpenCount(0),
          m_stItems(), m_strCardBag(), m_strLevelDrop(), m_strLevelDropBag() {}

    int32_t         m_iLevel;
    int32_t         m_iOpenDate;
    bool            m_bIsPerDay;
    int32_t         m_iIntervalTime;
    int32_t         m_iOpenCount;
    std::vector<CPackGameItem> m_stItems;
    std::string     m_strCardBag;
    std::string     m_strLevelDrop;
    std::string     m_strLevelDropBag;
};

class CLogicConfigGiftBagParser : public CLogicConfigParserBase
{
public:
    CLogicConfigGiftBagParser() : m_stConfigMap() {}

    std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

    const TLogicGiftBagElem* GetConfig(int32_t iItemID) const;
    bool OpenGift(int32_t iItemID, std::vector<CPackGameItem>& stBonusVec, int32_t iNum = 1) const;

    std::unordered_map<int32_t, TLogicGiftBagElem> m_stConfigMap;
};
