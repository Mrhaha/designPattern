//
// Created by lxhbroser on 2019/8/10.
//

#include "logic_config_special_gift.h"
#include "Markup.h"
#include "common_datetime.h"
#include "logic_config.h"
#include "logic_link_common.h"

std::pair<bool, std::string>
CLogicConfigSpecialGiftParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

        if(false == stXML.FindElem("activity_config"))
        {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
            throw std::exception();
        }

        stXML.IntoElem();
        while(stXML.FindElem("activity"))
        {
            TLogicSpecialGift stGift;

            stGift.m_iActivityID = (int32_t)atoi(stXML.GetAttrib("activity_id").c_str());

            stXML.IntoElem();
            if(false == stXML.FindElem("desc"))
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << "|ACTIVITY_ID:" << stGift.m_iActivityID << std::endl;
                throw std::exception();
            }

            stGift.m_stActivityRef.m_iType      = atoi(stXML.GetAttrib("type").c_str());
            if(!IsValidAvyType(stGift.m_stActivityRef.m_iType))
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:INVALID ACTIVITY_TYPE:" << stGift.m_stActivityRef.m_iType
                            << "|ACTIVITY_ID:" << stGift.m_iActivityID <<std::endl;
                throw std::exception();
            }

            stGift.m_stActivityRef.m_iTimeType  = atoi(stXML.GetAttrib("time_type").c_str());
            stGift.m_stActivityRef.m_iStartDay  = atoi(stXML.GetAttrib("start_day").c_str());
            stGift.m_stActivityRef.m_iEndDay    = atoi(stXML.GetAttrib("end_day").c_str());
            stGift.m_stActivityRef.m_iLevel     = atoi(stXML.GetAttrib("level").c_str());
            stGift.m_stActivityRef.m_iServerDay = atoi(stXML.GetAttrib("server_day").c_str());

            std::string strDate;
            strDate = stXML.GetAttrib("start_date");
            if(!strDate.empty())
            {
                struct tm stTm = {};
                if(strptime(strDate.c_str(), "%Y-%m-%d %T", &stTm) == nullptr)
                {
                    strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:FORMAT TIME ERROR|ACTIVE ID:" << stGift.m_iActivityID << std::endl;
                    throw std::exception();
                }
                stGift.m_stActivityRef.m_iStartTime = (int)mktime(&stTm);
            }

            strDate = stXML.GetAttrib("end_date");
            if(!strDate.empty())
            {
                struct tm stTm = {};
                if(strptime(strDate.c_str(), "%Y-%m-%d %T", &stTm) == nullptr)
                {
                    strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:FORMAT TIME ERROR|ACTIVE ID:" << stGift.m_iActivityID << std::endl;
                    throw std::exception();
                }
                stGift.m_stActivityRef.m_iEndTime = (int)mktime(&stTm);
            }

            if(stGift.m_stActivityRef.m_iStartDay != 0 && stGift.m_stActivityRef.m_iStartTime != 0)
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:StartDay and StartTime can't both be valid! ID:" << stGift.m_iActivityID << std::endl;
                throw std::exception();
            }

            if(false == stXML.FindElem("data"))
            {
                strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" <<  stXML.GetResult() << "|ACTIVITY_ID:" << stGift.m_iActivityID << std::endl;
                throw std::exception();
            }

            stXML.IntoElem();
            while(stXML.FindElem("info"))
            {
                TLogicSpecialGiftInfo stInfo;
                stInfo.m_iLevelMin = (int32_t)atoi(stXML.GetAttrib("level_min").c_str());
                for (int i = 1; i <= 10; ++i)
                {
                    std::stringstream stStream;
                    stStream << "deposit_id_" << i;
                    if(stXML.GetAttrib(stStream.str()).empty()) break;

                    int32_t iDepositID = (int32_t)atoi(stXML.GetAttrib(stStream.str()).c_str());
                    CONFIG_ASSERT_EXCEPTION(iDepositID > 0);

                    stInfo.m_stDepositIDList.push_back(iDepositID);
                }

                stGift.m_stInfoMap[stInfo.m_iLevelMin] = stInfo;
            }
            stXML.OutOfElem();

            if(stGift.m_stActivityRef.m_iType == CLogicConfigDefine::ESGT_Week)
            {
                m_stSpecialGiftWeekMap[stGift.m_iActivityID] = stGift;
            }
            else if(stGift.m_stActivityRef.m_iType == CLogicConfigDefine::ESGT_Month)
            {
                m_stSpecialGiftMonthMap[stGift.m_iActivityID] = stGift;
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

bool CLogicConfigSpecialGiftParser::IsValidDepositID(int32_t iType, int32_t iAvyID, int32_t iLevel, int32_t iDepositID) const
{
    if(!IsValidAvyType(iType)) return false;

    const_iterator avyIter;
    if(iType == CLogicConfigDefine::ESGT_Week)
    {
        avyIter = m_stSpecialGiftWeekMap.find(iAvyID);
        if(avyIter == m_stSpecialGiftWeekMap.end())
            return false;
    }
    else
    {
        avyIter = m_stSpecialGiftMonthMap.find(iAvyID);
        if(avyIter == m_stSpecialGiftMonthMap.end())
            return false;
    }

    auto levelIter = avyIter->second.m_stInfoMap.find(iLevel);
    if(levelIter == avyIter->second.m_stInfoMap.end())
        return false;

    return std::find(levelIter->second.m_stDepositIDList.begin(), levelIter->second.m_stDepositIDList.end(), iDepositID) != levelIter->second.m_stDepositIDList.end();
}
