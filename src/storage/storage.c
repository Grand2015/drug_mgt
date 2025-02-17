#include "c_std_inc.h"
#include "storage.h"
#include "sqlite3.h"
//#include "sqlite3ext.h"

static sqlite3 *drug_info_db;

sqlite3 ** storage_server_get_db_handle()
{
    return &drug_info_db;
}


int storage_server_insert_drug_info(DRUG_INFO_T *pstDrugInfo)
{
    sqlite3 **db = storage_server_get_db_handle();
    char sql[512] = {0};
    char* zErrMsg = 0;
    int rc = 0;

    if (NULL == pstDrugInfo) return -1;

    sprintf(sql, INSERT_DRUG_INFO, pstDrugInfo->id, pstDrugInfo->name, pstDrugInfo->item, \
        pstDrugInfo->manufacture_date, pstDrugInfo->expiration_date, pstDrugInfo->indications, pstDrugInfo->disable_description);

    printf("sql:%s\n", sql);
    rc = sqlite3_exec(*db, sql, NULL, 0, &zErrMsg);
    if (SQLITE_OK != rc)
    {
        printf("[%s:%d]SQLITE3 EXEC FAILED(%d,%s).\n", __FUNCTION__, __LINE__, rc, zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }   

    return 0;
}


DRUG_INFO_T* storage_server_select_drug_info()
{
    DRUG_INFO_T * drug_info = get_drug_info_from_ram();
    sqlite3 **db = storage_server_get_db_handle();
    int rc = 0;
    char* zErrMsg = 0;
    char sql[512] = {0};
    sqlite3_stmt *stmt;
    int index = 0;
    const unsigned char *result = NULL;
    char *param_point = NULL;


    sprintf(sql, "%s", SELECT_ALL_DRUG_INFO);
    
    rc = sqlite3_prepare_v2(*db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) 
    {
        printf("[%s:%d]SQLITE3 EXEC FAILED(%d:%s).\n", __FUNCTION__, __LINE__, rc, zErrMsg);
        return NULL;
    }

    memset(drug_info, 0, sizeof(DRUG_INFO_T)*DRUG_NUM_MAX);

    // 遍历查询结果
    while (sqlite3_step(stmt) == SQLITE_ROW) 
    {
        result = sqlite3_column_text(stmt, 0);
        memcpy(drug_info[index].id, result, strlen(result));

        result = sqlite3_column_text(stmt, 1);
        memcpy(drug_info[index].name, result, strlen(result));

        result = sqlite3_column_text(stmt, 2);
        memcpy(drug_info[index].item, result, strlen(result));
        
        result = sqlite3_column_text(stmt, 3);
        memcpy(drug_info[index].manufacture_date, result, strlen(result));

        result = sqlite3_column_text(stmt, 4);
        memcpy(drug_info[index].expiration_date, result, strlen(result));

        result = sqlite3_column_text(stmt, 5);
        memcpy(drug_info[index].indications, result, strlen(result));

        result = sqlite3_column_text(stmt, 6);
        memcpy(drug_info[index].disable_description, result, strlen(result));

        index++;
    }

    // 释放语句对象
    sqlite3_finalize(stmt);

    return drug_info;
}


int storage_delete_drug_info_by_id(char *id)
{
    sqlite3 **db = storage_server_get_db_handle();
    char sql[512] = {0};
    char* zErrMsg = 0;
    int rc = 0;

    if (NULL == id) return -1;

    sprintf(sql, DELETE_DRUG_INFO_BY_ID, id);

    printf("sql:%s\n", sql);
    rc = sqlite3_exec(*db, sql, NULL, 0, &zErrMsg);
    if (SQLITE_OK != rc)
    {
        printf("[%s:%d]SQLITE3 EXEC FAILED(%d,%s).\n", __FUNCTION__, __LINE__, rc, zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }   

    return 0;
}

int storage_server_create_database()
{
    sqlite3 **db = storage_server_get_db_handle();
    int rc = 0;
    char* zErrMsg = 0;
    char sql[512] = {0};
    int isExist = 0;

    char pwd_url[128] = {0};
    char db_url[256] = {0};
    
    if (getcwd(pwd_url, sizeof(pwd_url)) != NULL) 
    {
        //printf("当前目录: %s\n", pwd_url);
        sprintf(db_url, "%s/test/%s", pwd_url, DRUG_DB_NAME);
    } 
    else 
    {
        printf("[%s:%d] getcwd() err\n", __FUNCTION__, __LINE__);
        return -1;
    }

    isExist = (0 == access(db_url, F_OK)) ? 1 : 0;

    rc = sqlite3_open(db_url, db);
    if (SQLITE_OK != rc)
    {
        printf("[%s:%d]SQLITE3 OPEN FAILED(%d).\n", __FUNCTION__, __LINE__, rc);
        return -1;
    }

    if (!isExist)
    {
        sprintf(sql, "%s", CREATE_DRUG_INFO_TABLE);
        rc = sqlite3_exec(*db, sql, NULL, 0, &zErrMsg);
        if (SQLITE_OK != rc)
        {
            printf("[%s:%d]SQLITE3 EXEC FAILED(%d:%s).\n", __FUNCTION__, __LINE__, rc, zErrMsg);
            sqlite3_free(zErrMsg);
            return -1;
        }   
    }

    return 0;
}


int storage_server_deal()
{
    int ret = 0;
    ret = storage_server_create_database();
    if (0 != ret)
    {
        printf("[%s:%d]CREATE DATABASE FAILED(%d).\n", __FUNCTION__, __LINE__, ret);
        return -1;
    }


    return 0;
}