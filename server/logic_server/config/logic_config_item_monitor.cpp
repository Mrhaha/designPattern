//
// Created by lxhbroser on 2019/7/1.
//

#include "logic_config_item_monitor.h"
#include "server_errcode.h"
#include "Markup.h"

std::pair<bool, std::string>
CLogicConfigItemMonitorParser::Load(const std::string &strFileName, const std::string &strXMLContent)
{
    CMarkup stXML;
    std::stringstream strErrorInfo;

    do
    {
        if(true == strXMLContent.empty())
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:XML CONTENT IS EMPTY" << std::endl;
            break;
        }

        if(false == stXML.SetDoc(strXMLContent))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            break;
        }

        if(false == stXML.FindElem("item_list"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            break;
        }

        stXML.IntoElem();
        while (true == stXML.FindElem("item"))
        {
            TItemMonitor stMonitor;
            stMonitor.m_iType = ::atoi(stXML.GetAttrib("type").c_str());
            if(IsValidItemType(stMonitor.m_iType) == false)
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:Invalid Type:" << stMonitor.m_iType << std::endl;
                break;
            }

            stMonitor.m_iItemID = ::atoi(stXML.GetAttrib("id").c_str());
            int32_t iKey = GetKey(stMonitor.m_iType, stMonitor.m_iItemID);
            if(m_stItemMonitorConfigMap.find(iKey) != m_stItemMonitorConfigMap.end())
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:Repeated Type:" << stMonitor.m_iType << " AND ID:" << stMonitor.m_iItemID << std::endl;
                break;
            }

            stMonitor.m_iTimeType = (int8_t)::atoi(stXML.GetAttrib("time_type").c_str());
            if(IsValidTimeType(stMonitor.m_iTimeType) == false)
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:Invalid TimeType Type:" << stMonitor.m_iType << " ID:" << stMonitor.m_iItemID << std::endl;
                break;
            }

            stMonitor.m_iMaxNum = ::atoi(stXML.GetAttrib("max_num").c_str());

            m_stItemMonitorConfigMap[iKey] = std::move(stMonitor);
        }
    }while (0);

    if(strErrorInfo.str().empty())
    {
        return (std::pair<bool, std::string>(true, "OK"));
    }
    else
    {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }
}

const TItemMonitor* CLogicConfigItemMonitorParser::GetConfig(int32_t iItemType, int32_t iItemID) const
{
    int32_t iKey = GetKey(iItemType, iItemID);
    const auto iter = m_stItemMonitorConfigMap.find(iKey);

    return iter == m_stItemMonitorConfigMap.end() ? nullptr : &iter->second;
}
