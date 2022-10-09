#include "common_base.h"
#include "common_datetime.h"
#include "common_platform.h"

int32_t get_string_datetime(const time_t time, char* pszBuffer)
{
	if (NULL == pszBuffer)
	{
		return fail;
	}

	struct tm stTime;
	struct tm* pstTime = NULL;
	pstTime = localtime_r(&time, &stTime);
	if (NULL == pstTime)
	{
		return fail;
	}

	sprintf(pszBuffer, "%04d-%02d-%02d %02d:%02d:%02d",
		stTime.tm_year + 1900, stTime.tm_mon + 1, stTime.tm_mday,
		stTime.tm_hour, stTime.tm_min, stTime.tm_sec);

	return success;
}

int32_t get_struct_datetime(const time_t time, stDateTime& out)
{
	struct tm *pstTime = NULL;
	struct tm stTime;
	time_t tTmp = time;

	
	pstTime = localtime_r(&tTmp, &stTime);
	if(NULL == pstTime)
	{
		return fail;
	}

	out.m_iYear = stTime.tm_year + 1900;
	out.m_iMon = stTime.tm_mon + 1;
	out.m_iDay = stTime.tm_mday;
	out.m_iHour = stTime.tm_hour;
	out.m_iMin = stTime.tm_min;
	out.m_iSec = stTime.tm_sec;
	out.m_iMSec = 0;

	return success;
}

int32_t get_current_string_datetime(char* pszBuffer)
{
	time_t now;
	time(&now);

	return get_string_datetime((const time_t) now, pszBuffer);
}

int32_t get_current_struct_datetime(stDateTime& out)
{
	time_t now;
	time(&now);
	return get_struct_datetime((const time_t)now, out);
}

int32_t get_current_string_date(char* strDate)
{
	struct tm *pstTm = NULL;
	time_t now  =0;

	if(NULL == strDate)
	{
		return fail;
	}

	time(&now);
	strDate[0] = '\0';

	pstTm = localtime(&now);
	if(NULL == pstTm)
	{
		return fail;
	}

	sprintf(strDate, "%04d-%02d-%02d", pstTm->tm_year + 1900, pstTm->tm_mon + 1, pstTm->tm_mday);
	return success;
}

int32_t get_string_from_struct_datetime(const stDateTime& stDate,char* pszBuffer)
{
	if (NULL == pszBuffer)
	{
		return  fail;
	}
	sprintf(pszBuffer, "%04d-%02d-%02d %02d:%02d:%02d",
		stDate.m_iYear , stDate.m_iMon, stDate.m_iDay,
		stDate.m_iHour, stDate.m_iMin, stDate.m_iSec);
	return  success;
}

void plus_timevalue(struct timeval& src, const struct timeval& plus)
{
	timeval tvTemp;
	tvTemp.tv_sec = src.tv_sec + plus.tv_sec;
	tvTemp.tv_sec += ((src.tv_usec+plus.tv_usec)/usec_of_one_second);
	tvTemp.tv_usec = ((src.tv_usec+plus.tv_usec)%usec_of_one_second);

	src.tv_sec = tvTemp.tv_sec;
	src.tv_usec = tvTemp.tv_usec;
}

void minus_timevalue(struct timeval& src, const struct timeval& minus)
{
	timeval tvTemp;

	if( src.tv_usec < minus.tv_usec )
	{
		tvTemp.tv_usec = (usec_of_one_second + src.tv_usec) - minus.tv_usec;
		tvTemp.tv_sec = src.tv_sec - minus.tv_sec - 1;
	}
	else
	{
		tvTemp.tv_usec = src.tv_usec - minus.tv_usec;
		tvTemp.tv_sec  = src.tv_sec - minus.tv_sec;
	}

	src.tv_sec = tvTemp.tv_sec;
	src.tv_usec = tvTemp.tv_usec;
}

int32_t get_date_integer(time_t time)
{
	struct tm stDate;
	::localtime_r(&time, &stDate);

	char szBuffer[128] = { 0 };
	snprintf(szBuffer, sizeof(szBuffer),
		"%04d%02d%02d",
		stDate.tm_year + 1900,
		stDate.tm_mon + 1,
		stDate.tm_mday);
	return atoi(szBuffer);
}

std::string get_date_string(time_t time)
{
	struct tm stDate;
	::localtime_r(&time, &stDate);

	char szBuffer[128] = { 0 };
	snprintf(szBuffer, sizeof(szBuffer),
		"%04d-%02d-%02d",
		stDate.tm_year + 1900,
		stDate.tm_mon + 1,
		stDate.tm_mday);
	return szBuffer;
}

