
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

HWW_DEBUG_LEVEL_E g_debug_level = HWW_DEBUG_LEVEL_ERR;
// 定义颜色代码
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_RESET   "\033[0m"

void HWW_INFO(const char* format, ...)
{
    if(g_debug_level >= HWW_DEBUG_LEVEL_INFO)
    {
        va_list args;
        va_start(args, format);

        printf("%s[INFO][%s][%d]", COLOR_BLUE, __func__, __LINE__);
        vprintf(format, args);
        printf("%s", COLOR_RESET);

        va_end(args);
    }
    return;
}



void log_print(HWW_DEBUG_LEVEL_E log_level, const char* func, int line, const char* format, ...)
{
    if (g_debug_level < log_level) return;

    va_list args;
    va_start(args, format);

    switch (log_level)
    {
        case HWW_DEBUG_LEVEL_ERR:            
            printf("%s[ERROR][%s][%d]", COLOR_RED, func, line);
            break;
        case HWW_DEBUG_LEVEL_INFO:
            printf("%s[INFO][%s][%d]", COLOR_BLUE, func, line);
            break;
        case HWW_DEBUG_LEVEL_HINT:
            printf("%s[HINT][%s][%d]", COLOR_GREEN, func, line);
            break;
        default:
            break;
    }

    vprintf(format, args);
    printf("%s", COLOR_RESET);

    va_end(args);

    return;
}

void HWW_HINT(const char* format, ...)
{
    if(g_debug_level >= HWW_DEBUG_LEVEL_HINT)
    {
        va_list args;
        va_start(args, format);

        printf("%s[HINT][%s][%d]", COLOR_GREEN, __func__, __LINE__);
        vprintf(format, args);
        printf("%s", COLOR_RESET);

        va_end(args);
    }

    return;
}


void HWW_ERR(const char* format, ...)
{
    if(g_debug_level >= HWW_DEBUG_LEVEL_ERR)
    {
        va_list args;
        va_start(args, format);

        printf("%s[ERROR][%s][%d]", COLOR_RED, __func__, __LINE__);
        vprintf(format, args);
        printf("%s", COLOR_RESET);

        va_end(args);
    }

    return;
}


void hww_print_init(HWW_DEBUG_LEVEL_E debug_level)
{
	if (debug_level < HWW_DEBUG_LEVEL_ERR || debug_level > HWW_DEBUG_LEVEL_HINT) return;

	g_debug_level = debug_level;

	return;
}
