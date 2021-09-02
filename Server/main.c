# include<stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <mysql/mysql.h>
# include "server_database.h"
# include <time.h>

//gcc main.c -o main -include server_database.h server_database.c  -lmysqlclient -L/usr/lib/mysql

int u_confd[999];  // u_confd[ID] = confd 
char num_str[10];  // convert int to str

void select_func(char *buf, int confd){
	char types[3] = {0};
	strncpy(types, buf+1, 2);
	
	switch(atoi(types)){
		case 1: sign_up_01(buf, confd);break;
		case 2: login_02(buf, confd); break;  // 登录 
		case 3: log_off_03(buf, confd);break;
		case 4: apply_04(buf, confd);break;
		case 5: agree_05(buf, confd);break;
		case 6:refuse_06(buf, confd);break;
		case 7: click_chat_window_07(buf, confd); break;  // 点击对话框 
		case 8: click_refuse_08(buf, confd); break;  // 拒绝好友申请 
		case 9: send_offline_09(buf, confd); break;  // 发送离线消息 
		case 10: send_online_10(buf, confd); break;  // 发送在线消息 
		case 11: set_remark_11(buf, confd); break;  // 设置备注 
		case 12: modify_name_12(buf, confd); break;  // 更改昵称 
		case 13: modify_pword_13(buf, confd); break;  // 更改密码 
		case 14: delete_friend_14(buf, confd); break;  // 删除好友 
		case 15: click_delete_15(buf, confd); break;  // 点击被拒绝消息 
		case 16: history_16(buf, confd); break;  // 查看聊天记录 
		case 17: close_chat_window_17(buf, confd);break;
	}
}

int main(){
	struct sockaddr_in sevaddr;
	memset(&sevaddr, 0, sizeof(sevaddr));
	sevaddr.sin_family = AF_INET;
	sevaddr.sin_port = htons(8899);
	sevaddr.sin_addr.s_addr = inet_addr("192.168.43.64");
	
	int clifd[1024] = {0};
	memset(clifd, -1, sizeof(clifd));
	int confd = 0;
	int curi = 0;
	int i = 0;
	
	int maxfd;
	fd_set allset;
	fd_set rset;
	FD_ZERO(&allset);
	FD_ZERO(&rset);

	int lisfd = socket(AF_INET, SOCK_STREAM, 0);
	if(bind(lisfd, (struct sockaddr *)&sevaddr, sizeof(sevaddr)) != 0){
		printf("bind error\n");
		return -1;
	}

	listen(lisfd, 10);
	FD_SET(lisfd, &allset);
	
	if(lisfd > maxfd)
		maxfd = lisfd;

	while(1){
		rset = allset;
		int nready = select(maxfd+1, &rset, NULL, NULL, NULL);
		if(nready <= 0){
			printf("select error\n");
			return -1;
		}

		if(FD_ISSET(lisfd, &rset)){
			confd = accept(lisfd, NULL, NULL);
			if(confd==-1){
				printf("accept error\n");
				return -1;
			}
			FD_SET(confd, &allset);
			if(confd > maxfd)
				maxfd = confd;
			
			for(i=0; i<curi; i++){
				if(clifd[i]==-1){
					clifd[i] = confd;
					break;
				}
			}
			if(i==curi){
				if(curi==1024)
					printf("max\n");
				else
					clifd[curi++] = confd;
			}
		}
		
		for(i=0; i<curi; i++){
			confd = clifd[i];

			if(FD_ISSET(confd, &rset)){
				char buf[1024] = {0};
				
				if(read(confd, buf, sizeof(buf))==0){
					FD_CLR(confd, &allset);
					clifd[i] = -1;
					continue;
				}
				select_func(buf, confd);
			}
		}
	}

	return 0;

}
