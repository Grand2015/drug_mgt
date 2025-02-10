
#include "common.h"


long long get_timestamp()//获取时间戳函数
{
    struct timeval tv;

    gettimeofday(&tv, NULL);

    return tv.tv_sec;
}

long long format_time_to_timestamp(struct tm* ptimeinfo) 
{
	struct tm timeinfo;
    time_t timestamp;

    // 设置时间信息 这里示例 2023/08/08 17:43:11
	timeinfo.tm_year = ptimeinfo->tm_year - 1900; // 年份需要减去1900
    timeinfo.tm_mon = ptimeinfo->tm_mon - 1; // 月份是从0开始的，所以需要减去1
    timeinfo.tm_mday = ptimeinfo->tm_mday; // 日
    timeinfo.tm_hour = ptimeinfo->tm_hour - 8; // 时
    timeinfo.tm_min = ptimeinfo->tm_min; // 分
    timeinfo.tm_sec = ptimeinfo->tm_sec; // 秒
    // 转换为时间戳
    timestamp = mktime(&timeinfo);
	//printf("ts :%ld\n", timestamp);
	return timestamp;
}
