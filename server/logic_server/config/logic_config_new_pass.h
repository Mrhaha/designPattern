//
// Created by czy on 2022/4/25.
//

#pragma once
#include "logic_config_base.h"
#include "server_base_protocol.h"

struct TLogicLevelExpElem
{
    TLogicLevelExpElem():m_iLevel(0),m_iNeedExp(0){}

    int32_t m_iLevel;
    int32_t m_iNeedExp;
};

struct TLogicNewPassRewardElem
{
    TLogicNewPassRewardElem():m_iLevel(0),m_stReward(){}

    int32_t m_iLevel;
    std::vector<CPackGameItem> m_stReward;
};


class CLogicConfigNewPassParser: public CLogicConfigParserBase
{
public:
    CLogicConfigNewPassParser()
        : CLogicConfigParserBase(),m_mapLevel2Exp(),m_mapLevel2Normal(),m_mapLevel2Advanced(){}

    std::pair<bool,std::string> Load(const std::string &strFileName,const std::string &strXMLContent);

    std::map <int32_t,int32_t> m_mapLevel2Exp;                                   //各个通行证等级所需经验
    std::unordered_map<int32_t,TLogicNewPassRewardElem> m_mapLevel2Normal;       //通行证等级->NormalReward
    std::unordered_map<int32_t,TLogicNewPassRewardElem> m_mapLevel2Advanced;     //通行证等级->AdvancedReward

    int32_t m_iActID;
    int32_t m_iDepositID;                                                        //对应活动的通行证ID
    int32_t m_iUpDepositID;                                                      //通行证升级对应的ID
    int32_t m_iAdvancedDepositID;                                                //高级通行证对应的ID;

    std::string m_strSender;
    std::string m_strTitle;
    std::string m_strContent;
    int32_t GetNewPassLevelByExp(int32_t currExp) const;
    const TLogicNewPassRewardElem* GetNormalRewardByLevel(int32_t level) const;
    const TLogicNewPassRewardElem* GetAdvancedRewardByLevel(int32_t level) const;
};