#pragma once

#include "msgpack.hpp"

#include <ctime>
#include <sys/time.h>

#include <string>
#include <random>
#include <algorithm>
#include "common_platform.h"

class CLogicCommon
{
public:

	struct unique_seq_t
	{
		int32_t m_iTime = 0;//时间
		int32_t m_iSeqID = 0;//序列
	};

	static __FORCE_INLINE__ void SetDebugTimeOffset(int32_t iOffset)
    {
	    m_iTimeOffset = iOffset;
    }

    static __FORCE_INLINE__ void FlushTime()
    {
        struct timeval stNow;
        ::gettimeofday(&stNow, nullptr);

        m_iNowSec = stNow.tv_sec;
        m_iNowMSec = (int)(stNow.tv_usec / 1000);

#if LOGIC_BUILD_PLATFORM == BUILD_PLATFORM_LOCAL
        m_iNowSec += m_iTimeOffset;
#endif

        time_t iNow = (time_t)m_iNowSec;
        ::localtime_r((const time_t*)&iNow, &m_stNowDate);
    }
    
    static __FORCE_INLINE__ int GetSec()
    {
        return (m_iNowSec);
    }
    
    static __FORCE_INLINE__ int GetMSec()
    {
        return (m_iNowMSec);
    }
    
    static __FORCE_INLINE__ const struct tm& GetDate()
    {
        return (m_stNowDate);
    }

	static __FORCE_INLINE__ std::string GetDateString()
    {
		char szBuffer[128];
		snprintf(szBuffer, sizeof(szBuffer),
			"%04d-%02d-%02d",
			m_stNowDate.tm_year + 1900,
			m_stNowDate.tm_mon + 1,
			m_stNowDate.tm_mday);
		m_strNowDate = szBuffer;
		return m_strNowDate;
    }

    static __FORCE_INLINE__ std::string GetDateTimeString()
    {
        char szBuffer[128];
        snprintf(szBuffer, sizeof(szBuffer),
                "%04d-%02d-%02d %02d:%02d:%02d",
                m_stNowDate.tm_year + 1900,
                m_stNowDate.tm_mon + 1,
                m_stNowDate.tm_mday,
                m_stNowDate.tm_hour,
                m_stNowDate.tm_min,
                m_stNowDate.tm_sec);
        m_strNowDate = szBuffer;
        return m_strNowDate;
    }

    static bool IsRefreshByDay(int iTimestamp,int iRefreshHour);

    static bool IsRefreshByWeek(int iTimestamp,int iRefreshHour);

    static bool IsRefreshByMonth(int iTimestamp,int iRefreshHour);

    static bool IsCurrentDay(int iTimestamp);

	static bool IsCurrentWeek(int iTimestamp);

	static bool IsCurrentMonth(int iTimestamp);

    static bool IsOverTheAimClock(int hour=0);

    static bool IsOverTargetDay(int iTimestamp,int day);

    static bool IsWithinCycleDay(int32_t iNowTime, int32_t iStartDatetime, int32_t iStillDay, int32_t iStopDay);

	static int32_t GetRandNum();

    // bClosedInterval: false 半闭区间随机数 [iMin, iMax), true 闭区间随机数 [iMin, iMax]
    static int32_t GetRandNum(int32_t iMin, int32_t iMax, bool bClosedInterval = false);

	static unique_seq_t GetUniqueSeq();

private:
    static int                  m_iTimeOffset;
    static int                  m_iNowSec;
    static int                  m_iNowMSec;
    static struct tm            m_stNowDate;
    static std::string          m_strNowDate;
    static std::random_device   m_stRandDevice;
    static std::mt19937         m_stMt19937;
	static unique_seq_t			m_stLastSeq;
};

#define LOGIC_CHECK_DAY_REFRESH(timestamp) (CLogicCommon::IsRefreshByDay(timestamp,EVERYDAY_REFRESH_HOUR))
#define LOGIC_CHECK_WEEK_REFRESH(timestamp) (CLogicCommon::IsRefreshByWeek(timestamp,EVERYDAY_REFRESH_HOUR))
#define LOGIC_CHECK_MONTH_REFRESH(timestamp) (CLogicCommon::IsRefreshByMonth(timestamp,EVERYDAY_REFRESH_HOUR))

