//
// Created by czy on 2022/9/6.
//
#pragma once
#include "logic_config_base.h"
#include "server_base_protocol.h"


struct TLogicAreaInfo
{
    TLogicAreaInfo():m_iBuffID(0),m_vecCardID(),m_iLevelParam(0),m_vecBuffParam(){}

    int32_t m_iBuffID;
    std::vector<int32_t> m_vecCardID;
    int32_t m_iLevelParam;
    std::vector<float> m_vecBuffParam;
};

struct TLogicAreaLevelup
{
    TLogicAreaLevelup():m_iLevel(0),m_iNeedStar(0){}

    int32_t m_iLevel;
    int32_t m_iNeedStar;
};

class CLogicConfigCompleteAtlasParser: public CLogicConfigParserBase
{
public:
    CLogicConfigCompleteAtlasParser()
        : CLogicConfigParserBase(),m_mapBuffID2Config(),m_mapLevelUpInfo(),m_vecCompleteCard(),m_iAvyID(){}

    std::pair<bool,std::string> Load(const std::string &strFileName,const std::string &strXMLContent) override;

    std::string m_strCardBag;
    std::unordered_map<int32_t,TLogicAreaInfo> m_mapBuffID2Config;
    std::unordered_map<int32_t,TLogicAreaLevelup> m_mapLevelUpInfo;
    std::vector<int32_t> m_vecCompleteCard;
    int32_t m_iAvyID;
};






