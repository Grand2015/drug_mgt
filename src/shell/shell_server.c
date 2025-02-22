#include "c_std_inc.h"
#include "shell_server.h"
#include "common.h"
#include "durgMgt.h"
#include "storage.h"
#include "mail.h"


static SHELL_CMD_INFO_T stShellCMD[SHELL_CMD_MAX] = {0};
static mqd_t shell_mqt = -1;


SHELL_CMD_INFO_T* get_global_shell_cmd()
{
    return stShellCMD;
}

mqd_t shell_get_mqt()
{
    return shell_mqt;
}


static int shell_help(char *cmd, int cmd_len)
{
    int index = 0;
    int i = 1;
    int len = 0;

    for(index=0; index < SHELL_CMD_MAX; index++)
    {
        len = strlen(stShellCMD[index].cmd);
        if (0 != len)
        {       
            if (len <= 4)
            {
                printf("[%d]%s\t\t\t:%s\n", i, stShellCMD[index].cmd, stShellCMD[index].commit);
            } 
            else
            {
                printf("[%d]%s\t\t:%s\n", i, stShellCMD[index].cmd, stShellCMD[index].commit);
            }
            i++;
        }
    }
    return 0;
}


static int shell_get_drug_param_point_by_index(DRUG_INFO_T* drug_point, char** point, int index)
{
    if (index >= DRUG_PARAM_MAX) return 0;

    switch(index)
    {
        case 0:*point = drug_point->name;break;
        case 1:*point = drug_point->item;break;
        case 2:*point = drug_point->manufacture_date;break;
        case 3:*point = drug_point->expiration_date;break;
        case 4:*point = drug_point->indications;break;
        case 5:*point = drug_point->disable_description;break;
        default:break;
    }

    return 0;
}

static int shell_get_full_data(DRUG_INFO_T *pstDrugInfo, char *arg, int len)
{
    char *param_point = NULL;
    char *v_arg = arg;
    char *colon_point = NULL;
    int index = 0;
    
    while(len > 0)
    {
        colon_point = strstr(v_arg, "+");
        if (NULL != colon_point)
        {
            shell_get_drug_param_point_by_index(pstDrugInfo, &param_point, index);
            memcpy(param_point, v_arg, colon_point-v_arg);
            index++;

            v_arg = colon_point+1;
            len = len - (colon_point-v_arg)-1;
        }
        else
        {
            shell_get_drug_param_point_by_index(pstDrugInfo, &param_point, index);
            memcpy(param_point, v_arg, len);
            break;
        }
    }

    return 0;
}

