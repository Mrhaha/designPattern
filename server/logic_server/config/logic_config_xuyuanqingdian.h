#pragma once
#include "logic_config_base.h"
#include "logic_config_define.h"
#include "server_base_protocol.h"

////////////////////////////////////////////////////////////////////////////////////
//

struct TMonthPassLevelExp
{
    int32_t m_iExp = 0;
    std::vector<CPackGameItem> m_stBonusVec;
    std::vector<CPackGameItem> m_stDepositVec;
};

struct TMonthPassTask
{
    int32_t m_iType = 0;
    int32_t m_iTarget = 0;
    int32_t m_iTargetAmount = 0;

    std::vector<CPackGameItem>  m_stRewardItemVec;
};

struct TMonthPassExchange
{
    int32_t m_iMaxCount = 0;
    int32_t m_iNeedPass = 0;
    CPackGameItem m_stBonusItem;
    CPackGameItem m_stConsumeItem;
};

struct TMonthPassWishPool
{
    int32_t m_iNeedLevel = 0;
    int32_t m_iNeedCharge = 0;
};

struct TMonthPassWishItem
{
    int32_t m_iNeedPass = 0;
    CPackGameItem m_stWishItem;
};

struct TMonthPassWishEvent
{
    int32_t m_iStartDay = 0;
    int32_t m_iEndDay = 0;
    int32_t m_iWeight = 0;
    std::string m_strEvent;
    int32_t m_iBonusIndex = 0;
    int32_t m_iBonusNum = 0;
};


class CLogicConfigMonthPassParser : public CLogicConfigParserBase
{
public:
    CLogicConfigMonthPassParser() : CLogicConfigParserBase() {}

    std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

    int32_t GetLevelByExp(int32_t iExp) const;

    const TMonthPassLevelExp* GetLevelExpConfig(int32_t iLevel) const;

    const TMonthPassTask* GetTaskConfig(int32_t iTaskID) const;

    const TMonthPassExchange* GetExchangeConfig(int32_t iExchangeID) const;

    const TMonthPassWishPool* GetWishPoolConfig(int32_t iPoolIndex) const;

    const TMonthPassWishItem* GetWishItemConfig(int32_t iItemIndex) const;

    const TMonthPassWishEvent* GetWishEventConfig(int32_t iEventIndex) const;

    int32_t m_iTimeType{0};
    int32_t m_iStartDay{0};
    int32_t m_iEndDay{0};
    int32_t m_iCustomCreateTime{0};
    int32_t m_iDepositID{0};

    std::string m_strWishMailSender;
    std::string m_strWishMailTitle;
    std::string m_strWishMailContent;

    std::map<int32_t, TMonthPassLevelExp> m_stLevelMap;

    std::multimap<int32_t, int32_t> m_stDailyTask;

    std::multimap<int32_t, int32_t> m_stWeeklyTask;

    std::map<int32_t, TMonthPassTask> m_stTaskInfoMap;

    std::map<int32_t, TMonthPassExchange> m_stExchangeMap;

    std::map<int32_t, TMonthPassWishPool> m_stWishPoolMap;

    std::map<int32_t, TMonthPassWishItem> m_stWishItemMap;

    std::map<int32_t, TMonthPassWishEvent> m_stWishEventMap;

};

