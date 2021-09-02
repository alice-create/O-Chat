#include "server_database.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <mysql/mysql.h>
#include <netinet/in.h>
#include <time.h>

void convert_num(int x)
{
	extern char num_str[10];
	sprintf(num_str, "%4d", x);
	for(int k=0; k<4; k++)
	{
		if(num_str[k] == ' ')
			num_str[k] = '0';
	}
}

int sign_up_01(char *buf,int confd)
{
	// #01|昵称|密码
	extern char num_str[10];

	MYSQL mysql; 
	MYSQL_RES *result = NULL; 
	MYSQL_ROW row; 
	char uName[50]={0};
	char pWord[50]={0};
	sscanf(buf+4,"%[^|]|%[^|]",uName,pWord);
	
	printf("#01\n");
	printf("uName=%s\n",uName);
	printf("pWord=%s\n",pWord);
	
	// 用户名或者密码是否太长
	if(strlen(uName)>32||strlen(pWord)>32)
	{
		write(confd,"#01|1|1|dataType",16);
		printf("#01|1|1|dataType\n\n");
		mysql_free_result(result);
		mysql_close(&mysql);
		return -1;
	}
	else{
		char sqlStr[1024]={0}; //sqlStr存储MYSQL命令
		sprintf(sqlStr,"select * from UserTb where uName='%s'",uName);  //sprintf把一个字符串写入sqlStr
		mysql_init(&mysql); // &mysql是已有MYSQL结构的地址。这一行初始化MYSQL结构。
		//mysql_real_connect(&mysql,"localhost","debian-sys-maint","6BsPCPu0HmJBMOJ9","WORK_db",0,NULL,0);
		mysql_real_connect(&mysql,"localhost","root","root","WORK_db",0,NULL,0);  // 执行这一行之前，必须通过mysql_init()初始化MYSQL结构的地址
		mysql_query(&mysql,sqlStr); //执行mysql语句sqlStr

		result=mysql_store_result(&mysql);  //取出mysql语句放入result
		row=mysql_fetch_row(result);  //对result逐行读取（我觉得有点像cursor）
		// 用户是否存在
		if(row)
		{
			// 存在
			write(confd ,"#01|1|0|userExist",17);
			printf("#01|1|0|userExist\n\n");
			mysql_free_result(result);
			mysql_close(&mysql);
			return -1;
		}
		else{        
			// 不存在
			char cmd[70]={0}; //用于存储之后sprintf的中间的语句（为了sizeof）
			// 更新UserTb
			sprintf(sqlStr,"insert into UserTb values(0,'%s','%s',0,NOW())",uName,pWord);
			mysql_query(&mysql,sqlStr);
			
			mysql_free_result(result);
			sprintf(sqlStr,"select ID from UserTb where uName='%s'",uName);
			mysql_query(&mysql,sqlStr);
			result=mysql_store_result(&mysql);
			row=mysql_fetch_row(result);
			convert_num(atoi(row[0]));
			sprintf(cmd,"#01|0|0|%s|regSuccess",num_str);
			write( confd,cmd,sizeof(cmd));
			printf("%s\n", cmd);
		}
	}
	
	printf("\n");
	mysql_free_result(result); //释放result
	mysql_close(&mysql); //关闭MYSQL
	return 0;
}

