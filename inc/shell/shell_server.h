#ifndef __SHELL_SERVER_H__
#define __SHELL_SERVER_H__


#define SHELL_CMD_LEN 16
#define SHELL_CMD_MAX 32
#define SHELL_COMMIT_LEN 64
#define SHELL_ARG_LEN 256
#define SHELL_INPUT_LEN 256

typedef int (*cmdFun_cb)(char*, int);

typedef struct {
    char cmd[SHELL_CMD_LEN]; //
    char commit[SHELL_COMMIT_LEN];
    cmdFun_cb cmd_fun;
}SHELL_CMD_INFO_T;

typedef struct{
    char cmd[SHELL_CMD_LEN];
    char arg[SHELL_ARG_LEN];
}SHELL_MSG_T;

int shell_cmd_server();

#endif