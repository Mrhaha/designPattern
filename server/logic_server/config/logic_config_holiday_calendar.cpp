//
// Created by lxhbroser on 2019/12/27.
//

#include "logic_config_holiday_calendar.h"
#include "server_errcode.h"
#include "Markup.h"
#include "common_datetime.h"

std::pair<bool, std::string> CLogicConfigHolidayCalendarParser::Load(const std::string &strFileName, const std::string &strXMLContent)
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

    if(false == stXML.FindElem("calendar"))
    {
      strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:" << stXML.GetResult() << std::endl;
      break;
    }

    stXML.IntoElem();
    while (stXML.FindElem("year"))
    {
      int32_t iYear = (int32_t)atoi(stXML.GetAttrib("year").c_str());
      stXML.IntoElem();
      while (stXML.FindElem("month"))
      {
        int8_t byMonth = (int8_t)atoi(stXML.GetAttrib("month").c_str());
        stXML.IntoElem();
        while (stXML.FindElem("day"))
        {
          int8_t byDay = (int8_t)atoi(stXML.GetAttrib("day").c_str());
          int8_t byState = (int8_t)atoi(stXML.GetAttrib("state").c_str());
          if(byState < (int8_t)CEHolidayStateType::EHST_Normal || byState > (int8_t)CEHolidayStateType::EHST_Holiday)
          {
            strErrorInfo << CONFIG_ERROR_MSG_HEADER << "MSG:Holiday State not Match! YEAR:" << iYear
                         << "|MONTH:" << byMonth << "|DAY:" << byDay << "|STATE:" << byState << std::endl;
            return (std::pair<bool, std::string>(false, strErrorInfo.str()));
          }

          m_stHolidayCalendarMap[iYear][byMonth][byDay] = byState;
        }

        stXML.OutOfElem();
      }

      stXML.OutOfElem();
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

int8_t CLogicConfigHolidayCalendarParser::GetHolidayCalendarStat(int32_t iYear, int8_t byMonth, int8_t byDay) const
{
  auto iterYear = m_stHolidayCalendarMap.find(iYear);
  if(iterYear != m_stHolidayCalendarMap.end())
  {
    auto iterMonth = iterYear->second.find(byMonth);
    if(iterMonth != iterYear->second.end())
    {
      auto iterDay = iterMonth->second.find(byDay);
      if(iterDay != iterMonth->second.end())
        return iterDay->second;
    }
  }

  return (int8_t)CEHolidayStateType::EHST_Normal;
}
