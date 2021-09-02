#ifndef SERVER_DATABASE_H
#define SERVER_DATABASE_H

void convert_num(int x);

int sign_up_01(char* buf,int confd);

int login_02(char *buf,int confd);

int log_off_03(char* buf,int confd);

int apply_04(char* buf,int confd);

int agree_05(char* buf,int confd);

int refuse_06(char* buf,int confd);

int click_chat_window_07(char *buf,int confd);

int click_refuse_08(char *buf,int confd);

int send_offline_09(char *buf,int confd);

int send_online_10(char *buf,int confd);

int set_remark_11(char *buf,int confd);

int modify_name_12(char *buf,int confd);

int modify_pword_13(char *buf,int confd);

int delete_friend_14(char *buf,int confd);

int click_delete_15(char *buf,int confd);

int history_16(char *buf,int confd);

int close_chat_window_17(char *buf,int confd);

#endif