static int shell_insert_durg_info(char *arg, int arg_len)
{
    static DRUG_INFO_T s_stDrugInfo = {0};
    char durg_param[DRUG_PARAM_MAX][DRUG_PARAM_LEN] = {"name", "item", "manufacture_date", "expiration_date", "indications", "disable_description"};
    int index = 0;
    char *param_point = NULL;

    if (0 == arg_len)
    {
        printf("### INSERT MODE ###\n");
        printf("insert name:xxx \t:INSERT DRUG NAME IS XXX\n");
        printf("insert item:xxx \t:INSERT DRUG ITEM IS XXX\n");
        printf("insert manufacture_date:xxxx-xx-xx \t:INSERT DRUG MANUFACTURE DATE IS xxxx-xx-xx\n");
        printf("insert expiration_date:xxxx-xx-xx \t:INSERT DRUG EXPIRATION DATE IS xxxx-xx-xx\n");
        printf("insert indications:xxx \t:INSERT DRUG INDICATIONS IS XXX\n");
        printf("insert disable_description:xxx \t:INSERT DRUG DISABLE DESCRIPTION IS XXX\n");
        printf("insert quit \t:QUIT INSERT MODE\n");
        printf("insert full_data:[name]+[item]+[manufacture_date]+[expiration_date]+[indications]+[disable_description]\n");

        memset(&s_stDrugInfo, 0, sizeof(s_stDrugInfo));
    }
    else
    {
        if (NULL != strstr(arg, "quit"))
        {
            HWW_PRINT(HWW_DEBUG_LEVEL_INFO, "QUIT INSERT MODE.\n");
            if(0 != strlen(s_stDrugInfo.name))
            {
                insert_drug_info(&s_stDrugInfo);
            }
            return 0;
        }
        else if (NULL != strstr(arg, "full_data"))
        {
            shell_get_full_data(&s_stDrugInfo, arg+strlen("full_data")+1, arg_len-(strlen("full_data")+1));
            HWW_PRINT(HWW_DEBUG_LEVEL_HINT, "[%d]\nname:\t%s\nitem:\t%s\nmanufacture_date:\t%s\nexpiration_date:\t%s\nindications:\t%s\ndisable_description:\t%s\n",
                index, s_stDrugInfo.name, s_stDrugInfo.item, s_stDrugInfo.manufacture_date, s_stDrugInfo.expiration_date,
                s_stDrugInfo.indications, s_stDrugInfo.disable_description);

            return 0;
        }
        else
        {
            for(index = 0; index < DRUG_PARAM_MAX; index++)
            {
                if (NULL != strstr(arg, durg_param[index]))
                {  
                    HWW_PRINT(HWW_DEBUG_LEVEL_HINT, "param is %s\n", durg_param[index]);
                    shell_get_drug_param_point_by_index(&s_stDrugInfo, &param_point, index);
                    
                    if (NULL != param_point)
                    {
                        memcpy(param_point, arg+strlen(durg_param[index])+1, arg_len-(strlen(durg_param[index])+1));
                        param_point = NULL;
                        HWW_PRINT(HWW_DEBUG_LEVEL_HINT, "[%d]\nname:\t%s\nitem:\t%s\nmanufacture_date:\t%s\nexpiration_date:\t%s\nindications:\t%s\ndisable_description:\t%s\n",
                            index, s_stDrugInfo.name, s_stDrugInfo.item, s_stDrugInfo.manufacture_date, s_stDrugInfo.expiration_date,
                            s_stDrugInfo.indications, s_stDrugInfo.disable_description);
                    }
                    else
                    {
                        HWW_PRINT(HWW_DEBUG_LEVEL_ERR, "param_point is NULL \n");
                    }
                }        
            }
        }
    }
   
    return 0;
}


static int shell_cmd_reg(char *cmd_str, void *cmd_deal, char *commit)
{
    int index = 0;
    for (index=0; index < SHELL_CMD_MAX; index++)
    {
        if(0 == strlen(stShellCMD[index].cmd))
        {
            memcpy(stShellCMD[index].cmd, cmd_str, strlen(cmd_str));
            memcpy(stShellCMD[index].commit, commit, strlen(commit));
            stShellCMD[index].cmd_fun = cmd_deal;
            HWW_PRINT(HWW_DEBUG_LEVEL_HINT, "CMD [%s] REG SUCCESSFULLY.\n", cmd_str);
            return 0;
        }
    }

    if(index >= SHELL_CMD_MAX)
    {
        HWW_PRINT(HWW_DEBUG_LEVEL_ERR, "SHELL CMD IS OUTOF MAX!\n");
        return -1;
    }

    return 0;    
}


static int shell_show_durg_info()
{
    DRUG_INFO_T* stDrugInfo = get_drug_info();
    int index = 0;
    
    for (index = 0; index < DRUG_NUM_MAX; index++)
    {
        if(0 != strlen(stDrugInfo[index].name))
        {
            HWW_PRINT(HWW_DEBUG_LEVEL_HINT, "[%s]\nname:\t%s\nitem:\t%s\nmanufacture_date:\t%s\nexpiration_date:\t%s\nindications:\t%s\ndisable_description:\t%s\n",
            stDrugInfo[index].id, stDrugInfo[index].name, stDrugInfo[index].item, stDrugInfo[index].manufacture_date, stDrugInfo[index].expiration_date,
            stDrugInfo[index].indications, stDrugInfo[index].disable_description);
        }
    }
    return 0;
}


static int shell_select_durg_info(char *arg, int arg_len)
{
    HWW_PRINT(HWW_DEBUG_LEVEL_INFO, "HAVE NOT DONE\n");
    return 0;
}