int login_02(char *buf,int confd)
{
	// #02|ID|密码
	extern char num_str[10];
	extern int u_confd[999];
	int ID;
	char uName[32] = {0};
	char ID_str[32] = {0};
	char pWord[32] = {0};
	char output[1024] = {0};
	char all_output[1024] = {0};
	sscanf(buf+4, "%[^|]|%s", ID_str, pWord);
	ID = atoi(ID_str);

	printf("#02\n");
	printf("ID = %d\n", ID);
	printf("pWord = %s\n", pWord);

	MYSQL mysql; MYSQL_RES *result = NULL; MYSQL_ROW row; 
	MYSQL_RES *result_ = NULL; MYSQL_ROW row_;
	mysql_init(&mysql);
	//mysql_real_connect(&mysql,"localhost","debian-sys-maint","6BsPCPu0HmJBMOJ9","WORK_db",0,NULL,0);
	mysql_real_connect(&mysql,"localhost","root","root","WORK_db",0,NULL,0);
		
	// ID是否存在
	char sqlStr[1024] = {0}; 
	sprintf(sqlStr, "select uName from UserTb where ID=%d;", ID); 
	if(mysql_query(&mysql, sqlStr) != 0)
	{
		printf("query error: %d\n\n", mysql_query(&mysql, sqlStr)); 
		write(confd, "#02|1|dataType", 14);
		mysql_free_result(result);
		mysql_close(&mysql);
		return -1;
	}
	else
	{ 
		result = mysql_store_result(&mysql); 
		row = mysql_fetch_row(result);

		if(!row)
		{  
			// ID不存在
			write(confd, "#02|1|0|notExist", 16);
			printf("#02|1|0|notExist\n\n");
			mysql_free_result(result);
			mysql_close(&mysql);
			return -1;
		}
		else
			strcpy(uName, row[0]);

	}

	// 密码是否正确
	sprintf(sqlStr, "select * from UserTb where ID=%d and pWord='%s';", ID, pWord);
	if(mysql_query(&mysql, sqlStr) != 0)
	{
		printf("query error: %d\n\n", mysql_query(&mysql, sqlStr));
		write(confd, "#02|1|dataType", 14);
		mysql_free_result(result);
		mysql_close(&mysql);
		return -1;
	}
	result = mysql_store_result(&mysql);
	row = mysql_fetch_row(result);

	if(row)
	{  
		// 密码正确
		u_confd[ID] = confd;
		// 更改在线状态
		sprintf(sqlStr, "update UserTb set state=1 where ID=%d;", ID);
		if(mysql_query(&mysql, sqlStr)!=0)
		{
			printf("query error: %d\n\n", mysql_query(&mysql, sqlStr));
			write(confd, "#02|1|dataType", 14);
			return -1;
		}
		sprintf(sqlStr, "update UserTb set last_online=(select now()) where ID=%d;", ID);
		if(mysql_query(&mysql, sqlStr)!=0)
		{
			printf("query error: %d\n\n", mysql_query(&mysql, sqlStr));
			write(confd, "#02|1|dataType", 14);
			return -1;
		}
		// 对在线好友发上线消息
		sprintf(sqlStr, "select UserTb.ID from UserTb join FriendTb on UserTb.ID=FriendTb.Friend_ID where FriendTb.ID=%d and UserTb.state=1;", ID);
		if(mysql_query(&mysql, sqlStr)!=0)
                {
                        printf("query error: %d\n\n", mysql_query(&mysql, sqlStr));
                        write(confd, "#02|1|dataType", 14);
                        return -1;
                }
		result = mysql_store_result(&mysql);
		int online_friend[999] = {0};
		int online_friend_num = 0;
		int message_num = 0;
		while(row = mysql_fetch_row(result))
		{
			convert_num(atoi(row[0]));
			online_friend[online_friend_num++] = atoi(row[0]);
			sprintf(sqlStr, "select isRead from FriendTb where ID=%d and Friend_ID=%d;", atoi(row[0]), ID);
			if(mysql_query(&mysql, sqlStr)!=0)
                	{
                        	printf("query error: %d\n\n", mysql_query(&mysql, sqlStr));
                        	write(confd, "#02|1|dataType", 14);
                        	return -1;
                	}
			result_ = mysql_store_result(&mysql);
			row_ = mysql_fetch_row(result_);
			message_num = atoi(row_[0]);
			sprintf(output, "###02|%s|%s|%d", num_str, ID_str,message_num);
			write(u_confd[atoi(row[0])], output, strlen(output));
			printf("%s\n", output);
		}
		
		// 查找所有好友
                sprintf(sqlStr, "select UserTb.ID, UserTb.state, UserTb.uName, FriendTb.remark from UserTb join FriendTb on UserTb.ID=FriendTb.Friend_ID where FriendTb.ID=%d;", ID);
                if(mysql_query(&mysql, sqlStr)!=0)
                {
                        printf("query error: %d\n\n", mysql_query(&mysql, sqlStr));
                        write(confd, "#02|1|dataType", 14);
                        return -1;
                }
                result = mysql_store_result(&mysql);
                int friend[999] = {0};
                int friend_num = 0;
                while(row = mysql_fetch_row(result))
                {
                        friend_num++;
			convert_num(atoi(row[0]));
			if(row[3]==NULL)
                        	sprintf(output, "#02|0|%s|%s|%d", num_str, row[2], atoi(row[1]));
                        else
				sprintf(output, "#02|0|%s|%s|%d", num_str, row[3], atoi(row[1]));
			strcat(all_output, output);
                        printf("%s\n", output);
                }
		sprintf(output, "#02|0|%s|%d", uName, friend_num);
		printf("%s\n", output);
		strcat(output, all_output);
		write(confd, output, strlen(output));

		//输出未读消息
		sprintf(sqlStr, "select Friend_ID, isRead, isChating from FriendTb where ID=%d and isRead !=0;", ID);
		mysql_query(&mysql, sqlStr);
		result = mysql_store_result(&mysql);
		int message_list[999] = {0};  // message[F_ID]=num_of_message
		int unread_friend[999] = {0};  // unread_friend[...]=F_ID
		int unread_friend_state[999]= {0}; // unread_friend_state[F_ID] = state
		int unread_friend_num = 0;
		sleep(1);
		while(row = mysql_fetch_row(result))
		{
			message_list[atoi(row[0])] = atoi(row[1]);
			unread_friend_state[atoi(row[0])] = atoi(row[2]);
			unread_friend[unread_friend_num++] = atoi(row[0]);
		}
		for(int j=0; j<unread_friend_num; j++)
		{ 
			// 有未读消息
			memset(output, 0, sizeof(output));
			memset(all_output, 0, sizeof(all_output));
			int message_num = 0;
			convert_num(unread_friend[j]);
			sprintf(output, "###00|%s|%s|%d|%d", ID_str, num_str, unread_friend_state[unread_friend[j]], message_list[unread_friend[j]]);
			strcat(all_output, output);
			// write(confd, output, strlen(output));
			printf("%s\n", output);
			
			sprintf(sqlStr, "select send_time, content from RecordTb where ID=%d and Friend_ID=%d order by send_time desc limit %d;", unread_friend[j], ID, message_list[unread_friend[j]]);
			mysql_query(&mysql, sqlStr);
			result = mysql_store_result(&mysql);
			while(row = mysql_fetch_row(result))
			{
				sprintf(output, "###01|%s|%s|%d|%s|%s", ID_str, num_str, message_num++, row[0], row[1]);
				strcat(all_output, output);
				// write(confd, output, strlen(output));
				printf("%s\n", output);
			}

			write(confd, all_output, strlen(all_output));
		}

		// 输出申请消息
		sprintf(sqlStr, "select ID from AddTb where Friend_ID=%d and isAdd=0;", ID);
		mysql_query(&mysql, sqlStr);
		result = mysql_store_result(&mysql);
		sleep(1);
		while(row = mysql_fetch_row(result))
		{
			int Friend_ID = atoi(row[0]);
			convert_num(Friend_ID);
			sprintf(output, "###04|%s|%s", ID_str, num_str);
			write(confd, output, strlen(output));
			printf("%s\n", output);
		}

		// 输出拒绝消息
		sprintf(sqlStr, "select ID from AddTb where ID=%d and isRead=0;", ID);
		mysql_query(&mysql, sqlStr);
		result = mysql_store_result(&mysql);
		sleep(1);
		while(row = mysql_fetch_row(result))
		{
			int Friend_ID = atoi(row[0]);
			convert_num(Friend_ID);
			sprintf(output, "###05|%s|%s", ID_str, num_str);
			write(confd, output, strlen(output));
			printf("%s\n", output);
		}

		// 输出删除消息
		sprintf(sqlStr, "select ID from AddTb where Friend_ID=%d and isAdd=3;", ID);
		mysql_query(&mysql, sqlStr);
		result = mysql_store_result(&mysql);
		while(row = mysql_fetch_row(result))
		{
			int Friend_ID = atoi(row[0]);
			convert_num(Friend_ID);
			printf("ID: %s delete friend\n", num_str);
			sprintf(output, "###06|%s|%s", ID_str, num_str);
			write(confd, output, strlen(output));
			printf("%s\n", output);
		}

	}
	else{  
		// 密码错误
		write(confd, "#02|1|1|wrongPassword", 21);
		printf("#02|1|1|wrongPassword\n\n");
		mysql_free_result(result);
		mysql_close(&mysql);
		return -1;
	}

	printf("\n");
	mysql_free_result(result);
	mysql_close(&mysql);
	return 0;
}

