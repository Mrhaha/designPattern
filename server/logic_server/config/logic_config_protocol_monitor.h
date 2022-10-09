//
// Created by lxhbroser on 2019/6/29.
//

#pragma once

#include "logic_config_base.h"
#include "server_cmd_logic_define.h"

struct TProtoMonitor
{
    TProtoMonitor(int32_t iCmd = 0, int8_t iTimeType = 0, int32_t iMaxNum = 0)
        : m_iCmd(iCmd), m_iTimeType(iTimeType), m_iMaxNum(iMaxNum) {}

    int32_t         m_iCmd;
    int8_t          m_iTimeType;
    int32_t         m_iMaxNum;
};

class CLogicConfigProtocolMonitorParser : public CLogicConfigParserBase
{
public:
    CLogicConfigProtocolMonitorParser() : CLogicConfigParserBase() {}

    std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

    bool IsValidTimeType(int8_t iType) const { return iType > CLogicConfigDefine::EMTT_Invalid && iType < CLogicConfigDefine::EMTT_MAX; }
    bool IsValidCmd(int32_t iCmd) const { return (iCmd > MSG_LOGIC_NULL_CMD && iCmd < MSG_LOGIC_MAX_CMD) || (iCmd > MSG_LOGIC_TOUHOU_NULL_CMD && iCmd < MSG_LOGIC_TOUHOU_MAX_CMD); }

    const TProtoMonitor* GetConfig(int32_t iCmd) const;

    std::map<int32_t, TProtoMonitor>        m_stProtoMonitorConfigMap;
};
