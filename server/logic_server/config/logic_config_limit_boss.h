//
// Created by czy on 2022/5/5.
//

#pragma once
#include "logic_config_base.h"
#include "server_base_protocol.h"

struct TLogicLimitBossWeightElem
{
    TLogicLimitBossWeightElem():m_iLevelID(0),m_iWeight(0){}

    int32_t m_iLevelID;
    int32_t m_iWeight;
};

struct TLogicLimitBossElem
{
    TLogicLimitBossElem():m_iLevelID(0),m_iTotalHP(0),m_iWeight(0),m_iPhysicalConsume(0),m_iHpRewardScore(0),m_iFindRewardScore(0),m_stRewardItem(){}

    int32_t m_iLevelID;
    int32_t m_iTotalHP;
    int32_t m_iWeight;
    int32_t m_iPhysicalConsume;                                         //限时Boss体力消耗
    int32_t m_iHpRewardScore;
    int32_t m_iFindRewardScore;
    std::vector<CPackGameItem> m_stRewardItem;                          //击杀奖励，每个人都有
};


class CLogicConfigLimitBossParser: public CLogicConfigParserBase
{
public:
    CLogicConfigLimitBossParser()
        : CLogicConfigParserBase(),m_iAcyID(0),m_iPointItemID(0),m_iLimitTime(0),m_iInviteFriendNum(0),
          m_iBossLimitNumber(0),m_iDailyGetRewardCount(0),m_strSender(""),m_strTitle(""),m_strContent(""),
          m_mapLeveID2Weight(),m_mapLevelID2Boss(),m_iBaodiTime(0){}

    std::pair<bool,std::string> Load(const std::string &strFileName,const std::string &strXMLContent);


    int32_t m_iAcyID;
    int32_t m_iPointItemID;
    int32_t m_iLimitTime;
    int32_t m_iInviteFriendNum;
    int32_t m_iBossLimitNumber;
    int32_t m_iDailyGetRewardCount;

    int32_t m_iLevelStartTime;
    int32_t m_iLevelEndTime;

    std::string m_strSender;
    std::string m_strTitle;
    std::string m_strContent;


    std::map<int32_t,int32_t> m_mapLeveID2Weight;
    std::map<int32_t,TLogicLimitBossElem> m_mapLevelID2Boss;
    int32_t m_iBaodiTime;

    int32_t GetLevelRefreshWeight(int32_t levelID) const;

    const TLogicLimitBossElem* GetLimitBossConfigByLevelID(int32_t levelID) const;

};