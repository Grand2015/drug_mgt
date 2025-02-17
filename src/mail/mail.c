#include "c_std_inc.h"
#include "mail.h"

#define SMTP_SERVER "smtp.qq.com"
#define SMTP_PORT 465
#define BUFFER_SIZE 1024

#define HOST_MAIL_ADDR  "997812940@qq.com"
#define HOST_MAIL_PWD   "kholuazkwjotbcai"


void send_smtp_command(SSL *ssl, const char *command, char *response) {
    printf("发送命令: %s", command);
    SSL_write(ssl, command, strlen(command));

    char buffer[BUFFER_SIZE];
    int n = SSL_read(ssl, buffer, BUFFER_SIZE - 1);
    if (n > 0) {
        buffer[n] = '\0';
        printf("服务器响应: %s", buffer);
        if (response) {
            strncpy(response, buffer, BUFFER_SIZE);
        }
    }
}

int mail_server_send_mail(char *to_mail, char *title, char *mail_body) 
{
    const char *username = "你的QQ邮箱@qq.com";
    const char *password = "你的授权码"; // 使用QQ邮箱的授权码
    const char *to = "收件人@example.com";
    const char *subject = "测试邮件";
    const char *body = "这是一封通过 C 程序发送的测试邮件。";

    int sockfd;
    struct sockaddr_in server_addr;
    SSL_CTX *ctx;
    SSL *ssl;
    char response[BUFFER_SIZE];

    // 初始化 OpenSSL
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ctx = SSL_CTX_new(TLS_client_method());

    // 创建套接字
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("套接字创建失败");
        exit(EXIT_FAILURE);
    }

    // 设置服务器地址
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SMTP_PORT);
    if (inet_pton(AF_INET, SMTP_SERVER, &server_addr.sin_addr) <= 0) {
        perror("地址转换失败");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // 连接到服务器
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("连接服务器失败");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // 创建 SSL 连接
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sockfd);
    if (SSL_connect(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // 读取欢迎信息
    send_smtp_command(ssl, "", response);

    // EHLO 命令
    send_smtp_command(ssl, "EHLO localhost\r\n", response);

    // 登录（AUTH LOGIN）
    send_smtp_command(ssl, "AUTH LOGIN\r\n", response);

    // 发送用户名（Base64 编码）
    char username_encoded[256];
    // 将 username 转换为 Base64 编码（需要实现或使用库）
    send_smtp_command(ssl, username_encoded, response);

    // 发送密码（Base64 编码）
    char password_encoded[256];
    // 将 password 转换为 Base64 编码（需要实现或使用库）
    send_smtp_command(ssl, password_encoded, response);

    // 发件人
    char mail_from[256];
    snprintf(mail_from, sizeof(mail_from), "MAIL FROM:<%s>\r\n", username);
    send_smtp_command(ssl, mail_from, response);

    // 收件人
    char rcpt_to[256];
    snprintf(rcpt_to, sizeof(rcpt_to), "RCPT TO:<%s>\r\n", to);
    send_smtp_command(ssl, rcpt_to, response);

    // 邮件内容
    send_smtp_command(ssl, "DATA\r\n", response);

    char email_data[1024];
    snprintf(email_data, sizeof(email_data), "Subject: %s\r\n\r\n%s\r\n.\r\n", subject, body);
    send_smtp_command(ssl, email_data, response);

    // 退出
    send_smtp_command(ssl, "QUIT\r\n", response);

    // 关闭 SSL 和套接字
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sockfd);
    SSL_CTX_free(ctx);

    printf("邮件发送完成\n");
    return 0;
}




static int mail_server_init()
{
    return 0;
}




void mail_server_deal()
{
    mail_server_init();
    
    return;
}