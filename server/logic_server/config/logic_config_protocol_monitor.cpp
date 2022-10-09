//
// Created by lxhbroser on 2019/6/29.
//

#include "logic_config_protocol_monitor.h"
#include "server_errcode.h"
#include "Markup.h"

std::pair<bool, std::string>
CLogicConfigProtocolMonitorParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

        if(false == stXML.FindElem("protocol_list"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            break;
        }

        stXML.IntoElem();

        while (true == stXML.FindElem("protocol"))
        {
            TProtoMonitor stMonitor;
            stMonitor.m_iCmd = ::atoi(stXML.GetAttrib("id").c_str());
            if(IsValidCmd(stMonitor.m_iCmd) == false)
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:Invalid CMD:" << stMonitor.m_iCmd << std::endl;
                break;
            }

            if(m_stProtoMonitorConfigMap.find(stMonitor.m_iCmd) != m_stProtoMonitorConfigMap.end())
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:Repeated CMD:" << stMonitor.m_iCmd << std::endl;
                break;
            }

            stMonitor.m_iTimeType = (int8_t)::atoi(stXML.GetAttrib("time_type").c_str());
            if(IsValidTimeType(stMonitor.m_iTimeType) == false)
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:Invalid TimeType CMD:" << stMonitor.m_iCmd<< std::endl;
                break;
            }

            stMonitor.m_iMaxNum = ::atoi(stXML.GetAttrib("max_num").c_str());

            m_stProtoMonitorConfigMap[stMonitor.m_iCmd] = std::move(stMonitor);
        }
    }while (0);

    if (strErrorInfo.str().empty())
    {
        return (std::pair<bool, std::string>(true, "OK"));
    }
    else
    {
        return (std::pair<bool, std::string>(false, strErrorInfo.str()));
    }
}

const TProtoMonitor* CLogicConfigProtocolMonitorParser::GetConfig(int32_t iCmd) const
{
    const auto iter = m_stProtoMonitorConfigMap.find(iCmd);

    return iter == m_stProtoMonitorConfigMap.end() ? nullptr : &iter->second;
}
