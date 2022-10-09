//
// Created by lxhbroser on 2019/7/3.
//

#include "logic_config_proto_switch.h"
#include "server_errcode.h"
#include "Markup.h"
#include "common_datetime.h"

std::pair<bool, std::string>
CLogicConfigProtoSwitchParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

        if(false == stXML.FindElem("switch_list"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            break;
        }

        stXML.IntoElem();
        while (true == stXML.FindElem("switch"))
        {
            int32_t iCmd = ::atoi(stXML.GetAttrib("cmd").c_str());
            if(IsValidCmd(iCmd) == false)
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:Invalid CMD:" << iCmd << std::endl;
                break;
            }

            if(m_stProtoSwitchConfigMap.find(iCmd) != m_stProtoSwitchConfigMap.end())
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:Repeated CMD:" << iCmd << std::endl;
                break;
            }

            int32_t dateTime = ::atoi(stXML.GetAttrib("expire").c_str());
            if (dateTime <= 0)
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << "expire <= 0" << std::endl;
                break;
            }

            m_stProtoSwitchConfigMap[iCmd] = dateTime;
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

int32_t CLogicConfigProtoSwitchParser::GetConfig(int32_t iCmd) const
{
    const auto dateTime = m_stProtoSwitchConfigMap.find(iCmd);

    return dateTime == m_stProtoSwitchConfigMap.end() ? 0 : dateTime->second;
}
