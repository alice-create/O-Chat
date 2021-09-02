--在虚拟机的mysql中运行本文件代码内容


-- 用户信息表
create table if not exisits UserTb(  
	ID int not null auto_increment,  -- 个人ID
	uName char(50) not null,  -- 昵称
	pWord char(50) not null,  -- 登录密码
	state int not null,  -- 在线状态
last_online DATETIME,  -- 最后在线时间（上次下线时间）
	primary key(ID)
);


-- 好友信息表
create table if not exisits FriendTb(  
	ID int not null,  -- 个人ID
	Friend_ID int not nul,  -- 好友ID
	remark char(50) not null,  -- 备注
isChating int not nul,  -- 是否正在聊天（用户是否打开了与朋友的聊天框）
	isRead int not null,  -- 未读消息数量
	primary key(ID, Friend_ID), 
foreign key(ID) references UserTb(ID)	
foreign key(Friend_ID) references UserTb(ID)
);


-- 聊天记录表
create table if not exisits RecordTb(  
	ID int not null,  -- 发送方ID
	Friend_ID int not nul,  -- 接收方ID
	content char(150) not null,  -- 聊天内容
	send_time DATETIME not null,  -- 发送时间
	primary key(ID, Friend_ID, send_time), 
foreign key(ID) references UserTb(ID)	
foreign key(Friend_ID) references UserTb(ID)
);


-- 申请添加/删除好友表
create table if not exisits AddTb(  
	ID int not null,  申请人ID
	Friend_ID int not nul,  -- 被申请ID
	isAdd int not null,  -- 被申请用户是否处理
	isRead int not null,  -- 申请人是否已读
	primary key(ID, Friend_ID, send_time), 
foreign key(ID) references UserTb(ID)	
foreign key(Friend_ID) references UserTb(ID)
);