std::string get_datetime_string(time_t time)
{
	struct tm stDate;
	::localtime_r(&time, &stDate);

	char szBuffer[128] = { 0 };
	snprintf(szBuffer, sizeof(szBuffer),
		"%04d-%02d-%02d %02d:%02d:%02d",
		stDate.tm_year + 1900,
		stDate.tm_mon + 1,
		stDate.tm_mday,
		stDate.tm_hour,
		stDate.tm_min,
		stDate.tm_sec);
	return szBuffer;
}

bool try_time_from_string(const std::string& string_time, time_t& out)
{
	if (!string_time.empty())
	{
		struct tm stTM = {};
		if (strptime(string_time.c_str(), "%Y-%m-%d %T", &stTM) == nullptr)
		{
			return false;
		}
		out = mktime(&stTM);
	}
	else
	{
		out = 0;
	}
	return true;
}

bool try_time_from_string(const std::string& string_time, int32_t& out)
{
	if (!string_time.empty())
	{
		struct tm stTM = {};
		if (strptime(string_time.c_str(), "%Y-%m-%d %T", &stTM) == nullptr)
		{
			return false;
		}
		out = static_cast<int32_t>(mktime(&stTM));
	}
	else
	{
		out = 0;
	}
	return true;
}

time_t get_time_from_string(const std::string& string_time)
{
	time_t out = 0;
	if (!try_time_from_string(string_time, out))
	{
		out = 0;
	}
	return out;
}

time_t now_sec(void)
{
	return time(nullptr);
}

time_t now_msec(void)
{
	timeval tv_time = { };
	::gettimeofday(&tv_time, nullptr);
	return tv_time.tv_sec*1000 + tv_time.tv_usec/1000;
}

time_t get_monday_time(const time_t time)
{
	tm stTmpTime = { };
	localtime_r(&time, &stTmpTime);
	stTmpTime.tm_hour = 0;
	stTmpTime.tm_min = 0;
	stTmpTime.tm_sec = 0;

	time_t day_time = mktime(&stTmpTime);
	int week = stTmpTime.tm_wday;
	if (0 == week)
	{
		week = 7;
	}

	day_time -= (week - 1) * 86400;

	return (day_time);
}

//获取该时间当天0点的时间值
time_t get_day_begin_time(const time_t time)
{
	tm stTmpTime = { };
	localtime_r(&time, &stTmpTime);
	stTmpTime.tm_hour = 0;
	stTmpTime.tm_min = 0;
	stTmpTime.tm_sec = 0;

	return mktime(&stTmpTime);
}

bool is_same_day(const time_t time1, const time_t time2)
{
	tm stTmpTime1 = { };
	tm stTmpTime2 = { };
	localtime_r(&time1, &stTmpTime1);
	localtime_r(&time2, &stTmpTime2);

	return stTmpTime1.tm_year == stTmpTime2.tm_year 
		&& stTmpTime1.tm_mon == stTmpTime2.tm_mon 
		&& stTmpTime1.tm_mday == stTmpTime2.tm_mday;
}

bool is_same_week(const time_t time1, const time_t time2)
{
	const time_t monday_time1 = get_monday_time(time1);
	const time_t monday_time2 = get_monday_time(time2);
	return monday_time1 == monday_time2;
}

bool is_same_month(const time_t time1, const time_t time2)
{
	tm stTmpTime1 = { };
	tm stTmpTime2 = { };
	localtime_r(&time1, &stTmpTime1);
	localtime_r(&time2, &stTmpTime2);

	return stTmpTime1.tm_year == stTmpTime2.tm_year && stTmpTime1.tm_mon == stTmpTime2.tm_mon;
}

bool is_today(const time_t time1)
{
	return is_same_day(time1, time(nullptr));
}

bool is_cross_weekday(time_t time_1, int32_t wday, int32_t ihour)
{
	tm timeStamp1 = { };
	time_t tmNow = time(nullptr);
	localtime_r(&tmNow, &timeStamp1);

	timeStamp1.tm_hour = ihour;
	timeStamp1.tm_min = 0;
	timeStamp1.tm_sec = 0;

	time_t wday_seconds = mktime(&timeStamp1) + 24 * 3600 * (wday - timeStamp1.tm_wday);
	time_t last_wday_seconds = wday_seconds - (tmNow < wday_seconds ? 7 * 24 * 3600 : 0);
	return time_1 < last_wday_seconds;
}

int32_t weekday(const time_t time)
{
	tm timeStamp1 = { };
	localtime_r(&time, &timeStamp1);

	return timeStamp1.tm_wday;
}

int32_t month(const time_t time)
{
	tm timeStamp1 = { };
	localtime_r(&time, &timeStamp1);

	return timeStamp1.tm_mon + 1;
}

