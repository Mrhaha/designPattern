//
// Created by lxhbroser on 2019/7/3.
//

#pragma once

#include "logic_config_base.h"
#include "server_cmd_admin_define.h"
#include "server_cmd_logic_define.h"
#include "server_cmd_extend_define.h"

class CLogicConfigProtoSwitchParser : public CLogicConfigParserBase
{
public:
    CLogicConfigProtoSwitchParser() : CLogicConfigParserBase() {}

    std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

    bool IsValidCmd(int32_t iCmd) const {
        return (iCmd > CMD_EXTEND_START && iCmd < CMD_EXTEND_END)
            || (iCmd > MSG_LOGIC_NULL_CMD && iCmd < MSG_LOGIC_MAX_CMD)
            || (iCmd > MSG_LOGIC_TOUHOU_NULL_CMD && iCmd < MSG_LOGIC_TOUHOU_MAX_CMD)
            || (iCmd >= MSG_ADMIN_START && iCmd < MSG_ADMIN_END);
    }

    int32_t GetConfig(int32_t iCmd) const;

    std::map<int32_t, int32_t>      m_stProtoSwitchConfigMap;
};
