//
// Created by lxhbroser on 2019/7/1.
//

#pragma once

#include "logic_config_base.h"

struct TItemMonitor
{
    TItemMonitor(int32_t iType = 0, int32_t iItemID = 0, int8_t iTimeType = 0, int32_t iMaxNum = 0)
        : m_iType(iType), m_iItemID(iItemID), m_iTimeType(iTimeType), m_iMaxNum(iMaxNum) {}

    int32_t     m_iType;
    int32_t     m_iItemID;
    int8_t      m_iTimeType;
    int32_t     m_iMaxNum;
};

class CLogicConfigItemMonitorParser : public CLogicConfigParserBase
{
public:
    CLogicConfigItemMonitorParser() : CLogicConfigParserBase() {}

    std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

    bool IsValidTimeType(int8_t iType) const { return iType > CLogicConfigDefine::EMTT_Invalid && iType < CLogicConfigDefine::EMTT_MAX; }
    bool IsValidItemType(int32_t iType) const { return iType > CLogicConfigDefine::GAME_ITEM_COLLECTION_START && iType < CLogicConfigDefine::GAME_ITEM_COLLECTION_END; }

    int32_t GetKey(int32_t iItemType, int32_t iItemID) const { return (iItemID * 100 + iItemType); }

    const TItemMonitor* GetConfig(int32_t iItemType, int32_t iItemID) const;

    std::map<int32_t, TItemMonitor>     m_stItemMonitorConfigMap;
};
