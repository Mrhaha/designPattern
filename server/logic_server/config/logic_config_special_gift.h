//
// Created by lxhbroser on 2019/8/10.
//

#pragma once

#include "logic_config_base.h"
#include "logic_config_define.h"
#include "server_base_protocol.h"

struct TLogicSpecialGiftInfo
{
    int32_t                 m_iLevelMin;
    std::vector<int32_t>    m_stDepositIDList;
};

struct TLogicSpecialGift
{
    int32_t                     m_iActivityID;
    TLogicActivityRefElem       m_stActivityRef;
    std::map<int32_t, TLogicSpecialGiftInfo>    m_stInfoMap;
};

class CLogicConfigSpecialGiftParser : public CLogicConfigParserBase
{
public:
    typedef std::map<int32_t, TLogicSpecialGift>            map_container_type;
    typedef typename map_container_type::const_iterator     const_iterator;

public:
    CLogicConfigSpecialGiftParser() : CLogicConfigParserBase() {}

    std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

    bool IsValidAvyType(int32_t iType) const { return iType > CLogicConfigDefine::ESGT_Invalid && iType < CLogicConfigDefine::ESGT_MAX; }
    bool IsValidDepositID(int32_t iType, int32_t iAvyID, int32_t iLevel, int32_t iDepositID) const;

    map_container_type        m_stSpecialGiftWeekMap;
    map_container_type        m_stSpecialGiftMonthMap;
};