int log_off_03(char *buf,int confd)
{
	// #03|ID
	extern int u_confd[999];

	MYSQL mysql;
	MYSQL_RES *result,*result_=NULL;
	MYSQL_ROW row,row_;
	char ID_S[50]={0};
	sscanf(buf+4,"%[^|]",ID_S);
	int ID=atoi(ID_S);
	char sqlStr[1024]={0};
	
	printf("#03\n");
	printf("ID = %d\n", ID);

	// 确定ID在库中且在线
	sprintf(sqlStr,"select * from UserTb where ID=%d and state=1",ID);
	mysql_init(&mysql);
	//mysql_real_connect(&mysql,"localhost","debian-sys-maint","6BsPCPu0HmJBMOJ9","WORK_db",0,NULL,0);
	mysql_real_connect(&mysql,"localhost","root","root","WORK_db",0,NULL,0);
	mysql_query(&mysql,sqlStr);
	result=mysql_store_result(&mysql);
	row=mysql_fetch_row(result);
        // 用户存在
	if(row){ 
                //更新UserTb
		sprintf(sqlStr,"update UserTb set state=0,last_online=(select now())  where ID=%d",ID);
		mysql_query(&mysql,sqlStr); 
		// 发送下线消息
		char cmd[1024]={0};
		sprintf(cmd,"#03|0");

		sprintf(sqlStr,"select UserTb.ID from UserTb join FriendTb on UserTb.ID=FriendTb.Friend_ID where FriendTb.ID=%d and state=1",ID);
		mysql_query(&mysql,sqlStr);
		result_=mysql_store_result(&mysql);
		while((row_=mysql_fetch_row(result_))!=NULL)
		{
			char f_ID[10],tmp[100]={0};
			int temp_num = atoi(row_[0]);
			int message_num = 0;
			sprintf(sqlStr, "select isRead from FriendTb where ID=%d and Friend_ID=%d;", atoi(row_[0]), ID);
                        mysql_query(&mysql, sqlStr);
			result = mysql_store_result(&mysql);
                        row = mysql_fetch_row(result);
                        message_num = atoi(row[0]);
			printf("online friend: %d\n", temp_num);
			sprintf(f_ID,"%04d",temp_num);
			sprintf(tmp,"###03|%s|%s|%d",f_ID,ID_S,message_num);
			write(u_confd[temp_num],tmp,sizeof(tmp));
			printf("%s\n", tmp);
		}

		// 更新FriendTb
		sprintf(sqlStr,"update FriendTb set isChating=0 where ID=%d",ID);
		mysql_query(&mysql,sqlStr);

	}
	else{
		//用户不存在或者是离线状态
		write(confd ,"#03|1",5);
		printf("#03|1\n\n");
		mysql_free_result(result);
		mysql_close(&mysql);
		return -1;
	}
	// write函数：ssize_t write (int fd, const void * buf, size_t count);
	// write()会把参数buf 所指的内存写入count 个字节到参数fd 所指的文件内. 当然, 文件读写位置也会随之移动.
	mysql_free_result(result); //释放result
	mysql_close(&mysql); //关闭MYSQL
	printf("\n");
	return 0;

}

int apply_04(char *buf,int confd)
{
        // 04|ID|F_ID
	extern int u_confd[999];
	
	MYSQL mysql;  // MYSQL结构的地址
	MYSQL_RES *result,*result_=NULL;
	MYSQL_ROW row,row_;
	char ID_S[10],FID_S[10]={0};  //ID_S和FID_S分别是自己和好友的id字符串
	sscanf(buf+4,"%[^|]|%[^|]",ID_S,FID_S);
	int ID=atoi(ID_S);
	int FID=atoi(FID_S);
	//由于只有登录的用户可以log off，所以不判断合法性
	char sqlStr[1024]={0};
	printf("#04\n");
	printf("ID = %d\n", ID);
	printf("Friend_ID = %d\n", FID);

        // 确定被申请的id在库中
	sprintf(sqlStr,"select * from UserTb where ID=%d",FID);
	mysql_init(&mysql);
	//mysql_real_connect(&mysql,"localhost","debian-sys-maint","6BsPCPu0HmJBMOJ9","WORK_db",0,NULL,0);
	mysql_real_connect(&mysql,"localhost","root","root","WORK_db",0,NULL,0);
	mysql_query(&mysql,sqlStr);
	result=mysql_store_result(&mysql);
	row=mysql_fetch_row(result);
	if(row)
	{
		//如果用户存在，接着判断是否已经是好友
                //检查是否已经是好友
		sprintf(sqlStr,"select * from FriendTb where ID=%d and Friend_ID=%d",ID,FID);
		mysql_query(&mysql,sqlStr);
		result_=mysql_store_result(&mysql);
		row_=mysql_fetch_row(result_);
		if(row_)
		{	//如果已经是好友
			write(confd, "#04|1|0|alreadyFriends",22);
			printf("#04|1|0|alreadyFriends\n\n");
			mysql_free_result(result_);
			return -1;
		}
		else
		{
			//正常发送申请
			char cmd[50]={0};
			write(confd, "#04|0|0|successfullySent", 24);
			printf("#04|0|0|successfullySent\n");
			// 判断是否在线
			sprintf(sqlStr,"select state from UserTb where ID=%d;", FID);
			mysql_query(&mysql,sqlStr);
			result_=mysql_store_result(&mysql);
			row_=mysql_fetch_row(result_);
			if(atoi(row_[0])==1)
			{
				// 在线则发送实时消息
				sprintf(cmd,"###04|%s|%s", FID_S, ID_S);
				write(u_confd[FID], cmd, sizeof(cmd));
				printf("%s\n", cmd);
			}
			// 更新AddTb
			sprintf(sqlStr,"insert into AddTb values(%d, %d, 0, 1)",ID,FID);
			mysql_query(&mysql,sqlStr);
		}

	}
	else
	{
		//想要申请的好友不存在
		write(confd ,"#04|1|1|notExist",16);
		printf("#04|1|1|notExist\n\n");
		mysql_free_result(result);
		mysql_close(&mysql);
		return -1;
	}
        
	printf("\n");
	mysql_free_result(result);
	mysql_close(&mysql);
	return 0;
}

