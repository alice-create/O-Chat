# O-Chat

## 服务端代码作者：
    WormLK、Ling-LXY, Runzhi Ye
## 客户端代码作者：
    ArwenLeo、yyChen

## 运行平台： 
### 服务端：
    ubuntu 20.0.4 
    mysql 8.0.20
### 客户端：
    Qtcreator 4.2.1 或者更高版本

## 运行方式：
### 服务端：
    （1）在虚拟机的mysql下运行DB_set.sql文件中的代码，建立服务端所需数据库
    （2）查看虚拟机下IP地址，填写入main.c文件对应部分，此虚拟机将作为服务器
    （3）编译server_database.c和main.c文件，执行main.c文件产生的exe文件
### 客户端：
    （1）在运行聊天软件的电脑上复制整个ChatAppCom项目
    （2）将服务器IP和端口填写入login.cpp文件
    （3）在服务器已经开启并且客户端和服务器连接同一个局域网的条件下，点击运行即可进入聊天软件