static int shell_del_durg_info(char *arg, int arg_len)
{
    HWW_PRINT(HWW_DEBUG_LEVEL_INFO, "ID IS %s\n", arg);
    storage_delete_drug_info_by_id(arg);

    return 0;
}


static int shell_send_mail(char *arg, int arg_len)
{
    HWW_PRINT(HWW_DEBUG_LEVEL_INFO, "TEST MAIL IS %s\n", arg);
    
    //mail_server_send_mail(arg, NULL, NULL);
    mail_server_send_mail("hongweiwei2023@163.com", "Test Email 02", "This is a test email sent from C language.");
    return 0;
}

static int shell_set_log_level(char *arg, int arg_len)
{
    HWW_PRINT(HWW_DEBUG_LEVEL_INFO, "LEVEL IS %s\n", arg);
    HWW_DEBUG_LEVEL_E debug_level = HWW_DEBUG_LEVEL_ERR;
    
    if (0 == strcmp("error", arg))
    {
        debug_level = HWW_DEBUG_LEVEL_ERR;
    }
    else if (0 == strcmp("info", arg))
    {
        debug_level = HWW_DEBUG_LEVEL_INFO;
    }
    else if (0 == strcmp("hint", arg))
    {
        debug_level = HWW_DEBUG_LEVEL_HINT;
    }
    else
    {
        HWW_PRINT(HWW_DEBUG_LEVEL_ERR, "LEVEL OF %s IS ERR\n", arg);
    }

    hww_print_init(debug_level);

    return 0;
}

static int shell_cmd_reg_deal()
{
    shell_cmd_reg("help", shell_help, "SHOW ALL CMD");

    shell_cmd_reg("insert", shell_insert_durg_info, "SWITCH TO INSERT MODE.");

    shell_cmd_reg("show", shell_show_durg_info, "LIST OF DRUG INFO.");

    shell_cmd_reg("select", shell_select_durg_info, "SELECT DRUG INFO.");

    shell_cmd_reg("delete", shell_del_durg_info, "DELETE DRUG INFO BY ID.");

    shell_cmd_reg("email", shell_send_mail, "SEND TEST EMAIL");

    shell_cmd_reg("log_level", shell_set_log_level, "SET LOG LEVEL");

    
    return 0;
}

static int get_keyboard_input(char *cmd, int cmd_len, char* arg, int arg_len)
{
    int ch = 0;
    char str[SHELL_INPUT_LEN] = {0};
    int index = 0;
    char *pos = NULL;
    
    while((ch = getchar()) != '\n' && ch != EOF)
    {
        if (index >= SHELL_INPUT_LEN) break;
        str[index] = ch;
        index++;
    }

    HWW_HINT("INPUT DATA:%s\n", str);

    pos = strstr(str, " ");
    if (NULL != pos)
    {
        memcpy(cmd, str, MIN(pos-str, cmd_len));
        memcpy(arg, pos+1, MIN(strlen(str)-(pos-str+1), arg_len));
        HWW_PRINT(HWW_DEBUG_LEVEL_INFO, "CMD:%s, ARG:%s\n", cmd, arg);
    }
    else
    {
        memcpy(cmd, str, MIN(strlen(str), cmd_len));
    }


    return 0;
}

static int shell_cmd_listion_pthread()
{
    char cmd[SHELL_CMD_LEN] = {0};
    char arg[SHELL_ARG_LEN] = {0};
    SHELL_MSG_T stShellMsg = {0};

    while(1)
    {
        memset(cmd, 0, SHELL_CMD_LEN);
        memset(arg, 0, SHELL_ARG_LEN);

        get_keyboard_input(cmd, SHELL_CMD_LEN, arg, SHELL_ARG_LEN);
        if (0 != strlen(cmd))
        {
            memset(&stShellMsg, 0, sizeof(stShellMsg));
            memcpy(stShellMsg.cmd, cmd, strlen(cmd));
            memcpy(stShellMsg.arg, arg, strlen(arg));
            
            /* send cmd */
            if (mq_send(shell_mqt, (const char *)&stShellMsg, sizeof(stShellMsg), 0) < 0)
            {
                HWW_PRINT(HWW_DEBUG_LEVEL_ERR, "MQ_SEND ERR!\n");
            }
            else
            {
                HWW_PRINT(HWW_DEBUG_LEVEL_INFO, "MQ_SEND SUCCESSFULLY[%s]\n", stShellMsg.cmd);
            }
            
        }
        usleep(1000*20);
    }

    return 0;
}

