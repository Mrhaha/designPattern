//
// Created by czy on 2022/8/8.
//

#pragma once
#include "logic_config_base.h"
#include "server_base_protocol.h"
#include "server_common_protocol.h"
#include "server_gashapon_protocol.h"

struct TLogicGashRewardInfo
{
    TLogicGashRewardInfo():m_iIndex(0),m_strGrade(""),m_iWeight(0),m_iTotalNum(0),
          m_stItem(){}

    int32_t m_iIndex;
    std::string m_strGrade;
    int32_t m_iWeight;
    int32_t m_iTotalNum;
    CPackGameItem m_stItem;
};

struct TLogicGashInfo
{
    TLogicGashInfo():m_iGashID(0),m_vecConsumeItem(),m_mapGashRewardInfo(){}

    int32_t m_iGashID;
    std::vector<CPackGameItem> m_vecConsumeItem;
    std::unordered_map<int32_t,TLogicGashRewardInfo> m_mapGashRewardInfo;
};

struct TLogicLevelBossPro
{
    TLogicLevelBossPro():m_iLevelID(0),m_iCrushBossPro(0),m_uMapGradeWeight(){}

    int32_t m_iLevelID;
    int32_t m_iCrushBossPro;
    std::unordered_map<int32_t,int32_t> m_uMapGradeWeight;
};

struct TLogicBossInfo
{
    TLogicBossInfo():m_iLevelID(0),m_iBossHp(0),m_iGrade(0),m_stRewardItem(){}

    int32_t m_iLevelID;
    int32_t m_iBossHp;
    int32_t m_iGrade;
    std::vector<CPackGameItemRandomNum> m_stRewardItem;
};


class CLogicConfigGashaponParser: public CLogicConfigParserBase
{
public:
    CLogicConfigGashaponParser():m_iAvyID(0),m_iCrushBossCount(0),m_iChapterAvyID(0),m_mapLeveID2Weight(),m_uMapGashInfo(),m_uMapBossInfo()
              ,m_strMailSender(""),m_strMailContent(""),m_strMailTitle(""),m_stMailReward(){};
    std::pair<bool,std::string> Load(const std::string &strFileName,const std::string &strXMLContent);

    int32_t m_iAvyID;
    int32_t m_iCrushBossCount;
    int32_t m_iChapterAvyID;
    std::unordered_map<int32_t,TLogicLevelBossPro> m_mapLeveID2Weight;  //各个关卡的boss概率
    std::unordered_map<int32_t,TLogicGashInfo> m_uMapGashInfo;          //key1=gashID
    std::unordered_map<int32_t,std::vector<TLogicBossInfo>> m_uMapBossInfo;          //key1=grade
    std::unordered_map<int32_t,std::vector<CPackGameItemRandomNum>> m_uMapBossReward;         //key1=bossLevel
    std::unordered_map<int32_t,TLogicBossInfo> m_uMapBossLevelToBossInfo;            //key1=bossLevel
    std::unordered_map<int32_t,std::vector<int32_t>>  m_uMapGashapon2Index;   //key1=gashID,value=index
    std::string m_strMailSender;
    std::string m_strMailContent;
    std::string m_strMailTitle;
    std::vector<CPackGameItem> m_stMailReward;
};