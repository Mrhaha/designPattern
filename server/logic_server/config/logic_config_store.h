#pragma once

#include "Markup.h"
#include "logic_config_base.h"
#include "server_base_protocol.h"
#include "logic_config_define.h"

///////////////////////////////////////////////////////////////////////////////////////////////
class CLogicConfigLevelDropParser : public CLogicConfigParserBase
{
public:
	CLogicConfigLevelDropParser() : CLogicConfigParserBase(), m_stBonusMap() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

	std::vector<CPackGameItem> GetRandBonusByName(const std::string& strCardBagName) const;

	void GetRandBonusByName(const std::string& strCardBagName, std::vector<CPackGameItem>& stRandBonus) const;

    const std::vector<TLogicRandBonusItem>* GetConfig(std::string strName) const;

	std::map<std::string, std::vector<TLogicRandBonusItem> >  m_stBonusMap;
};

struct TLogicLevelDropBag
{
    TLogicLevelDropBag() : m_iTotalWeight(0), m_stDrops() {}

    int32_t     m_iTotalWeight;
    std::vector<std::pair<int32_t, std::vector<CPackGameItem>>> m_stDrops;  // <weight, <items>>
};

struct TLogicCardbagConfigElem
{
    TLogicCardbagConfigElem() : m_iTotalWeight(0), m_iConfigTotalWeight(0), m_stCardbagElem() {}

    int m_iTotalWeight;
    int m_iConfigTotalWeight;
    std::vector<TLogicRandBonusItem> m_stCardbagElem;
};

class CLogicConfigLevelDropBagParser : public CLogicConfigParserBase
{
public:
    CLogicConfigLevelDropBagParser() : m_stBags() {}

    std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

    const TLogicLevelDropBag* GetConfig(const std::string& strBag) const;

    void GetRandReward(const std::string& strBag, std::vector<CPackGameItem>& stBonus) const;

    std::map<std::string, TLogicLevelDropBag> m_stBags;
};

//////////////////////////////////////////////////////////////////////////////////////////
struct TLogicMallConfigElem
{
	TLogicMallConfigElem() : m_iAvyID(0), m_bFirstFree(false), m_strFirstBag(""), m_bIsOnlyPayRmb(false), m_iTotalTimes(0),
	    m_strCardBag(""), m_iRecordGrade(0), m_strTenBaodiBag(""), m_iDailyLimitPayRmb(0), m_iDailyLimitPayTimes(0),
	    m_iSinglePrice(0), m_iTenPrice(0), m_stSingleItem(), m_stTenItem(), m_stPtCardExchangeMap(),m_strName(""),
          m_iStartFixed(0),m_iPerFixedWeight(0),m_iInitGradeCard(),m_iInitTenGradeCard(){}

    int32_t                 m_iAvyID;                       // ????????????ID
	bool                    m_bFirstFree;                   // ??????????????????
	std::string             m_strFirstBag;                  // ??????????????????
    bool                    m_bIsOnlyPayRmb;                // ??????????????????????????????
    int32_t                 m_iTotalTimes;
    std::string             m_strCardBag;
    int32_t                 m_iRecordGrade;
    std::string             m_strTenBaodiBag;
    int32_t                 m_iDailyLimitPayRmb;
    int32_t                 m_iDailyLimitPayTimes;
    int32_t                 m_iSinglePrice;                 // ??????????????????
    int32_t                 m_iTenPrice;                    // ????????????????????????
    CPackGameItem           m_stSingleItem;                 // ??????????????????
    std::vector<CPackGameItem> m_stTenItem;                    // ??????????????????
    std::map<int32_t, int32_t> m_stPtCardExchangeMap;
    std::string             m_strName;                      // ????????????

    int32_t                 m_iStartFixed;                // ????????????????????????
    int32_t                 m_iPerFixedWeight;              // ??????????????????????????????????????????

    std::unordered_map<int32_t,int32_t> m_initCardWeight;       //????????????<grade,weight>
    std::unordered_map<int32_t,int32_t> m_initTenCardWeight;    //??????????????????<grade,weight>
    std::unordered_map<int32_t,TLogicCardbagConfigElem> m_iInitGradeCard; //???????????????????????????card
    std::unordered_map<int32_t,TLogicCardbagConfigElem> m_iInitTenGradeCard; //????????????????????????card

};

class CLogicConfigMallParser : public CLogicConfigParserBase
{
public:
    enum
    {
        E_TenMallNum = 10,          // ????????????
    };
	CLogicConfigMallParser() : CLogicConfigParserBase(), m_iBaodiTimes(0), m_strBaodiBag(), m_iExcludeID(0), m_iRecordGrade(0), m_stMallConfig() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

    const TLogicMallConfigElem* GetConfig(int iCommodityID) const;

