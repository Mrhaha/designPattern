//
// Created by luoxiaohu on 2021/8/5.
//

#include "logic_config_define.h"
#include "logic_config_base.h"

bool CLogicConfigCommonFunc::ParserAvyOpeningElem(const CMarkup& stXML, TLogicAvyOpeningElem& stElem)
{
    stElem.m_iOpeningType = ::atoi(stXML.GetAttrib("opening_type").c_str());
    if(!CLogicConfigDefine::IsValidEnumAvyOpeningType(stElem.m_iOpeningType))
        return false;

    if(stElem.m_iOpeningType == CLogicConfigDefine::EAOT_AbsoluteDate)
    {
        std::string strDate;
        strDate = stXML.GetAttrib("opening_para1");
        if (!strDate.empty())
        {
            struct tm stTM = {};
            if (strptime(strDate.c_str(), "%Y-%m-%d %T", &stTM) == nullptr)
            {
                return false;
            }
            stElem.m_iOpeningPara1 = (int)mktime(&stTM);
        }

        strDate = stXML.GetAttrib("opening_para2");
        if (!strDate.empty())
        {
            struct tm stTM = {};
            if (strptime(strDate.c_str(), "%Y-%m-%d %T", &stTM) == nullptr)
            {
                return false;
            }
            stElem.m_iOpeningPara2 = (int)mktime(&stTM);
        }
    }
    else
    {
        stElem.m_iOpeningPara1 = ::atoi(stXML.GetAttrib("opening_para1").c_str());
        stElem.m_iOpeningPara2 = ::atoi(stXML.GetAttrib("opening_para2").c_str());
    }

    if (!try_time_from_string(stXML.GetAttrib("custom_create_time"), stElem.m_iCustomCreateTime))
    {
        return false;
    }

    if(stElem.m_iOpeningPara2 < stElem.m_iOpeningPara1)
        return false;

    return true;
}

void CLogicConfigUtil::CombineGameItem(std::vector<CPackGameItem>& stItemList, const CPackGameItem& stItem)
{
    for (size_t i = 0; i < stItemList.size(); ++i)
    {
        if (stItemList[i].m_iItemType == stItem.m_iItemType &&
            stItemList[i].m_iCardID == stItem.m_iCardID)
        {
            stItemList[i].m_iNum += stItem.m_iNum;
            return;
        }
    }

    stItemList.push_back(stItem);
}