int agree_05(char *buf,int confd)
{
	// #05|ID|FID
	extern int u_confd[999];
	
	MYSQL mysql;
	MYSQL_RES *result,*result_=NULL;
	MYSQL_ROW row,row_;
	char ID_S[10],FID_S[10]={0};
	sscanf(buf+4,"%[^|]|%[^|]",ID_S,FID_S);
	int ID=atoi(ID_S);
	int FID=atoi(FID_S);
	
	char sqlStr[1024]={0};
	printf("#05\n");
 	printf("ID = %d\n", ID);
	printf("Friend_ID = %d\n", FID);

	//确定被申请的id在库中
	sprintf(sqlStr,"select * from UserTb where ID=%d",FID);
	mysql_init(&mysql);
	mysql_real_connect(&mysql,"localhost","root","root","WORK_db",0,NULL,0);
	//mysql_real_connect(&mysql,"localhost","debian-sys-maint","6BsPCPu0HmJBMOJ9","WORK_db",0,NULL,0);
	mysql_query(&mysql,sqlStr);
	result=mysql_store_result(&mysql);
	row=mysql_fetch_row(result);
	if(row){
        	//用户存在，判断是否已经是好友
		sprintf(sqlStr,"select * from FriendTb where ID=%d and Friend_ID=%d",ID,FID);
		mysql_query(&mysql,sqlStr);
		result_=mysql_store_result(&mysql);
		row_=mysql_fetch_row(result_);
		if(row_){
			//已经是好友
			write(confd,"#05|1",5);
			printf("#05|1\n\n");
			mysql_free_result(result);
			mysql_close(&mysql);
			return -1;
		}
		else{
			//正常添加好友
			write(confd,"#05|0",5);
			printf("#05|0\n");
			// 更新FriendTb
			sprintf(sqlStr,"insert into FriendTb values(%d,%d,NULL,0,0),(%d,%d,NULL,0,1)",ID,FID,FID,ID);
			mysql_query(&mysql,sqlStr);
			// 更新AddTb
			sprintf(sqlStr,"update AddTb set isAdd=2 where ID=%d and Friend_ID=%d", FID, ID);
			mysql_query(&mysql,sqlStr);
			char cmd[1024] = {0};
			sprintf(cmd, "###07|%s|%s|%s|%d",ID_S,FID_S,row[1],atoi(row[3]));
			write(confd,cmd,strlen(cmd));
			printf("%s\n",cmd);
			// 如果在线发送实时消息
			if(atoi(row[3])==1)
			{
				sprintf(sqlStr, "select uName from UserTb where ID=%d;", ID);
				mysql_query(&mysql,sqlStr);
				result_=mysql_store_result(&mysql);
                		row_=mysql_fetch_row(result_);
				sprintf(cmd, "###07|%s|%s|%s|%d",FID_S,ID_S,row_[0],atoi(row[3]));
				write(u_confd[FID], cmd, strlen(cmd));
				printf("%s\n", cmd);
				// sprintf(cmd, "###00|%s|%s|1",FID_S,ID_S);
				// write(u_confd[FID], cmd, strlen(cmd));
				// printf("%s\n", cmd);
				// time_t t;
				// struct tm *lt;
				// time(&t);
				// lt = localtime(&t);
				// char timenow[32]= {0};
				// sprintf(timenow, "%d-%d-%d %d:%d:%d", lt->tm_year+1900, lt->tm_mon+1, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);
				// sprintf(cmd, "###01|%s|%s|0|%s|Let's chat!",FID_S, ID_S, timenow);
				// write(u_confd[FID], cmd, strlen(cmd));
				// printf("%s\n", cmd);

			}
			// 更新RecordTb
			// sprintf(sqlStr,"insert into RecordTb values(%d, %d, 'O!Chat', (select now()))", ID, FID);
			// mysql_query(&mysql,sqlStr);
		}
	}
	else
	{	//用户不存在报错
		write(confd ,"#05|1",5);
		printf("#05|1\n\n");
		mysql_free_result(result);
		mysql_close(&mysql);
		return -1;
	}
	
	printf("\n");
	mysql_free_result(result);
	mysql_close(&mysql);
	return 0;
}

int refuse_06(char *buf,int confd) 
{
	// #06|ID|Friend_ID
	extern int u_confd[999];

	MYSQL mysql;
	MYSQL_RES *result,*result_=NULL;
	MYSQL_ROW row,row_;
	char ID_S[10],FID_S[10]={0};
	sscanf(buf+4,"%[^|]|%[^|]",ID_S,FID_S);
	int ID=atoi(ID_S);
	int FID=atoi(FID_S);
	char sqlStr[1024]={0};
	
	printf("#06\n");
	printf("ID = %d\n", ID);
	printf("Friend_ID = %d\n", FID);

	sprintf(sqlStr,"select * from UserTb where ID=%d",FID);
	mysql_init(&mysql);
	mysql_real_connect(&mysql,"localhost","root","root","WORK_db",0,NULL,0);
	//mysql_real_connect(&mysql,"localhost","debian-sys-maint","6BsPCPu0HmJBMOJ9","WORK_db",0,NULL,0);
	mysql_query(&mysql,sqlStr);
	result=mysql_store_result(&mysql);
	row=mysql_fetch_row(result);
	if(row){ 
		//如果用户存在，判断是否已经是好友
		sprintf(sqlStr,"select * from FriendTb where ID=%d and Friend_ID=%d",ID,FID);
		mysql_query(&mysql,sqlStr);
		result_=mysql_store_result(&mysql);
		row_=mysql_fetch_row(result_);
		if(row_)
		{
			//如果已经是好友,报错
			write(confd,"#06|1",5);
			printf("#06|1\n\n");
			mysql_free_result(result);
			mysql_close(&mysql);
			return -1;
		}
		else
		{
			// 更新AddTb
			//mysql_free_result(result);
			sprintf(sqlStr,"update AddTb set isAdd=1, isRead=0 where ID=%d and Friend_ID=%d", FID, ID);
			mysql_query(&mysql,sqlStr);
			// 判断是否在线
			char cmd[1024] = {0};
			if(atoi(row[3]) == 1){
				sprintf(cmd, "###05|%s|%s", FID_S, ID_S);
				write(u_confd[FID], cmd, strlen(cmd));
				printf("%s\n", cmd);
			}
			write(confd,"#06|0",5);
			printf("#06|0\n");
		}
	}
	else
	{
		//如果bbb不存在，报错
		write(confd ,"#06|1",5);
		printf("#06|1\n\n");
		mysql_free_result(result);
		mysql_close(&mysql);
		return -1;
	}

	printf("\n");
	mysql_free_result(result);
	mysql_close(&mysql);
	return 0;
}

