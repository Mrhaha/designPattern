#include <fstream>
#include "common_datetime.h"
#include "logic_link_common.h"

int CLogicCommon::m_iTimeOffset = 0;
int CLogicCommon::m_iNowSec = 0;
int CLogicCommon::m_iNowMSec = 0;

struct tm CLogicCommon::m_stNowDate;

std::string         CLogicCommon::m_strNowDate;
std::random_device  CLogicCommon::m_stRandDevice;
std::mt19937        CLogicCommon::m_stMt19937(m_stRandDevice());

CLogicCommon::unique_seq_t CLogicCommon::m_stLastSeq;

struct CFlushTimeOnStart {
    CFlushTimeOnStart()
    {
        CLogicCommon::FlushTime();
    }
};

CFlushTimeOnStart stFlushTimeOnStart;

bool 
CLogicCommon::IsCurrentDay(int iTimestamp)
{
    struct tm stDate;
    time_t stTimestamp = iTimestamp;
    ::localtime_r(&stTimestamp, &stDate);
    return ((stDate.tm_year == m_stNowDate.tm_year) && (stDate.tm_yday == m_stNowDate.tm_yday));
}

bool CLogicCommon::IsRefreshByDay(int iTimestamp,int iRefreshHour)
{
    struct tm stDate;
    time_t stTimestamp = iTimestamp;
    ::localtime_r(&stTimestamp, &stDate);

    if (stDate.tm_year != m_stNowDate.tm_year)
    {
        return true;
    } else
    {
        if (m_stNowDate.tm_yday - stDate.tm_yday > 1)
        {
            return true;
        } else if (m_stNowDate.tm_yday - stDate.tm_yday == 1)
        {
            if (stDate.tm_hour < iRefreshHour)
            {
                return true;
            }
            else if (m_stNowDate.tm_hour >= iRefreshHour)
            {
                return true;
            }
        } else if (m_stNowDate.tm_yday == stDate.tm_yday)
        {
            if (m_stNowDate.tm_hour >= iRefreshHour && stDate.tm_hour < iRefreshHour) return true;
        }
    }
    return false;
}

bool CLogicCommon::IsRefreshByWeek(int iTimestamp,int iRefreshHour)
{
    struct tm stNow;
    time_t stNowTimestamp = m_iNowSec;
    ::localtime_r(&stNowTimestamp, &stNow);

    stNow.tm_hour = 0;
    stNow.tm_min = 0;
    stNow.tm_sec = 0;

    time_t day_time = mktime(&stNow);
    int week = stNow.tm_wday;
    if (0 == week) week = 7;

    day_time -= (int)((week - 1) * 1_day); //获取星期一的0点时间戳
    day_time += (int)(iRefreshHour * 1_hour); //获取对应的刷新时间戳
    time_t last_day_time = day_time - 7_day; //获取上次的刷新周期

    if (iTimestamp < last_day_time || (m_iNowSec >= day_time && iTimestamp < day_time)) return true;
    return false;
};

bool CLogicCommon::IsRefreshByMonth(int iTimestamp,int iRefreshHour)
{
    struct tm stNow;
    time_t stNowTimestamp = m_iNowSec;
    ::localtime_r(&stNowTimestamp, &stNow);

    stNow.tm_hour = 0;
    stNow.tm_min = 0;
    stNow.tm_sec = 0;
    stNow.tm_mday = 1;

    time_t day_time = mktime(&stNow);
    day_time += (int)(iRefreshHour * 1_hour); //获取对应的刷新时间戳
    time_t last_day_time = day_time - 31_day; //获取上次的刷新周期

    if (iTimestamp < last_day_time || (m_iNowSec >= day_time && iTimestamp < day_time)) return true;
    return false;
}

bool CLogicCommon::IsCurrentWeek(int iTimestamp)
{
	return is_same_week(iTimestamp, m_iNowSec);
}

bool CLogicCommon::IsCurrentMonth(int iTimestamp)
{
	return is_same_month(iTimestamp, m_iNowSec);
}
//判断当前时间戳是否大于当天指定hour的时间戳
bool CLogicCommon::IsOverTheAimClock(int hour)
{
    int aimTimeStamp = get_hour_timestamp(m_iNowSec,hour);
    return m_iNowSec >= aimTimeStamp;
}

bool CLogicCommon::IsOverTargetDay(int iTimestamp,int day)
{
    struct tm stDate;
    time_t stTimestamp = iTimestamp;
    ::localtime_r(&stTimestamp, &stDate);
    return ((stDate.tm_year == m_stNowDate.tm_year) && (m_stNowDate.tm_yday > stDate.tm_yday + day));
}

bool CLogicCommon::IsWithinCycleDay(int32_t iNowTime, int32_t iStartDatetime, int32_t iStillDay, int32_t iStopDay)
{
	int32_t iDistanceDay = day_diff(iStartDatetime, iNowTime) + 1;
	if (iDistanceDay <= 0)
	{
		return false;
	}

	const int32_t iCycleDay = iStillDay + iStopDay;
	if (iCycleDay <= 0)
	{
		return true;
	}

	iDistanceDay %= iCycleDay;
	return iDistanceDay <= iStillDay;
}

int32_t CLogicCommon::GetRandNum()
{
	return m_stMt19937() & 0x7fffffff;
}

int32_t CLogicCommon::GetRandNum(int32_t iMin, int32_t iMax, bool bClosedInterval/* = false*/)
{
    if(iMin == iMax) return (iMin);

    int32_t iEx = (bClosedInterval ? 1 : 0);
    
    if(iMin > iMax)
    {
		std::swap(iMin, iMax);
    }

    return (iMin + (GetRandNum() % (iMax - iMin + iEx)));
}

CLogicCommon::unique_seq_t CLogicCommon::GetUniqueSeq()
{
	if (m_stLastSeq.m_iTime != m_iNowSec)
	{
		m_stLastSeq.m_iTime = m_iNowSec;
		m_stLastSeq.m_iSeqID = 1;		
	}
	else
	{
		++m_stLastSeq.m_iSeqID;
	}

	return m_stLastSeq;
}

