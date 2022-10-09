//
// Created by lxhbroser on 2019/12/25.
//

#pragma once

#include "logic_config_base.h"

enum class CEAntiAddNotifyType
{
    EAANT_Min,
    EAANT_LoginLimit    = 1,        // 登陆时间限制提示
    EAANT_NormalDay     = 2,        // 工作日提示
    EAANT_Holiday       = 3,        // 节假日提示
    EAANT_Visitor       = 4,        // 游客提示

    EAANT_Max,
};

struct TLogicAntiAddictionAttr
{
    bool        m_byMainSwitch;                 // 防沉迷总开关
    int16_t     m_nAgeLimit;                    // 防沉迷年龄限制
    int8_t      m_byLoginBeginHour;             // 允许登录开始时间
    int8_t      m_byLoginEndHour;
    int32_t     m_iVisitorPeriod;               // 单个设备游客模式可使用周期
    int32_t     m_iVisitorTime;                 // 游客模式允许游戏时间
    int32_t     m_iNormalTime;                  // 防沉迷用户工作日每日在线时长限制
    int32_t     m_iHolidayTime;                 // 防沉迷用户节假日每日在线时长限制
};

struct TLogicServerAntiAddictionElem
{
public:
    TLogicServerAntiAddictionElem() : m_iServerID(0), m_byNotifySwitch(false), m_byTimeLimitSwitch(false), m_byVisitorSwitch(false) {}

    int32_t     m_iServerID;                    // 服务器ID
    bool        m_byNotifySwitch;               // 防沉迷提示开关
    bool        m_byTimeLimitSwitch;            // 防沉迷在线时长限制开关
    bool        m_byVisitorSwitch;              // 游客模式开关
};

struct TLogicAntiAddRechargeNotify
{
  public:
    TLogicAntiAddRechargeNotify() : m_iAgeMin(0), m_iAgeMax(0), m_iSingleRecharge(0), m_strSingleNotify(""), m_iMonthRecharge(0), m_strMonthNotify("") {}

    int32_t     m_iAgeMin;
    int32_t     m_iAgeMax;
    int32_t     m_iSingleRecharge;
    std::string m_strSingleNotify;
    int32_t     m_iMonthRecharge;
    std::string m_strMonthNotify;
};

class CLogicConfigAntiAddictionParser : public CLogicConfigParserBase
{
public:
    CLogicConfigAntiAddictionParser() : CLogicConfigParserBase() {}

    std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

    const TLogicServerAntiAddictionElem* GetServerAntiAddictionConfig(int32_t iServerID) const;
    const std::vector<std::pair<int32_t, std::string>>* GetAntiAddNotificationsByType(CEAntiAddNotifyType eType) const;

    TLogicServerAntiAddictionElem   m_stDefaultServerAntiAddiction;

    TLogicAntiAddictionAttr         m_stAntiAddictionAttr;
    std::unordered_map<int32_t, TLogicServerAntiAddictionElem>  m_stServerAntiAddictionMap;
    std::unordered_map<int8_t, std::vector<std::pair<int32_t, std::string>>> m_stAntiAddNotifyMap;
    std::vector<TLogicAntiAddRechargeNotify>    m_stAntiAddRechargeNotify;
};