int click_chat_window_07(char *buf,int confd)
{
	extern char num_str[10];
	MYSQL mysql;
	MYSQL_RES *result = NULL;
	MYSQL_ROW row,row2;
	time_t t;
	struct tm *lt;
	extern int u_confd[999];
	char from[32] = {0};
	char to_who[32] = {0};
	sscanf(buf+4,"%[^|]|%[^|]",from,to_who);
	//sscanf(buf+4,"%[^|]|%s",from,to_who);
	int from1 = atoi(from); 
	int to1 = atoi(to_who);
	char sqlStr[1024] = {0};
	char output[1024] = {0};
	char all_output[1024] = {0};

	printf("#07\n");
	printf("ID = %s\n", from);
	printf("Friend_ID = %s\n", to_who);
        
	mysql_init(&mysql);
	mysql_real_connect(&mysql,"localhost","root","root","WORK_db",0,NULL,0);
	//mysql_real_connect(&mysql,"localhost","debian-sys-maint","6BsPCPu0HmJBMOJ9","WORK_db",0,NULL,0);
	
	// 返回成功信息
	sprintf(sqlStr,"select * from UserTb where ID=%d",to1);
	mysql_query(&mysql,sqlStr);
	result = mysql_store_result(&mysql);
	row = mysql_fetch_row(result);
	sprintf(output, "#07|0|%s|%s",to_who, row[1]);
	write(confd, output, strlen(output));
	printf("%s\n", output);

	// 提取未读消息
	sprintf(sqlStr,"select isRead from FriendTb where ID=%d and Friend_ID=%d",from1,to1);
	mysql_query(&mysql,sqlStr);
	result = mysql_store_result(&mysql);
	row = mysql_fetch_row(result);
	int message_num = atoi(row[0]);
	if(message_num != 0){
		// 有未读消息
		char SS[10]={0};
		convert_num(from1);
		strcpy(SS,num_str);
		sprintf(all_output, "###00|%s|%s|1|%d", SS, to_who, message_num);
		printf("%s\n",all_output);
		sprintf(sqlStr,"select * from RecordTb where Friend_ID=%d and ID=%d order by send_time desc limit %d", from1, to1, message_num);
		mysql_query(&mysql,sqlStr);
		result = mysql_store_result(&mysql);
		int xuhao = 0;
		while(row = mysql_fetch_row(result))
		{
			int ID=atoi(row[1]);
			int F_ID = atoi(row[0]);
			char ID_str[10]={0};
			char FID_str[10]={0};
			convert_num(ID);
			strcpy(ID_str,num_str);
			convert_num(F_ID);
			strcpy(FID_str,num_str);
			sprintf(output, "###01|%s|%s|%d|%s|%s", ID_str, FID_str, xuhao, row[3], row[2]);
			printf("%s\n", output);
			strcat(all_output, output);
			xuhao ++;
		}
		write(confd, all_output, strlen(all_output));
		
	}
	
	// 更新FriendTb
	sprintf(sqlStr,"update FriendTb set isChating=1, isRead=0 where ID=%d and Friend_ID=%d",from1,to1);
	int res = mysql_query(&mysql,sqlStr);
	if(res){
		write(confd,"#07|1",5);
		printf("#07|1\n\n");
		mysql_free_result(result);
		mysql_close(&mysql);
		return -1;
	}
	
	printf("\n");
	mysql_free_result(result);
	mysql_close(&mysql);
	return 0;
}

int click_refuse_08(char *buf,int confd)
{
	MYSQL mysql;
	MYSQL_RES *result = NULL;
	MYSQL_ROW row,row2;
	time_t t;
	struct tm *lt;

	extern int u_confd[999];
	char aaa[32] = {0};
	char bbb[32] = {0};
	sscanf(buf+4,"%[^|]|%s",aaa,bbb);
       
	int aa = atoi(aaa);
	int bb = atoi(bbb);
	printf("#08\n");
	printf("ID = %d\n", aa);
	printf("Friend_ID = %d\n", aa);
	        
	char sqlStr[1024]={0};

	sprintf(sqlStr,"update AddTb set isRead=1 where ID=%d and Friend_ID=%d",aa,bb);
       
	mysql_init(&mysql);
	mysql_real_connect(&mysql,"localhost","root","root","WORK_db",0,NULL,0);
	//mysql_real_connect(&mysql,"localhost","debian-sys-maint","6BsPCPu0HmJBMOJ9","WORK_db",0,NULL,0);
	int res =  mysql_query(&mysql,sqlStr);
    
	if(!res)
	{
		//success 
		write(confd,"#08|0",5);
		printf("#08|0\n");
	}
	else
	{
		write(confd,"#08|1",5);
		printf("#08|1\n\n");
		mysql_free_result(result);
		mysql_close(&mysql);
		return -1;
	}
	
	printf("\n");
	mysql_free_result(result);
	mysql_close(&mysql);
	return 0;
}

int send_offline_09(char *buf,int confd)
{
	MYSQL mysql;
        MYSQL_RES *result = NULL;
        MYSQL_ROW row,row2;
        time_t t;
        struct tm *lt;

	extern int u_confd[999];
	char from[32] = {0};
	char to_who[32] = {0};
	char contents[32] = {0}; 
	char tempp[32] = {0};
	time(&t); 
	lt = localtime(&t);
	char timenow[32]={0};
	sprintf(timenow,"%d-%d-%d %d:%d:%d",lt->tm_year+1900,lt->tm_mon+1,lt->tm_mday,lt->tm_hour,lt->tm_min,lt->tm_sec);
	sscanf(buf+4,"%[^|]|%s",from,to_who);

	int from1 = atoi(from);
	int to1 = atoi(to_who);
	printf("#09\n");
	printf("ID = %d\n", from1);
	printf("Friend_ID = %d\n", to1);

       // int lens = 6+strlen(from)+strlen(to_who);
	sscanf(buf+14,"%[^|]|%s",contents,tempp);
        
	char sqlStr[1024]={0};
        // 
	sprintf(sqlStr,"insert into RecordTb values(%d,%d,'%s','%s')",from1,to1,contents,timenow);

	mysql_init(&mysql);
	mysql_real_connect(&mysql,"localhost","root","root","WORK_db",0,NULL,0);
	//mysql_real_connect(&mysql,"localhost","debian-sys-maint","6BsPCPu0HmJBMOJ9","WORK_db",0,NULL,0);

	int res =  mysql_query(&mysql,sqlStr);
    
	if(res)
	{ 
        	write(confd,"#09|1",5);
        	printf("#09|1\n\n");
        	mysql_free_result(result);
        	mysql_close(&mysql);
		return -1;
	}
        
         
	sprintf(sqlStr,"update FriendTb set isRead=isRead+1 where ID=%d and Friend_ID=%d",to1,from1);
	res =  mysql_query(&mysql,sqlStr);
    
	if(!res) {
		write(confd,"#09|0",5);
		printf("#09|0\n");
	}
	else
	{
		write(confd,"#09|1",5);
		printf("#09|1\n\n");
		return -1;
	}
	
	printf("\n");
	mysql_free_result(result);
	mysql_close(&mysql);
	return 0;
}

