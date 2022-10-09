//
// Created by lxhbroser on 2020/3/10.
//

#pragma once

#include "logic_config_base.h"
#include "server_base_protocol.h"

struct TLogicNewsThemeElem
{
    int32_t     m_iID;
    int32_t     m_iPara1;
    int32_t     m_iPara2;
};

struct TLogicNewsThemeInfo
{
    int32_t     m_iIndex;       // 主体序号
    int32_t     m_iType;        // 事件类型
    int8_t      m_iLimitType;   // 互斥类型(1:不互斥，2:互斥递增类型,表示该类型多条配置时有互斥关系，只显示最高一条记录)
    int32_t     m_iLevelMin;    // 低等级
    int32_t     m_iLevelMax;    // 高等级
    std::vector<TLogicNewsThemeElem>    m_stElemList;
};

struct TLogicNewsDynamicElem
{
    int32_t     m_iIndex;
    int32_t     m_iType;
    int8_t      m_iLimitType;
    int32_t     m_iLvMin;
    int32_t     m_iLvMax;
    int32_t     m_iStartDay;
    int32_t     m_iEndDay;
    std::map<int32_t,std::vector<std::string>> m_iInfo;
};

struct TLogicNewsEmail
{
    std::string m_strTitle;
    std::string m_strContent;
    std::string m_strSender;
};

struct TLogicNewsChooseReward
{

    int32_t m_iChooseID;
    TLogicNewsEmail m_iStEmail;
    std::vector<CPackGameItem> m_stReward;
};

enum EnumWenWenNewsLimitType
{
    EWWNLT_Invalid,
    EWWNLT_Normal   = 1,    // 不互斥
    EWWNLT_Mutex    = 2,    // 互斥递增类型,表示该类型多条配置时有互斥关系，只显示最高一条记录

    EWWNLT_Max,
};

enum EnumWenWenNewsSubType
{
    EWWNST_Invalid,
    EWWNST_Rank         = 1,    // 排行榜类型
    EWWNST_MaxRecord    = 2,    // 只记录最大值类型(例如 卡牌数量)
    EWWNST_Mutex        = 3,    // 互斥类型，同上 EnumWenWenNewsLimitType 的互斥类型
    EWWNST_NormalValue  = 4,    // 普通值类型(例如 招募指定卡)

    EWWNST_Max,
};





class CLogicConfigWenWenNewsParser : public CLogicConfigParserBase
{
public:
    CLogicConfigWenWenNewsParser() : CLogicConfigParserBase() {}

    std::pair<bool, std::string> Load(const std::string& strFileName, const std::string& strXMLContent) override;

    bool IsValidType(int32_t iType) const { return iType > CLogicConfigDefine::EWWNT_Invalid && iType < CLogicConfigDefine::EWWNT_Max; }
    bool IsValidLimitType(int8_t iLimitType) const { return iLimitType > EWWNLT_Invalid && iLimitType < EWWNLT_Max; }
    bool IsValidSubType(int8_t iSubType) const { return iSubType > EWWNST_Invalid && iSubType < EWWNST_Max; }

    int32_t EnCodePersonData(int32_t iType, int32_t iID) const { return (iType*1000 + iID); }
    void DeCodePersonData(int32_t iCode, int32_t& iType, int32_t& iID) const { iType = iCode/1000; iID = iCode%1000; }

    const TLogicNewsChooseReward *GetWenWenNewsChooseConfig(int32_t m_iChoseID) const;

    int32_t     m_iIntervalTime;        // 主题内容刷新时间
    std::map<int32_t, TLogicNewsThemeInfo>       m_stNewsThemeMap;
    std::map<int32_t, TLogicNewsDynamicElem>     m_stNewsDynamicMap;
    std::unordered_map<int32_t, TLogicNewsChooseReward> m_stNewsChooseMap;
};
