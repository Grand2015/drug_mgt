/*
APP NAME:   YU SHAN FANG
APP AUTH:   GRAND LI
DATE    :   2025/01/29
*/

#include "c_std_inc.h"
#include "sys_common.h"
#include "durgMgt.h"
#include "shell_server.h"
#include "storage.h"
#include "mail.h"

void print_sysinfo()
{
    printf("\n*****************************************\n");
    printf(SYS_INFO, SYS_NAME, SYS_AUTH, CREAT_DATE);
    printf("*****************************************\n\n");

    return;    
}

int main()
{
    int ret = 0;
    print_sysinfo();

    shell_cmd_server();

    if (storage_server_deal() < 0)
    {
        return -1;
    }

    drug_mgt_deal();

    mail_server_deal();

    printf("WELCOME TO YUSHANFANG.\n\n");

    while(1)
    {
        sleep(1);
    }

    return 0;
}