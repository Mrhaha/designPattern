//
// Created by lxhbroser on 2019/12/25.
//

#include "logic_config_anti_addiction.h"
#include "server_errcode.h"
#include "Markup.h"
#include "common_datetime.h"

std::pair<bool, std::string> CLogicConfigAntiAddictionParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
    CMarkup stXML;
    std::stringstream strErrorInfo;

    try
    {
        if(true == strXMLContent.empty())
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
            throw std::exception();
        }

        if(false == stXML.SetDoc(strXMLContent))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        if(false == stXML.FindElem("root"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();

        if(false == stXML.FindElem("attr"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        m_stAntiAddictionAttr.m_byMainSwitch    = (bool)atoi(stXML.GetAttrib("switch").c_str());
        m_stAntiAddictionAttr.m_nAgeLimit       = (int16_t)atoi(stXML.GetAttrib("age_limit").c_str());
        m_stAntiAddictionAttr.m_byLoginBeginHour = (int8_t)atoi(stXML.GetAttrib("login_begin_hour").c_str());
        m_stAntiAddictionAttr.m_byLoginEndHour  = (int8_t)atoi(stXML.GetAttrib("login_end_hour").c_str());
        m_stAntiAddictionAttr.m_iVisitorPeriod  = ::atoi(stXML.GetAttrib("visitor_period").c_str());
        m_stAntiAddictionAttr.m_iVisitorTime    = ::atoi(stXML.GetAttrib("visitor_time").c_str());
        m_stAntiAddictionAttr.m_iNormalTime     = ::atoi(stXML.GetAttrib("normal_time").c_str());
        m_stAntiAddictionAttr.m_iHolidayTime    = ::atoi(stXML.GetAttrib("holiday_time").c_str());

        if(false == stXML.FindElem("servers"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();
        while (stXML.FindElem("server"))
        {
            TLogicServerAntiAddictionElem server;
            server.m_iServerID          = ::atoi(stXML.GetAttrib("server_id").c_str());
            server.m_byNotifySwitch     = (bool)::atoi(stXML.GetAttrib("notify_switch").c_str());
            server.m_byTimeLimitSwitch  = (bool)::atoi(stXML.GetAttrib("sys_switch").c_str());
            server.m_byVisitorSwitch    = (bool)::atoi(stXML.GetAttrib("visitor_switch").c_str());

            m_stServerAntiAddictionMap[server.m_iServerID] = server;
        }
        stXML.OutOfElem();

        if(stXML.FindElem("notifications"))
        {
            stXML.IntoElem();
            while (stXML.FindElem("notify"))
            {
                int8_t byType = (int8_t)atoi(stXML.GetAttrib("notify_type").c_str());
                CONFIG_ASSERT_EXCEPTION(byType > (int8_t)CEAntiAddNotifyType::EAANT_Min && byType < (int8_t)CEAntiAddNotifyType::EAANT_Max);

                int32_t iTimeLimit = ::atoi(stXML.GetAttrib("time_limit").c_str());
                std::string strNotify = stXML.GetAttrib("notify");

                m_stAntiAddNotifyMap[byType].push_back(std::make_pair(iTimeLimit, strNotify));
            }
            stXML.OutOfElem();
        }

        if(stXML.FindElem("recharge_notify"))
        {
            stXML.IntoElem();
            while (stXML.FindElem("notify"))
            {
                TLogicAntiAddRechargeNotify notify;
                notify.m_iAgeMin = ::atoi(stXML.GetAttrib("age_min").c_str());
                notify.m_iAgeMax = ::atoi(stXML.GetAttrib("age_max").c_str());
                notify.m_iSingleRecharge = ::atoi(stXML.GetAttrib("single_num").c_str());
                notify.m_strSingleNotify = stXML.GetAttrib("single_notify");
                notify.m_iMonthRecharge = ::atoi(stXML.GetAttrib("month_num").c_str());
                notify.m_strMonthNotify = stXML.GetAttrib("month_notify");

                m_stAntiAddRechargeNotify.push_back(notify);
            }
            stXML.OutOfElem();
        }
    }
    catch (const std::exception& stEx)
    {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }

    return (std::pair<bool, std::string>(true, "OK"));
}

const TLogicServerAntiAddictionElem* CLogicConfigAntiAddictionParser::GetServerAntiAddictionConfig(int32_t iServerID) const
{
    auto iter = m_stServerAntiAddictionMap.find(iServerID);
    // 如果服务器没有专属配置，则使用默认配置，默认配置的ServerID默认为0
    if(iter == m_stServerAntiAddictionMap.end())
        iter = m_stServerAntiAddictionMap.find(0);

    return (iter == m_stServerAntiAddictionMap.end() ? &m_stDefaultServerAntiAddiction : &(iter->second));
}

const std::vector<std::pair<int32_t, std::string>>* CLogicConfigAntiAddictionParser::GetAntiAddNotificationsByType(CEAntiAddNotifyType eType) const
{
    const auto iter = m_stAntiAddNotifyMap.find((int8_t)eType);
    if(iter == m_stAntiAddNotifyMap.end())
        return nullptr;

    return &iter->second;
}
