//
// Created by lxhbroser on 2019/12/27.
//

#pragma once

#include "logic_config_base.h"

enum class CEHolidayStateType
{
  EHST_Normal   = 0,    // 工作日
  EHST_DayOff   = 1,    // 休息日
  EHST_Holiday  = 2,    // 法定节假日
};

class CLogicConfigHolidayCalendarParser : public CLogicConfigParserBase
{
public:
  CLogicConfigHolidayCalendarParser() : CLogicConfigParserBase() {}

  std::pair<bool, std::string> Load(const std::string &strFileName, const std::string &strXMLContent) override;

  int8_t GetHolidayCalendarStat(int32_t iYear, int8_t byMonth, int8_t byDay) const;

  std::map<int32_t, std::map<int8_t, std::map<int8_t, int8_t>>> m_stHolidayCalendarMap;
};
