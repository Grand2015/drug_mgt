#include "c_std_inc.h"
#include "mail.h"
#include "base64.h"
#include "common.h"

#define BUFFER_SIZE 1024

#define SMTP_SERVER "smtp.qq.com"
#define SMTP_PORT_587 587
#define USERNAME "997812940@qq.com" // 替换为你的QQ邮箱
#define PASSWORD "kholuazkwjotbcai"     // 替换为你的邮箱授权码

#define MAIL_EHLO_CMD           "EHLO example.com\r\n"
#define MAIL_START_TTLS_CMD     "STARTTLS\r\n"
#define MAIL_AUTH_LOGIN_CMD     "AUTH LOGIN\r\n"
#define MAIL_MAIL_FROM_CMD      "MAIL FROM:<%s>\r\n"
#define MAIL_RCPT_TO_CMD        "RCPT TO:<%s>\r\n"
#define MAIL_DATA_CMD           "DATA\r\n"
#define MAIL_QUIT_CMD           "QUIT\r\n"
#define MAIL_CONNECT_CMD        "From: %s\r\nTo: %s\r\nSubject: %s\r\n\r\n%s\r\n.\r\n"
#define MAIL_ENTER_CMD           "\r\n"


typedef enum{
    MAIL_SEND_CMD_ATTR_R = 0,
    MAIL_SEND_CMD_ATTR_W,
    MAIL_SEND_CMD_ATTR_WR
}MAIL_SEND_CMD_ATTR_E;


static int mail_server_init()
{
    return 0;
}


void mail_server_deal()
{
    mail_server_init();
    
    return;
}


// Base64编码函数
char *base64_encode(const char *input) {
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);
    BIO_write(bio, input, strlen(input));
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);
    char *encoded = (char *)malloc(bufferPtr->length + 1);
    memcpy(encoded, bufferPtr->data, bufferPtr->length);
    encoded[bufferPtr->length] = '\0';
    BIO_free_all(bio);
    return encoded;
}

// 发送SMTP命令并读取响应
static void send_smtp_command_ssl(SSL *ssl, const char *command, MAIL_SEND_CMD_ATTR_E eAttr) 
{
    if (MAIL_SEND_CMD_ATTR_W == eAttr || MAIL_SEND_CMD_ATTR_WR == eAttr)
    {
        HWW_PRINT(HWW_DEBUG_LEVEL_HINT, "C: %s", command);
        SSL_write(ssl, command, strlen(command));
    }

    if (MAIL_SEND_CMD_ATTR_R == eAttr || MAIL_SEND_CMD_ATTR_WR == eAttr)
    {
        char response[BUFFER_SIZE] = {0};
        int len = SSL_read(ssl, response, sizeof(response) - 1);
        response[len] = '\0';
        HWW_PRINT(HWW_DEBUG_LEVEL_HINT, "S: %s", response);
    }
    
    return;
}

static void send_smtp_command(int sockfd, const char *command, MAIL_SEND_CMD_ATTR_E eAttr) 
{
    if (MAIL_SEND_CMD_ATTR_W == eAttr || MAIL_SEND_CMD_ATTR_WR == eAttr)
    {
        if (NULL != command)
        {
            HWW_PRINT(HWW_DEBUG_LEVEL_HINT, "C: %s", command);
            write(sockfd, command, strlen(command));
        }
    }

    if (MAIL_SEND_CMD_ATTR_R == eAttr || MAIL_SEND_CMD_ATTR_WR == eAttr)
    {
        char response[BUFFER_SIZE] = {0};
        int len = read(sockfd, response, sizeof(response) - 1);
        response[len] = '\0';
        HWW_PRINT(HWW_DEBUG_LEVEL_HINT, "S: %s", response);
    }

    return;
}


