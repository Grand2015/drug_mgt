
#ifndef __MAIL_H__
#define __MAIL_H__


void mail_server_deal();
int mail_server_send_mail(char *to_mail, char *title, char *mail_body);
int mail_server_send_mail_v2(char *to_mail, char *title, char *mail_body);

#endif