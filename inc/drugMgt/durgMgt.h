#ifndef __DRUG_MGT_H__
#define __DRUG_MGT_H__

#define DRUG_NAME_LEN 32
#define DRUG_ITEM_LEN 32
#define DRUG_DATE_LEN 12
#define DRUG_COMMIT_LEN 32

#define DRUG_NUM_MAX 1000


#define DRUG_PARAM_MAX 6
#define DRUG_PARAM_LEN 20

#define DRUG_ID_LEN  20


typedef struct{
    char name[DRUG_NAME_LEN];
    char item[DRUG_ITEM_LEN];
    char manufacture_date[DRUG_DATE_LEN];
    char expiration_date[DRUG_DATE_LEN];
    char indications[DRUG_COMMIT_LEN];
    char disable_description[DRUG_COMMIT_LEN];
    char id[DRUG_ID_LEN]; //唯一标识
}DRUG_INFO_T;

void drug_mgt_deal();
DRUG_INFO_T* get_drug_info();
DRUG_INFO_T* get_drug_info_from_ram();
int insert_drug_info(DRUG_INFO_T *pstDrugInfo);


#endif