int mail_server_send_mail(char *to_mail, char *title, char *mail_body) 
{
    // 创建套接字
    int sockfd = -1;
    struct sockaddr_in server_addr;
    struct hostent *server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        HWW_PRINT(HWW_DEBUG_LEVEL_ERR, "Socket creation failed");
        return -1;
    }

    server = gethostbyname(SMTP_SERVER);
    if (server == NULL) 
    {
        HWW_PRINT(HWW_DEBUG_LEVEL_ERR, "DNS resolution failed");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    server_addr.sin_port = htons(SMTP_PORT_587);

    // 连接到SMTP服务器
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
    {
        HWW_PRINT(HWW_DEBUG_LEVEL_ERR, "Connection failed");
        return -1;
    }
    
    // 读取服务器的欢迎消息
    send_smtp_command(sockfd, NULL, MAIL_SEND_CMD_ATTR_R);

    // 发送 EHLO 命令
    send_smtp_command(sockfd, MAIL_EHLO_CMD, MAIL_SEND_CMD_ATTR_WR);

    // 发送 STARTTLS 命令
    send_smtp_command(sockfd, MAIL_START_TTLS_CMD, MAIL_SEND_CMD_ATTR_WR);

    // 初始化 OpenSSL
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) 
    {
        HWW_PRINT(HWW_DEBUG_LEVEL_ERR, "SSL context creation failed");
        return -1;
    }

    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sockfd);

    // 升级到 SSL/TLS
    if (SSL_connect(ssl) <= 0) 
    {
        ERR_print_errors_fp(stderr);
        return -1;
    }

    // 继续发送 SMTP 命令
    send_smtp_command_ssl(ssl, MAIL_EHLO_CMD, MAIL_SEND_CMD_ATTR_WR);
    send_smtp_command_ssl(ssl, MAIL_AUTH_LOGIN_CMD, MAIL_SEND_CMD_ATTR_WR);

    // 发送Base64编码的用户名
    char usrname_encoded[64] = {0};
    hww_base64_encryption(USERNAME, strlen(USERNAME), usrname_encoded, sizeof(usrname_encoded));
    //printf("usrname: %s", usrname_encoded);
    send_smtp_command_ssl(ssl, usrname_encoded, MAIL_SEND_CMD_ATTR_W);
    send_smtp_command_ssl(ssl, MAIL_ENTER_CMD, MAIL_SEND_CMD_ATTR_WR);

    // 发送Base64编码的密码
    char pwd_encoded[64] = {0};
    hww_base64_encryption(PASSWORD, strlen(PASSWORD), pwd_encoded, sizeof(pwd_encoded));
    //printf("pwd: %s", pwd_encoded);
    send_smtp_command_ssl(ssl, pwd_encoded, MAIL_SEND_CMD_ATTR_W);
    send_smtp_command_ssl(ssl, MAIL_ENTER_CMD, MAIL_SEND_CMD_ATTR_WR);

    // 发送MAIL FROM命令
    char mail_from[256] = {0};
    snprintf(mail_from, sizeof(mail_from), MAIL_MAIL_FROM_CMD, USERNAME);
    send_smtp_command_ssl(ssl, mail_from, MAIL_SEND_CMD_ATTR_WR);

    // 发送RCPT TO命令
    char rcpt_to[256] = {0};
    snprintf(rcpt_to, sizeof(rcpt_to), MAIL_RCPT_TO_CMD, to_mail); // 替换为收件人邮箱
    send_smtp_command_ssl(ssl, rcpt_to, MAIL_SEND_CMD_ATTR_WR);

    // 发送DATA命令
    send_smtp_command_ssl(ssl, MAIL_DATA_CMD, MAIL_SEND_CMD_ATTR_WR);

    // 发送邮件内容
    char email_content[1024] = {0};
    snprintf(email_content, sizeof(email_content), MAIL_CONNECT_CMD, USERNAME, to_mail, title, mail_body); // 替换为收件人邮箱
    send_smtp_command_ssl(ssl, email_content, MAIL_SEND_CMD_ATTR_WR);

    // 发送QUIT命令
    send_smtp_command_ssl(ssl, MAIL_QUIT_CMD, MAIL_SEND_CMD_ATTR_WR);

    // 关闭SSL连接
    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);

    // 关闭套接字
    close(sockfd);

    return 0;
}