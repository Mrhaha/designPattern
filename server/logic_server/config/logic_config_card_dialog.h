//
// Created by lxhbroser on 2020/3/3.
//

#pragma once

#include "logic_config_base.h"
#include "server_base_protocol.h"

struct TCardDialogLimitAvyBonus
{
    int32_t     m_iAvyID;
    std::vector<CPackGameItem> m_stBonusList;
};

struct TCardDialogAvyTime
{
    int32_t     m_iStartDay;
    int32_t     m_iEndDay;
    int32_t     m_iStartTime;
    int32_t     m_iEndTime;
};

class CLogicConfigCardDialogParser : public CLogicConfigParserBase
{
public:
    CLogicConfigCardDialogParser() : CLogicConfigParserBase() {}

    std::pair<bool, std::string> Load(const std::string& strFileName, const std::string& strXMLContent) override;

    const std::vector<CPackGameItem> GetBonusByBagName(int32_t iLevel, std::string strBagName) const;

    std::vector<std::pair<int32_t, int32_t>>        m_stTimesCDList;
    std::multimap<int32_t, TCardDialogLimitAvyBonus>   m_stLimitAvyRewardMap;
    std::vector<std::pair<std::string, TCardDialogAvyTime>>     m_stOtherAvyRewardList;

    std::vector<std::vector<std::vector<std::string>>>      m_stDialogRewardList;        // [伙伴ID对应索引][互动次数][选项]
    std::map<int32_t, int32_t>      m_stCardIndexMap;       // 伙伴ID对应索引列表

    std::map<std::string, std::map<int32_t, std::vector<std::string>>>  m_stRewardBagsMap;
};