int send_online_10(char *buf,int confd)
{
	MYSQL mysql;
        MYSQL_RES *result = NULL;
        MYSQL_ROW row,row2;
        time_t t;
        struct tm *lt;

	extern int u_confd[999];
	char from[32] = {0};
	char to_who[32] = {0};
	char contents[32] = {0};
	char tempp[32]={0};
	time(&t); 
	lt = localtime(&t);
	char timenow[32]={0};
	sprintf(timenow,"%d-%d-%d %d:%d:%d",lt->tm_year+1900,lt->tm_mon+1,lt->tm_mday,lt->tm_hour,lt->tm_min,lt->tm_sec);
	sscanf(buf+4,"%[^|]|%s",from,tempp);
	sscanf(buf+9,"%[^|]|%s",to_who,tempp);
        
	int from1 = atoi(from);
	int to1 = atoi(to_who);
	printf("#10\n");
	printf("ID = %d\n", from1);
	printf("Friend_ID = %d\n", to1);
	
	sscanf(buf+14,"%[^|]|%s",contents,tempp);
       
	char sqlStr[1024]={0};
	sprintf(sqlStr,"insert into RecordTb values(%d,%d,'%s','%s')",from1,to1,contents,timenow);
        
	mysql_init(&mysql);
        mysql_real_connect(&mysql,"localhost","root","root","WORK_db",0,NULL,0);
	//mysql_real_connect(&mysql,"localhost","debian-sys-maint","6BsPCPu0HmJBMOJ9","WORK_db",0,NULL,0);

	int res =  mysql_query(&mysql,sqlStr);
    
	if(!res)
	{
		sprintf(sqlStr,"select isChating from FriendTb where ID=%d and Friend_ID=%d",to1,from1);  
		if(mysql_query(&mysql,sqlStr)!=0)
		{
			write(confd,"#10|1",5); 
			printf("query error:%s\n\n",mysql_error(&mysql));
			mysql_free_result(result);
			mysql_close(&mysql);
			return -1;
		}
		
		result = mysql_store_result(&mysql);
		if(result ==NULL)
		{
			write(confd,"#10|1",5); 
			printf("result error:%s\n\n",mysql_error(&mysql));
			mysql_free_result(result);
			mysql_close(&mysql);
			return -1;
		}
		 
		row = mysql_fetch_row(result);
		
		sprintf(sqlStr,"select isRead from FriendTb where ID=%d and Friend_ID=%d",to1,from1);
		mysql_query(&mysql,sqlStr);
		result = mysql_store_result(&mysql);
		row2 = mysql_fetch_row(result);
            
		if(!atoi(row[0]))
		{
			char strr[1024]={0};
			sprintf(strr,"###00|%s|%s|0|%d",to_who,from,atoi(row2[0])+1);
 
			// write(u_confd[to1],strr,strlen(strr));
			printf("%s\n", strr);
			char ss[1024]={0};
			sprintf(ss,"###|01|%s|%s|%s|%s|%s",to_who,from,row2[0],timenow,contents);
			strcat(strr, ss);
			sprintf(sqlStr,"update FriendTb set isRead=isRead+1 where ID=%d and Friend_ID=%d",to1,from1);
			mysql_query(&mysql,sqlStr);
			write(u_confd[to1],strr,strlen(strr));
			printf("%s\n", ss);
			write(confd,"#10|0",5);
			printf("#10|0\n");
		}
		else
		{
			char strr[1024]={0};
			sprintf(strr,"###00|%s|%s|1|1",to_who,from);
			// write(u_confd[to1],strr,strlen(strr));
			printf("%s\n", strr);
			char ss[1024]={0};
			
			sprintf(ss,"###01|%s|%s|0|%s|%s",to_who,from,timenow,contents);
			strcat(strr, ss);
			write(u_confd[to1],strr,strlen(strr));
			printf("%s\n", ss);
			write(confd,"#10|0",5);
			printf("#10|0\n");
		}
	}
	else
	{
		printf("error:%s\n",mysql_error(&mysql));
		write(confd,"#10|1",5);
		printf("#10|1\n\n");
		mysql_free_result(result);
		mysql_close(&mysql);
		return -1;
	}
	
	printf("\n");
	mysql_free_result(result);
	mysql_close(&mysql);
	return 0;
}

int set_remark_11(char *buf,int confd)
{
	extern int u_confd[999];
	MYSQL mysql;
	MYSQL_RES *result = NULL;
	MYSQL_ROW row;
	mysql_init(&mysql);
	mysql_real_connect(&mysql,"localhost","root","root","WORK_db",0,NULL,0);
	//mysql_real_connect(&mysql,"localhost","debian-sys-maint","6BsPCPu0HmJBMOJ9","WORK_db",0,NULL,0);
		
	// 读取ID，好友ID和备注内容
	int ID;
	int Friend_ID;
	char ID_str[32] = {0};
	char Friend_ID_str[32] = {0};
	char remark[32] = {0};
	char output[1024] = {0};
	sscanf(buf+4, "%[^|]|%[^|]|%s", ID_str, Friend_ID_str, remark);
	ID = atoi(ID_str);
	Friend_ID = atoi(Friend_ID_str);

	printf("#11\n");
	printf("ID = %d\n", ID);
	printf("Friend_ID = %d\n", Friend_ID);
	printf("remark = %s\n", remark);

	// 更新FriendTb
	char sqlStr[1024] = {0};
	sprintf(sqlStr, "update FriendTb set remark='%s' where ID=%d and Friend_ID=%d", remark, ID, Friend_ID);

	// 返回结果
	if(mysql_query(&mysql, sqlStr) != 0)
	{
		printf("query error: %d\n", mysql_query(&mysql, sqlStr));
		write(confd, "#11|1", 5);
		printf("#11|1\n\n");
		mysql_free_result(result);
		mysql_close(&mysql);
		return -1;
	}
	else
	{
		// 提取未读消息数量
		sprintf(sqlStr, "select isRead from FriendTb where ID=%d and Friend_ID=%d",ID,Friend_ID);
		mysql_query(&mysql, sqlStr);
		result = mysql_store_result(&mysql);
	        row = mysql_fetch_row(result);

		
		sprintf(output,"#11|0|%s|%s|%d",Friend_ID_str, remark ,atoi(row[0]));
		write(confd, output, strlen(output));
		printf("#11|0\n");
	}
	printf("\n");
	mysql_free_result(result);
	mysql_close(&mysql);
	return 0;
}

