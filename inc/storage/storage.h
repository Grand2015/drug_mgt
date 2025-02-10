#ifndef __STORAGE_H__
#define __STORAGE_H__

#include "durgMgt.h"

#define DRUG_INFO_DB    "/home/wendy/share/01_project/15_drugMgt/03_data/drug_info.db"
#define DRUG_DB_NAME    "drug_info.db"


#define CREATE_DRUG_INFO_TABLE  "CREATE TABLE drug_info("  \
         "id            CHAR(20)    PRIMARY KEY     NOT NULL," \
         "name          CHAR(32)," \
         "item          CHAR(32)," \
         "manufacture_date       CHAR(12)," \
         "expiration_date         CHAR(12)," \
         "indications       CHAR(32)," \
         "disable_description         CHAR(32));"

#define INSERT_DRUG_INFO  "INSERT INTO drug_info (id, name, item, manufacture_date, expiration_date, indications, disable_description) "  \
         "VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s');" \

#define SELECT_ALL_DRUG_INFO "SELECT id, name, item, manufacture_date, expiration_date, indications, disable_description FROM drug_info;"

#define DELETE_DRUG_INFO_BY_ID  "DELETE FROM drug_info WHERE id = '%s';"

int storage_server_deal();

int storage_server_insert_drug_info(DRUG_INFO_T *pstDrugInfo);
DRUG_INFO_T* storage_server_select_drug_info();
int storage_delete_drug_info_by_id(char *id);


#endif