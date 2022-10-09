//
// Created by lxhbroser on 2020/6/6.
//

#pragma once

#include "logic_config_base.h"
#include "server_base_protocol.h"
#include "logic_config_define.h"

struct TLogicRandomStoreLimit
{
    TLogicRandomStoreLimit() : m_iLevel(0), m_iHeartLevel(0), m_stStore() {}

    int32_t     m_iLevel;
    int32_t     m_iHeartLevel;

    std::vector<LiLogicCardbagRateVector> m_stStore;
};

struct TLogicRandomStoreInfo
{
    TLogicRandomStoreInfo() : m_iStoreType(0), m_iOpenLevel(0), m_iRefreshMaxTimes(0), m_stFlashTime(), m_stRefreshCost(), m_stStoreLimit() {}

    int32_t         m_iStoreType;
    int32_t         m_iOpenLevel;
    int32_t         m_iRefreshMaxTimes;
    std::vector<int32_t>                m_stFlashTime;
    std::vector<CPackGameItem>          m_stRefreshCost;
    std::vector<TLogicRandomStoreLimit> m_stStoreLimit;

    std::vector<TLogicCardbagInfoElem> GetCommodityElem(int32_t iLevel, int32_t iHeartLevel) const;
};

class CLogicConfigRandomStoreParser : public CLogicConfigParserBase
{
public:
    CLogicConfigRandomStoreParser() : CLogicConfigParserBase(), m_stRandomStoreInfoMap() {}

    std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

    const TLogicRandomStoreInfo* GetStoreInfo(int32_t iStoreID) const;

    std::map<int32_t, TLogicRandomStoreInfo> m_stRandomStoreInfoMap;
};

struct TLogicHeartStoreLevel
{
    TLogicHeartStoreLevel() : m_iExpValue(0), m_stDiscount() {}

    int32_t     m_iExpValue;        // 升下一级所需经验
    std::map<int32_t, std::vector<int32_t>> m_stDiscount;   // 商店对应折扣列表
};

class CLogicConfigHeartStoreDiscountParser : public CLogicConfigParserBase
{
public:
    CLogicConfigHeartStoreDiscountParser() : CLogicConfigParserBase(), m_iDialogExp(0), m_iDailyDialogExpTimes(0), m_stLevelList() {}

    std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent);

    void ReCalcHeartLevelExp(int32_t& iLevel, int32_t& iExp) const;
    const TLogicHeartStoreLevel* GetConfig(int32_t iLevel) const;

    int32_t     m_iDialogExp;           // 每次互动增加好感度经验
    int32_t     m_iDailyDialogExpTimes; // 每日互动次数
    std::vector<TLogicHeartStoreLevel> m_stLevelList;  // 等级经验以及对应商店折扣表
};
