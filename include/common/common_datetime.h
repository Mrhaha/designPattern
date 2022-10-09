#pragma once

#include <sys/time.h>
#include <utime.h>

#include "common_base.h"

enum
{
	usec_of_one_second = 1000000,	//
};

struct stDateTime
{
	int32_t m_iYear;
	int32_t m_iMon;
	int32_t m_iDay;
	int32_t m_iHour;
	int32_t m_iMin;
	int32_t m_iSec;
	int32_t m_iMSec;
};

int32_t get_string_datetime(const time_t time, char* pszBuffer);
int32_t get_struct_datetime(const time_t time, stDateTime& out);

int32_t get_current_string_datetime(char* pszBuffer);
int32_t get_current_struct_datetime(stDateTime& out);

int32_t get_current_string_date(char* strDate);
int32_t get_string_from_struct_datetime(const stDateTime& stDate, char* pszBuffer);

int32_t get_date_integer(time_t time);
std::string get_date_string(time_t time);
std::string get_datetime_string(time_t time);

bool try_time_from_string(const std::string& string_time, time_t& out);
bool try_time_from_string(const std::string& string_time, int32_t& out);
time_t get_time_from_string(const std::string& string_time);

//以秒和毫秒为单位的当前时间
time_t now_sec(void);
time_t now_msec(void);

//获取该时间的周一凌晨0点的时间值
time_t get_monday_time(const time_t time);
//获取该时间当天0点的时间值
time_t get_day_begin_time(const time_t time);
//两个时间是否在同一天。是在同一天，则返回true；否则返回false
bool is_same_day(const time_t time1, const time_t time2);
//两个时间是否在同一周。是在同一周，则返回true；否则返回false
bool is_same_week(const time_t time1, const time_t time2);
//两个时间是否在同一月。是在同一月，则返回true；否则返回false
bool is_same_month(const time_t time1, const time_t time2);
//该事件是否是今天。是今天，则返回true；否则返回false
bool is_today(const time_t time1);
//是否跨过了周几几点
bool is_cross_weekday(time_t time_1, int32_t wday, int32_t hour);
//返回星期几（0：周日；1：周一；2：周二；...；6：周六）
int32_t weekday(const time_t time);
//返回月份(1...12)
int32_t month(const time_t time);
//返回每个月的几号
int32_t month_day(const time_t time);
//返回从0001-01-01至今的天数
int32_t since_days(int32_t year, int32_t month, int32_t day);
//返回两个时间戳相隔的天数
int32_t day_diff(time_t start, time_t end);
//根据时间日期格式化文件名
int32_t format_file_name(char *FileName, size_t FileNameLength, const char *szDir, const char *szPrefix, time_t tmTimeStamp);
//在时间区间内，小时为单位
bool is_in_time_interval(int32_t iBeginHour, int32_t iEndHour);
//获取该时间当天某个整点小时的时间戳
time_t get_hour_timestamp(time_t time, int32_t hour = 0);
//获取该时间戳这一周某一星期的时间戳
time_t get_week_day_timestamp(time_t time, int32_t week, int32_t hour = 0);
//判断两个日期是否跨过了整点时间
bool is_pass_some_hour(time_t begin_time, time_t end_time, int32_t hour);
bool is_pass_some_hour(time_t begin_time, time_t end_time, const std::vector<int32_t>& hours);

//定义时间的字面值量，默认获取到以秒为单位的数值
//1分=60秒
//1小时=60分
//1天=24小时
//1月=30天
//1年=365天
inline unsigned long long operator""_sec(unsigned long long num)
{
	return num;
}

inline unsigned long long operator""_min(unsigned long long num)
{
	return num * 60_sec;
}

inline unsigned long long operator""_hour(unsigned long long num)
{
	return num * 60_min;
}

inline unsigned long long operator""_day(unsigned long long num)
{
	return num * 24_hour;
}

inline unsigned long long operator""_mon(unsigned long long num)
{
	return num * 30_day;
}

inline unsigned long long operator""_year(unsigned long long num)
{
	return num * 365_day;
}

bool IsRefreshByDay(int iNowTimestamp, int iTimestamp,int iRefreshHour) ;
