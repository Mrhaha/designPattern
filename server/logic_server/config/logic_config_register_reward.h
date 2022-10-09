//
// Created by czy on 2022/7/25.
//
#pragma once
#include "logic_config_base.h"
#include "server_base_protocol.h"


class CLogicConfigRegisterRewardParser: public CLogicConfigParserBase
{
public:
    CLogicConfigRegisterRewardParser()
        : CLogicConfigParserBase(),m_iAvyID(0),m_strSender(""),m_strContent(""),m_strTitle(""),m_stReward(),m_mapAccount(){}

    std::pair<bool,std::string> Load(const std::string &strFileName,const std::string &strXMLContent);

    int32_t m_iAvyID;
    std::string m_strSender;
    std::string m_strContent;
    std::string m_strTitle;
    std::vector<CPackGameItem> m_stReward;
    std::map<std::string,bool> m_mapAccount;
};