int32_t month_day(const time_t time)
{
	tm timeStamp1 = { };
	localtime_r(&time, &timeStamp1);

	return timeStamp1.tm_mday;
}

int32_t since_days(int32_t year, int32_t month, int32_t day)
{
	if (month < 3)
		year--, month += 12;
	return 365 * year + year / 4 - year / 100 + year / 400 + (153 * month - 457) / 5 + day - 306;
}

int32_t day_diff(time_t start, time_t end)
{
	tm tmStart, tmEnd;
	localtime_r(&start, &tmStart);
	localtime_r(&end, &tmEnd);

	return since_days(tmEnd.tm_year, tmEnd.tm_mon + 1, tmEnd.tm_mday)
		- since_days(tmStart.tm_year, tmStart.tm_mon + 1, tmStart.tm_mday);
}

int32_t format_file_name(char *FileName, size_t FileNameLength, const char *szDir, const char *szPrefix, time_t tmTimeStamp)
{
	tm lasttime = { };
	localtime_r(&tmTimeStamp, &lasttime);
	return snprintf(FileName, FileNameLength, "./%s/%s_%04d-%02d-%02d.txt", szDir, szPrefix, lasttime.tm_year + 1900, lasttime.tm_mon + 1, lasttime.tm_mday);
}

bool is_in_time_interval(int32_t iBeginHour, int32_t iEndHour)
{
	time_t tmNow = time(NULL);
	stDateTime stNowDateTime;
	get_struct_datetime(tmNow, stNowDateTime);
	if (iBeginHour <= stNowDateTime.m_iHour && stNowDateTime.m_iHour <= iEndHour)
	{
		return true;
	}
	else
	{
		return false;
	}
}

time_t get_hour_timestamp(time_t time, int32_t hour)
{
	struct tm stDate;
	::localtime_r(&time, &stDate);

	stDate.tm_hour = hour;
	stDate.tm_min = 0;
	stDate.tm_sec = 0;
	return mktime(&stDate);
}

time_t get_week_day_timestamp(time_t time, int32_t week, int32_t hour)
{
	time = get_monday_time(time);
	if (week == 0)
	{
		week = 7;
	}
	return time + (week - 1) * 1_day + hour * 1_hour;
}

bool is_pass_some_hour(time_t begin_time, time_t end_time, int32_t hour)
{
	return is_pass_some_hour(begin_time, end_time, std::vector<int32_t>{ hour });
}

bool is_pass_some_hour(time_t begin_time, time_t end_time, const std::vector<int32_t>& hours)
{
	for (const auto& hour : hours)
	{
		time_t refresh_time = get_hour_timestamp(end_time, hour);
		if (refresh_time > end_time)
		{
			refresh_time -= 1_day;
		}
		if (refresh_time > begin_time)
		{
			return true;
		}
	}
	return false;
}


bool IsRefreshByDay(int iNowTimestamp,int iTimestamp,int iRefreshHour)
{
    struct tm stDate;
    struct tm stNowDate;
    time_t tmNow = iNowTimestamp;
    time_t stTimestamp = iTimestamp;
    ::localtime_r(&stTimestamp, &stDate);
    ::localtime_r(&tmNow, &stNowDate);

    if (stDate.tm_year != stNowDate.tm_year)
    {
        return true;
    } else
    {
        if (stNowDate.tm_yday - stDate.tm_yday > 1)
        {
            return true;
        } else if (stNowDate.tm_yday - stDate.tm_yday == 1)
        {
            if (stDate.tm_hour < iRefreshHour)
            {
                return true;
            }
            else if (stNowDate.tm_hour >= iRefreshHour)
            {
                return true;
            }
        } else if (stNowDate.tm_yday == stDate.tm_yday)
        {
            if (stNowDate.tm_hour >= iRefreshHour && stDate.tm_hour < iRefreshHour) return true;
        }
    }
    return false;
}

bool IsRefreshByWeek(int iTimestamp,int iRefreshHour)
{
    struct tm stDate;
    time_t stTimestamp = iTimestamp;
    ::localtime_r(&stTimestamp, &stDate);

    stDate.tm_hour = 0;
    stDate.tm_min = 0;
    stDate.tm_sec = 0;

    time_t day_time = mktime(&stDate);
    int week = stDate.tm_wday;
    if (0 == week) week = 7;

    day_time -= (week - 1) * 1_day; //获取星期一的0点时间戳
    day_time += iRefreshHour * 1_hour; //获取对应的刷新时间戳
    int last_day_time = day_time - 7_day; //获取上次的刷新周期

    if (iTimestamp < last_day_time || now_sec() > day_time) return true;
    return false;
};

