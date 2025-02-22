#include "durgMgt.h"
#include "c_std_inc.h"
#include "common.h"
#include "storage.h"

static DRUG_INFO_T g_stDrugInfo[DRUG_NUM_MAX] = {0};


DRUG_INFO_T* get_drug_info_from_db()
{
    return storage_server_select_drug_info();
}

static int insert_drug_info_to_db(DRUG_INFO_T *pstDrugInfo)
{
    sprintf(pstDrugInfo->id, "%lld", get_timestamp());
    return storage_server_insert_drug_info(pstDrugInfo);
}

static int insert_drug_info_to_ram(DRUG_INFO_T *pstDrugInfo)
{
    if (NULL == pstDrugInfo) return -1;
    
    int index = 0;
    DRUG_INFO_T* stDrugInfo = get_drug_info_from_ram();

    for (index=0; index < DRUG_NUM_MAX; index++)
    {
        if (0 == strlen(stDrugInfo[index].id))
        {
            sprintf(pstDrugInfo->id, "%lld", get_timestamp());
            memcpy(&stDrugInfo[index], pstDrugInfo, sizeof(DRUG_INFO_T));
            return 0;
        }
    }

    if (index >= DRUG_NUM_MAX)
    {
        HWW_PRINT(HWW_DEBUG_LEVEL_ERR, "DRUG INFO OUT OF RANGE.\n");
        return -1;
    }

    return 0;
}



int insert_drug_info(DRUG_INFO_T *pstDrugInfo)
{
    #ifdef SUPPORT_SQILTE3
    return insert_drug_info_to_db(pstDrugInfo);
    #else
    return insert_drug_info_to_ram(pstDrugInfo);
    #endif
}


DRUG_INFO_T* get_drug_info_from_ram()
{
    return g_stDrugInfo;
}


DRUG_INFO_T* get_drug_info()
{
    #ifdef SUPPORT_SQILTE3
    return get_drug_info_from_db();
    #else
    return get_drug_info_from_ram();
    #endif
}

static void drug_mgt_init()
{
    DRUG_INFO_T* stDrugInfo = get_drug_info_from_ram();

    memset(stDrugInfo, 0, DRUG_NUM_MAX*sizeof(DRUG_INFO_T));

    return;   
}

static long long drug_get_time_stamp_of_expiration_date(char *pdate, int len)
{
    struct tm timeinfo = {0};

    if (len > 10)
    {
        sscanf(pdate, "%04d-%02d-%02d %02d:%02d:%02d", &timeinfo.tm_year, &timeinfo.tm_mon, &timeinfo.tm_mday, &timeinfo.tm_hour, &timeinfo.tm_min, &timeinfo.tm_sec);
    }
    else
    {
        sscanf(pdate, "%04d-%02d-%02d", &timeinfo.tm_year, &timeinfo.tm_mon, &timeinfo.tm_mday);
        //timeinfo.tm_hour = 8;
    }

    return format_time_to_timestamp(&timeinfo);
}

static int drug_detect_expiration_date_pthread()
{
    DRUG_INFO_T* stDrugInfo = get_drug_info();
    int index = 0;

    long long cur_time = 0;
    long long expiration_date = 0;

    while(1)
    {
        cur_time = get_timestamp();

        for(index = 0; index < DRUG_NUM_MAX; index++)
        {
            if (0 != strlen(stDrugInfo[index].name))
            {
                expiration_date = drug_get_time_stamp_of_expiration_date(stDrugInfo[index].expiration_date, strlen(stDrugInfo[index].expiration_date));
                if (expiration_date < cur_time)
                {
                    HWW_PRINT(HWW_DEBUG_LEVEL_ERR, "THE DRUG(%s) IS OUT OF DATE.\n",  stDrugInfo[index].id);                    
                }
                else if (expiration_date - cur_time <= _30_DAYS_S)
                {
                    HWW_PRINT(HWW_DEBUG_LEVEL_ERR, "THE DRUG(%s) HAS 30 DAYS LEFT TO EXPIRE.\n", stDrugInfo[index].id);
                }
            }
        }

        sleep(5);
    }

    return 0;
}

static int drug_detect_expiration_date()
{
    pthread_t thread;
    int ret = 0;

    ret = pthread_create(&thread, NULL, (void *)drug_detect_expiration_date_pthread, 0);
    if (0 != ret)
    {
        HWW_PRINT(HWW_DEBUG_LEVEL_ERR, "PTHREAD_CREAT ERR(%d)\n", ret);
        return -1;
    }

    return 0;
}


static int drug_mgt_start()
{
    drug_detect_expiration_date();

    return 0;
}



void drug_mgt_deal()
{

    drug_mgt_init();

    drug_mgt_start();
    
    return;
}