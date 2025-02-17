#ifndef __COMMON_H__
#define __COMMON_H__

#include "c_std_inc.h"

#define _15_DAYS_S (15*24*60*60)
#define _30_DAYS_S (30*24*60*60)

#define MIN(A,B)  (A)<=(B)?(A):(B)
#define MAX(A,B)  (A)<=(B)?(B):(A)



long long get_timestamp();
long long format_time_to_timestamp(struct tm* ptimeinfo);


#endif