static int shell_cmd_listion()
{
    pthread_t thread;
    int ret = 0;

    ret = pthread_create(&thread, NULL, (void *)shell_cmd_listion_pthread, 0);
    if (0 != ret)
    {
        HWW_PRINT(HWW_DEBUG_LEVEL_ERR, "PTHREAD_CREAT ERR(%d)\n", ret);
        return -1;
    }

    //pthread_join(thread, NULL);

    return 0;
}


static int shell_cmd_process_deal(SHELL_MSG_T *msg)
{
    SHELL_MSG_T stShellMsg = {0};
    int index = 0;
    SHELL_CMD_INFO_T* shell_cmd = get_global_shell_cmd();

    memcpy(&stShellMsg, msg, sizeof(stShellMsg));
    
    for(index = 0; index < SHELL_CMD_MAX; index++)
    {
        if (0 == strcmp(shell_cmd[index].cmd, msg->cmd))
        {
            if (shell_cmd[index].cmd_fun)
            {
                shell_cmd[index].cmd_fun(msg->arg, strlen(msg->arg));
            }
            else
            {
                HWW_PRINT(HWW_DEBUG_LEVEL_ERR, "THE PROC FUN OF CMD[%s] IS UNREG.\n", msg->cmd);
            }

            break;
        }
    }
    
    if (index >= SHELL_CMD_MAX)
    {
        HWW_PRINT(HWW_DEBUG_LEVEL_ERR, "THE CMD[%s] IS UNREG.\n", msg->cmd);
    }

    return 0;
}


static int shell_cmd_process_pthread()
{
    SHELL_MSG_T stShellMsg = {0};
    int ret = 0;

    while(1)
    {
        memset(&stShellMsg, 0, sizeof(stShellMsg));
        ret = mq_receive(shell_mqt, (char *)&stShellMsg, sizeof(stShellMsg), NULL);
        if (ret == sizeof(stShellMsg))
        {
            shell_cmd_process_deal(&stShellMsg);
        }

        usleep(1000*10);
    }

    return 0;
}


static int shell_cmd_process()
{
    pthread_t thread;
    int ret = 0;

    ret = pthread_create(&thread, NULL, (void *)shell_cmd_process_pthread, 0);
    if (0 != ret)
    {
        HWW_PRINT(HWW_DEBUG_LEVEL_ERR, "PTHREAD_CREAT ERR(%d)\n", ret);
        return -1;
    }

    //pthread_join(thread, NULL);

    return 0;
}


static int shell_mq_init()
{
    struct mq_attr attr = {0};
    #define SHELL_MQ ("/shell_mq")

    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(SHELL_MSG_T);

    mq_unlink(SHELL_MQ);
    shell_mqt = mq_open(SHELL_MQ, O_CREAT|O_RDWR, 0644, &attr);
    if (-1 == shell_mqt)
    {
        HWW_PRINT(HWW_DEBUG_LEVEL_ERR, "MQ_OPEN ERR!\n");
        return -1;
    }

    HWW_PRINT(HWW_DEBUG_LEVEL_INFO, "SHELL MQ(%d) INIT SUCCESSFULLY.\n", shell_mqt);

    return 0;
}


static int shell_cmd_proc()
{
    int ret = 0;

    /* mq init */
    if (shell_mq_init() < 0 )
    {
        return -1;
    }

    /* listion */
    shell_cmd_listion();

    /* process */
    shell_cmd_process();

    return 0;
}


int shell_cmd_server()
{
    shell_cmd_reg_deal();
    
    shell_cmd_proc();

    return 0;
}