int modify_name_12(char *buf,int confd)
{
	extern int u_confd[999];
	MYSQL mysql;
	MYSQL_RES *result = NULL;
	MYSQL_ROW row;

	mysql_init(&mysql);
	mysql_real_connect(&mysql,"localhost","root","root","WORK_db",0,NULL,0);
	//mysql_real_connect(&mysql,"localhost","debian-sys-maint","6BsPCPu0HmJBMOJ9","WORK_db",0,NULL,0);
		
	// 读取ID,密码和新昵称
	int ID;
	char ID_str[32] = {0};
	char pWord[32] = {0};
	char newName[32] = {0};
	sscanf(buf+4, "%[^|]|%[^|]|%s", ID_str, pWord, newName);
	ID = atoi(ID_str);

	printf("#12\n");
	printf("ID = %d\n", ID);
	printf("pWord = %s\n", pWord);
	printf("newName = %s\n", newName);

	// 昵称是否被占用
	char sqlStr[1024] = {0};
	sprintf(sqlStr, "select * from UserTb where uName = '%s'", newName);
	if(mysql_query(&mysql, sqlStr) != 0)
	{
		printf("query error: %d\n\n", mysql_query(&mysql, sqlStr));
		mysql_free_result(result);
		mysql_close(&mysql);
		return -1;
	}

	result = mysql_store_result(&mysql);
	row = mysql_fetch_row(result);

	if(row){
		write(confd, "#12|1|0|userExist", 17);
		printf("#12|1|0|userExist\n\n");
		mysql_free_result(result);
		mysql_close(&mysql);
		return -1;
	}
	else{
		// 密码是否正确
		sprintf(sqlStr, "select * from UserTb where ID=%d and pWord='%s'", ID, pWord);
		if(mysql_query(&mysql, sqlStr) != 0){
			printf("query error: %d\n\n", mysql_query(&mysql, sqlStr));
			mysql_free_result(result);
			mysql_close(&mysql);
			return -1;
		}
		MYSQL_RES *result = NULL;
		MYSQL_ROW row;
			
		result = mysql_store_result(&mysql);
		row = mysql_fetch_row(result);

		if(row){
			sprintf(sqlStr, "update UserTb set uName='%s' where ID=%d", newName, ID);
			if(mysql_query(&mysql, sqlStr) != 0)
			{
				printf("query error: %d\n\n", mysql_query(&mysql, sqlStr));
				mysql_free_result(result);
				mysql_close(&mysql);				
				return -1;
                        }
			else{
					char output[1024] = {0};
					sprintf(output,"#12|0|0|Sucess|%s",newName);
					write(confd, output, strlen(output));
					printf("%s\n",output);
			}
		}
		else
		{
				write(confd, "#12|1|1|wrongPassword", 21);
				printf("#12|1|1|wrongPassword\n\n");
				mysql_free_result(result);
				mysql_close(&mysql);
				return -1;
		}
	}

	mysql_free_result(result);
	mysql_close(&mysql);
	return 0;
}

int modify_pword_13(char *buf,int confd)
{
	extern int u_confd[999];
	MYSQL mysql;
	MYSQL_RES *result = NULL;
	MYSQL_ROW row;
	mysql_init(&mysql);
	mysql_real_connect(&mysql,"localhost","root","root","WORK_db",0,NULL,0);
	//mysql_real_connect(&mysql,"localhost","debian-sys-maint","6BsPCPu0HmJBMOJ9","WORK_db",0,NULL,0);
		
	// 读取ID,密码和新密码
	int ID;
	char ID_str[32] = {0};
	char pWord[32] = {0};
	char newPword[32] = {0};
	sscanf(buf+4, "%[^|]|%[^|]|%s", ID_str, pWord, newPword);
	ID = atoi(ID_str);

	printf("#13\n");
	printf("ID = %d\n", ID);
	printf("pWord = %s\n", pWord);
	printf("newPword = %s\n", newPword);

	// 密码是否正确
	char sqlStr[1024] = {0};
	sprintf(sqlStr, "select * from UserTb where ID=%d and pWord='%s'", ID, pWord);
	if(mysql_query(&mysql, sqlStr) != 0){
		printf("query error: %d\n\n", mysql_query(&mysql, sqlStr));
		mysql_free_result(result);
		mysql_close(&mysql);
		return -1;
	}

	result = mysql_store_result(&mysql);
	row = mysql_fetch_row(result);

	if(row){
		sprintf(sqlStr, "update UserTb set pWord='%s' where ID=%d", newPword, ID);
		if(mysql_query(&mysql, sqlStr) != 0){
			printf("query error: %d\n\n", mysql_query(&mysql, sqlStr));
			mysql_free_result(result);
			mysql_close(&mysql);
			return -1;
		}
		else{
			write(confd, "#13|0|0|Sucess", 14);
			printf("#13|0|0|Sucess\n\n");
			}
	}
	else{
		write(confd, "#13|1|0|wrongPassword", 17);
		printf("#13|1|0|wrongPassword\n\n");
		mysql_free_result(result);
		mysql_close(&mysql);
		return -1;
	}

	mysql_free_result(result);
	mysql_close(&mysql);
	return 0;
}

int delete_friend_14(char *buf,int confd)
{
	extern int u_confd[999];
	MYSQL mysql;
	MYSQL_RES *result = NULL;
	MYSQL_ROW row;
	mysql_init(&mysql);
	mysql_real_connect(&mysql,"localhost","root","root","WORK_db",0,NULL,0);
	//mysql_real_connect(&mysql,"localhost","debian-sys-maint","6BsPCPu0HmJBMOJ9","WORK_db",0,NULL,0);

	// 读取ID和好友ID
	int ID;
		int Friend_ID;
	char ID_str[32] = {0};
	char Friend_ID_str[32] = {0};
	sscanf(buf+4, "%[^|]|%s", ID_str, Friend_ID_str);
	ID = atoi(ID_str);
	Friend_ID = atoi(Friend_ID_str);

	printf("#14\n");
	printf("ID = %d\n", ID);
	printf("Friend_ID = %d\n", Friend_ID);
		

	// 更新FriendTb
	char sqlStr[1024] = {0};
	sprintf(sqlStr, "delete from FriendTb where ID=%d and Friend_ID=%d", ID, Friend_ID);
	if(mysql_query(&mysql, sqlStr) != 0)
	{
		printf("query error: %d\n\n", mysql_query(&mysql, sqlStr));
		write(confd, "#14|1", 5);
		mysql_free_result(result);
		mysql_close(&mysql);
		return -1;
	}
	sprintf(sqlStr, "delete from FriendTb where ID=%d and Friend_ID=%d", Friend_ID, ID);
	if(mysql_query(&mysql, sqlStr) != 0)
	{
		printf("query error: %d\n\n", mysql_query(&mysql, sqlStr));
		write(confd, "#14|1", 5);
		mysql_free_result(result);
		mysql_close(&mysql);
		return -1;
	}

	//更新Record
	sprintf(sqlStr, "delete from RecordTb where ID=%d and Friend_ID=%d", ID, Friend_ID);
	if(mysql_query(&mysql, sqlStr) != 0)
	{
		printf("query error: %d\n\n", mysql_query(&mysql, sqlStr));
		write(confd, "#14|1", 5);
		mysql_free_result(result);
		mysql_close(&mysql);
		return -1;
	}
	sprintf(sqlStr, "delete from RecordTb where ID=%d and Friend_ID=%d", Friend_ID, ID);
	if(mysql_query(&mysql, sqlStr) != 0)
	{
		printf("query error: %d\n\n", mysql_query(&mysql, sqlStr));
		write(confd, "#14|1", 5);
		mysql_free_result(result);
		mysql_close(&mysql);
		return -1;
	}

	//更新AddTb
	sprintf(sqlStr, "insert into AddTb values(%d, %d, 3, 2)", ID, Friend_ID);
	if(mysql_query(&mysql, sqlStr) != 0)
	{
		printf("query error: %d\n\n", mysql_query(&mysql, sqlStr));
		write(confd, "#14|1", 5);
		mysql_free_result(result);
		mysql_close(&mysql);
		return -1;
	}
	sprintf(sqlStr, "select state from UserTb where ID=%d",Friend_ID);
	if(mysql_query(&mysql, sqlStr) != 0)
	{
		printf("query error: %d\n\n", mysql_query(&mysql, sqlStr));
		write(confd, "#14|1", 5);
		mysql_free_result(result);
		mysql_close(&mysql);
		return -1;
	}
	result = mysql_store_result(&mysql);
	row = mysql_fetch_row(result);
	// 如果在线，发送拒绝消息
	if(atoi(row[0]))
	{
		char strr[1024]={0};
		sprintf(strr,"###06|%s|%s",Friend_ID_str,ID_str);
		write(u_confd[Friend_ID],strr,sizeof(strr));
		printf("%s\n", strr);
	}
	char ss[1024]={0};
	sprintf(ss,"#14|0|%s",Friend_ID_str);
	write(confd, ss, strlen(ss));
	printf("%s\n",ss);
	
	mysql_free_result(result);
	mysql_close(&mysql);
	return 0;
}

