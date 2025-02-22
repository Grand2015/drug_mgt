#ifndef __COMMON_H__
#define __COMMON_H__

#include "c_std_inc.h"


#define HWW_TRUE					(1)
#define HWW_FALSE					(0)
#define HWW_OK						(0)
#define HWW_ERROR					(-1)

#define _15_DAYS_S (15*24*60*60)
#define _30_DAYS_S (30*24*60*60)

#define MIN(A,B)  (A)<=(B)?(A):(B)
#define MAX(A,B)  (A)<=(B)?(B):(A)


typedef enum{
    HWW_DEBUG_LEVEL_ERR = 0,
    HWW_DEBUG_LEVEL_INFO,
    HWW_DEBUG_LEVEL_HINT
}HWW_DEBUG_LEVEL_E;

#define HWW_PRINT(log_level, format, ...) \
    log_print(log_level, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)

long long get_timestamp();
long long format_time_to_timestamp(struct tm* ptimeinfo);

void HWW_INFO(const char* format, ...);
void HWW_HINT(const char* format, ...);
void HWW_ERR(const char* format, ...);

void log_print(HWW_DEBUG_LEVEL_E log_level, const char* func, int line, const char* format, ...);

void hww_print_init(HWW_DEBUG_LEVEL_E debug_level);

#endif