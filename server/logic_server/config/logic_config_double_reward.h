//
// Created by czy on 2022/7/19.
//

#pragma once
#include "logic_config_base.h"
#include "server_base_protocol.h"

struct TLogicDoubleReward
{
    TLogicDoubleReward():m_iIndex(0),m_iLevelType(0),m_iAvy_time() {}

    int32_t m_iIndex;
    int32_t m_iLevelType;
    std::vector<int32_t> m_iAvy_time;
};

class CLogicConfigDoubleRewardParser: public CLogicConfigParserBase
{
public:
    CLogicConfigDoubleRewardParser()
        : CLogicConfigParserBase(){}

    std::pair<bool,std::string> Load(const std::string &strFileName,const std::string &strXMLContent);

    const TLogicDoubleReward* GetDoubleRewardConfig(int32_t iLevelType) const;
    std::map<int32_t,TLogicDoubleReward> m_stDoubleRewardConfig;
};