    int32_t         m_iBaodiTimes;
    std::string     m_strBaodiBag;
    int32_t         m_iExcludeID;
    int32_t         m_iRecordGrade;
	std::map<int, TLogicMallConfigElem>   m_stMallConfig;
};

/////////////////////////////////////////////////////////////////////////////////////////////////


class CLogicConfigCardbagParser : public CLogicConfigParserBase
{
public:
	CLogicConfigCardbagParser() : CLogicConfigParserBase(), m_stCardbagConfig() {  }

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

    const TLogicCardbagConfigElem* GetConfig(std::string strBag) const;
	std::vector<TLogicRandBonusItem> GetRandBonusByName(const std::string& strCardbagName, int iTimes = 1) const;
	void GetRandBonusByName(const std::string& strCardbagName, std::vector<CPackGameItem>& stBonusVec, int iTimes = 1) const;

	std::unordered_map<std::string, TLogicCardbagConfigElem> m_stCardbagConfig;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
struct TLogicBuyResourceElem
{
	CPackGameItem    m_stConsumeItem;
	CPackGameItem    m_stGetItem;
};

class CLogicConfigBuyResourceParser : public CLogicConfigParserBase
{
public:
	CLogicConfigBuyResourceParser() : m_iPVPOnceBuyTimes(0), m_stBuyGoldConfig(), m_stBuyEnergyConfig(), m_stBuyVitalityConfig(), m_stRefreshPVP(), m_stRefreshLevelTypeMap() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

    int     m_iPVPOnceBuyTimes;

	std::vector<TLogicBuyResourceElem>       m_stBuyGoldConfig;
	std::vector<TLogicBuyResourceElem>       m_stBuyEnergyConfig;
	std::vector<TLogicBuyResourceElem>       m_stBuyVitalityConfig;

	std::vector<CPackGameItem>                  m_stRefreshPVP;
    std::map<int, std::vector<CPackGameItem>>   m_stRefreshLevelTypeMap;

	const TLogicBuyResourceElem* GetBuyGoldConfig(int iTimes) const;
	const TLogicBuyResourceElem* GetBuyEnergyConfig(int iTimes) const;
	const TLogicBuyResourceElem* GetBuyVitalityConfig(int iTimes) const;

	const CPackGameItem* GetRefreshPVPConfig(int iTimes) const;
    const CPackGameItem* GetRefreshLevelTypeConfig(int iLevelType, int iTimes) const;
};

////////////////////////////////////////////////////////////////////////////////
class CLogicConfigMallBagParser : public CLogicConfigParserBase
{
public:
	CLogicConfigMallBagParser() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

	std::map<std::string, std::vector<TLogicCardbagInfoElem>>   m_stCardBagInfo;

	TLogicCardbagInfoElem GetCommodityElem(const std::string& strCardBagName) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////
struct TLogicShopItemElem
{
    int32_t         m_iAvyID;               // ????????????
	CPackGameItem   m_stConsumeItem;
    CPackGameItem   m_stOrConsumeItem;      // ????????????????????????
	CPackGameItem   m_stGetItem;
    int32_t         m_iLimitType;
    int32_t         m_iLimitBuyCount;
	int32_t         m_iRuleType;
	int32_t         m_iRulePara;
    std::vector<int32_t> m_vecContentIndex; //?????????????????????Index
};


//?????????????????????
struct TLogicConstelShop
{
    TLogicConstelShop():m_iRefreshCount(0),m_iSpecialPro(0),m_iGradePro3(0),m_iGradePro4(0),m_iGradePro5(0),
          m_iGradePro6(0),m_iRefreshTime(0),m_iRefreshConsume(){}

    int32_t     m_iRefreshCount;
    int32_t     m_iSpecialPro;
    int32_t     m_iGradePro3;
    int32_t     m_iGradePro4;
    int32_t     m_iGradePro5;
    int32_t     m_iGradePro6;
    int32_t     m_iRefreshTime;
    CPackGameItem m_iRefreshConsume;
};

class CLogicConfigShopParser : public CLogicConfigParserBase
{
public:
	CLogicConfigShopParser() : CLogicConfigParserBase(), m_stShopMap(), m_stLimitTypeMap(),m_stConstelShopConfig(),m_stConstelShop() {}

	std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

    const TLogicShopItemElem* GetConfig(int32_t iIndex) const;

	std::unordered_map<int32_t, TLogicShopItemElem> m_stShopMap;
    std::unordered_map<int32_t, std::vector<int32_t>> m_stLimitTypeMap;     // ??????????????????????????????????????????   [limit_type, [index]]
    TLogicConstelShop m_stConstelShopConfig;                                // ?????????????????????
    std::unordered_map<int32_t,std::vector<int32_t>> m_stConstelShop;       // ?????????????????????

};