int click_delete_15(char *buf,int confd)
{
	extern int u_confd[999];
	MYSQL mysql;
	MYSQL_RES *result = NULL;
	MYSQL_ROW row;
	mysql_init(&mysql);
	mysql_real_connect(&mysql,"localhost","root","root","WORK_db",0,NULL,0);
	//mysql_real_connect(&mysql,"localhost","debian-sys-maint","6BsPCPu0HmJBMOJ9","WORK_db",0,NULL,0);

	// 读取被删除人ID和删除人ID
	int ID;
	int Friend_ID;
	char ID_str[32] = {0};
	char Friend_ID_str[32] = {0};
	sscanf(buf+4, "%[^|]|%s", ID_str, Friend_ID_str);
	ID = atoi(ID_str);
	Friend_ID = atoi(Friend_ID_str);

	printf("#15\n");
	printf("ID = %d\n", ID);
	printf("Friend_ID = %d\n", Friend_ID);

	//更新AddTb
	char sqlStr[1024] = {0};
	sprintf(sqlStr, "update AddTb set isAdd=4 where ID=%d and Friend_ID=%d", Friend_ID, ID);
	if(mysql_query(&mysql, sqlStr) != 0)
	{
		printf("query error: %d\n\n", mysql_query(&mysql, sqlStr));
		write(confd, "#15|1", 5);
		mysql_free_result(result);
		mysql_close(&mysql);
		return -1;
	}

	write(confd, "#15|0", 5);
	printf("#15|0\n\n");

	mysql_free_result(result);
	mysql_close(&mysql);
	return 0;
}

int history_16(char *buf,int confd)
{
	MYSQL mysql;
	MYSQL_RES *result = NULL;
	MYSQL_ROW row,row2;
	time_t t;
	struct tm *lt;

	extern int u_confd[999];
	extern char num_str[10];
	char aaa[32]={0};
	char bbb[32]={0};
	sscanf(buf+4,"%[^|]|%s",aaa,bbb);
	int aa = atoi(aaa);
	int bb = atoi(bbb);
	char sqlStr[1024]={0};
	printf("#16\n");
	printf("ID = %d\n", aa);
	printf("Friend_ID = %d\n", bb);
			 
	sprintf(sqlStr,"select ID,Friend_ID,content, send_time from RecordTb where (ID=%d and Friend_ID=%d) or (ID=%d and Friend_ID=%d) order by send_time desc",aa,bb,bb,aa);
	mysql_init(&mysql);
	mysql_real_connect(&mysql,"localhost","root","root","WORK_db",0,NULL,0);
	//mysql_real_connect(&mysql,"localhost","debian-sys-maint","6BsPCPu0HmJBMOJ9","WORK_db",0,NULL,0);
	if(mysql_query(&mysql,sqlStr)!=0)
	{
		printf("query error:%s\n\n",mysql_error(&mysql));
		write(confd,"#16|1",5);
		mysql_free_result(result);
		mysql_close(&mysql);
		return -1;
	}
	result = mysql_store_result(&mysql);
	if(result ==NULL)
	{
		printf("result error:%s\n\n",mysql_error(&mysql));
		write(confd,"#16|1",5);
		mysql_free_result(result);
		mysql_close(&mysql);
		return -1;
	}
	int lens = mysql_num_rows(result);
	char out[1024]={0};
	char outt[1024]={0};
	sprintf(outt,"#16|0|%d",lens);
	for(int j=0;j<lens;j++)
	{
		char row_0[10] = {0};
		char row_1[10] = {0};
		row = mysql_fetch_row(result);
		convert_num(atoi(row[0]));
		strcpy(row_0, num_str);
		convert_num(atoi(row[1]));
		strcpy(row_1, num_str);
		sprintf(out,"#16|0|%d|%s|%s|%s|%s",j,row_0,row_1,row[3], row[2]);
		strcat(outt,out);
	}
	write(confd,outt,sizeof(outt));
	printf("%s\n", outt);
	
	printf("\n");
	mysql_free_result(result);
	mysql_close(&mysql);
	return 0;
}

int close_chat_window_17(char *buf,int confd)
{
        MYSQL mysql;
        MYSQL_RES *result = NULL;
        MYSQL_ROW row,row2;
        time_t t;
        struct tm *lt;
        extern int u_confd[999];
        char from[32] = {0};
        char to_who[32] = {0};
        sscanf(buf+4,"%[^|]|%s",from,to_who);
        int from1 = atoi(from);
        int to1 = atoi(to_who);
        printf("#17\n");
        printf("ID = %d\n", from1);
        printf("Friend_ID = %d\n", to1);

        char sqlStr[1024]={0};
        sprintf(sqlStr,"update FriendTb set isChating=0 where ID=%d and Friend_ID=%d",from1,to1);
        mysql_init(&mysql);
	mysql_real_connect(&mysql,"localhost","root","root","WORK_db",0,NULL,0);
        //mysql_real_connect(&mysql,"localhost","debian-sys-maint","6BsPCPu0HmJBMOJ9","WORK_db",0,NULL,0);
        int res = mysql_query(&mysql,sqlStr);
        if(res){
                write(confd,"#17|1",5);
                printf("#17|1\n\n");
                mysql_free_result(result);
                mysql_close(&mysql);
                return -1;
        }
        else
                write(confd,"#17|0",5);
                printf("#17|0\n\n");
        
	mysql_free_result(result);
	mysql_close(&mysql);
	return 0;
}
