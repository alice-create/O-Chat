#include "mainchatwin.h"
#include "ui_mainchatwin.h"
#include "qnchatmessage.h"

//这里是widget
MainChatWin::MainChatWin(QWidget *parent):
    QWidget (parent),
    ui(new Ui::MainChatWin)
//连接并美化窗口//此段已注释掉了
{
    ui->setupUi(this);//由.ui文件生成的类的构造函数，对界面进行初始化
//    //它按照我们在Qt设计器里设计的样子把窗体画出来，把我们在Qt设计器里面定义的信号和槽建立起来

//    /***   TCP连接部分   ***/
//    this->isconnection = false;
//    this->tcpSocket = new QTcpSocket();  //初始化tcpSocket

//    tcpSocket->abort();  //终止之前的连接，重置套接字
//    quint16 port = 8899;   // 获取服务器IP和端口，建立连接
//    QString ip = "192.168.43.68";//套接字socket = IP + 端口号，得在一个局域网下
//    tcpSocket->connectToHost(QHostAddress(ip), port);

//    connect(tcpSocket,SIGNAL(connected()),this,SLOT(slot_connected()));
//    connect(ui->sendBtn,SIGNAL(clicked()),this,SLOT(slot_sendmessage()));
//    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(slot_recvmessage()));
//    connect(tcpSocket,SIGNAL(disconnected()),this,SLOT(slot_disconnect()));

//    /***    窗体美化部分   ***/
//    resize(800,600);  //重绘窗体大小
//    setWindowFlags(Qt::FramelessWindowHint);//窗体无边框
//    setAttribute(Qt::WA_TranslucentBackground, true);//设置窗口半透明显示

//    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();//加阴影
//    shadow->setBlurRadius(20); //模糊半径
//    shadow->setOffset(0, 0);//偏移量，00向四周发散
//    shadow->setColor(QColor(192, 192, 192));//阴影颜色
//    ui->frame->setGraphicsEffect(shadow);

//    mMoveing = false;//这里不让移动放到下面定义移动
}

MainChatWin::MainChatWin(QTcpSocket *sock, /*QString messageStr, */QString thisID, QString thisName, QString thatID, QString thatName, QString state, QWidget *parent):
    QWidget (parent),
    ui(new Ui::MainChatWin)
//连接并美化窗口
{
    ui->setupUi(this);//由.ui文件生成的类的构造函数，对界面进行初始化，
    //它按照我们在Qt设计器里设计的样子把窗体画出来，把我们在Qt设计器里面定义的信号和槽建立起来

//    unprocessed_Data = messageStr;
    s_thisID = thisID;
    s_thisName = thisName;
    s_thatID = thatID;
    s_thatName = thatName;
    s_state = state;

    qDebug() << s_thatName; //理论上这里应该传进来备注

    QString setuname = "<html><head/><body><p><span style=' font-weight:600;'>" + s_thatName + "  " + "</span><span style=' color:#5b5b5b; vertical-align:sub;'>(" + s_thatID + ")" + "</span></p></body></html>";
    QString setmname = "<html><head/><body><p><span style=' font-weight:600;'>" + s_thisName + "  " + "</span><span style=' color:#5b5b5b; vertical-align:sub;'>(" + s_thisID + ")" + "</span></p></body></html>";
    QString setuquote = "Hello " + s_thisName + " !";
    QString setmquote = "Hello " + s_thatName + " !";
    ui->uName->setText(setuname);
    ui->mName->setText(setmname);
    ui->uQuote->setText(setuquote);
    ui->mQuote->setText(setmquote);
    ui->uName->repaint();
    ui->mName->repaint();
    ui->uQuote->repaint();
    ui->mQuote->repaint();

//    showSheMessage();

    /***   TCP连接部分   ***/
    tcpSocket = sock;
    this->isconnection = true;
//    this->tcpSocket = new QTcpSocket();  //初始化tcpSocket

//    tcpSocket->abort();  //终止之前的连接，重置套接字
//    quint16 port = 8899;   // 获取服务器IP和端口，建立连接
//    QString ip = "192.168.43.68";//套接字socket = IP + 端口号，得在一个局域网下
//    tcpSocket->connectToHost(QHostAddress(ip), port);

//    connect(tcpSocket,SIGNAL(connected()),this,SLOT(slot_connected()));
//    connect(ui->sendBtn,SIGNAL(clicked()),this,SLOT(slot_sendmessage()));

//    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(slot_recvmessage()));
    connect(tcpSocket,SIGNAL(disconnected()),this,SLOT(slot_disconnect()));

    /***    窗体美化部分   ***/
    resize(800,600);  //重绘窗体大小
    setWindowFlags(Qt::FramelessWindowHint);//窗体无边框
    setAttribute(Qt::WA_TranslucentBackground, true);//设置窗口半透明显示

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();//加阴影
    shadow->setBlurRadius(20); //模糊半径
    shadow->setOffset(0, 0);//偏移量，00向四周发散
    shadow->setColor(QColor(192, 192, 192));//阴影颜色
    ui->frame->setGraphicsEffect(shadow);

    mMoveing = false;//这里不让移动放到下面定义移动
}

MainChatWin::~MainChatWin()
{
    delete ui;
}

//重写鼠标按下事件
void MainChatWin::mousePressEvent(QMouseEvent *event)
{
    mMoveing = true;
    //记录下鼠标相对于窗口的位置：
    //event->globalPos()鼠标按下时，鼠标相对于整个屏幕位置
    //pos() this->pos()鼠标按下时，窗口相对于整个屏幕位置
    mMovePosition = event->globalPos() - pos();
    QWidget::mousePressEvent(event);
}

//重写鼠标移动事件
void MainChatWin::mouseMoveEvent(QMouseEvent *event)
{
    //(event->buttons() && Qt::LeftButton)按下是左键
    //实现目标：用鼠标移动窗口
    //event->globalPos()知道鼠标坐标，用它减去鼠标相对于窗口位置，就是窗口在整个屏幕的坐标

    if(mMoveing & (event->buttons() & Qt::LeftButton) //这里原来用的是&&
            &(event->globalPos() - mMovePosition).manhattanLength()
            > QApplication::startDragDistance()) {
        move(event->globalPos() - mMovePosition);
        mMovePosition = event->globalPos() - pos();
    }
    QWidget::mouseMoveEvent(event);
}

//重写鼠标松开事件
void MainChatWin::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    mMoveing = false;
}

void MainChatWin::dealMessage(QNChatMessage *messageW, QListWidgetItem *item, QString text, QString time,  QNChatMessage::User_Type type)
//把信息传到这里，以气泡显示
{
    messageW->setFixedWidth(this->width());
    QSize size = messageW->fontRect(text);
    item->setSizeHint(size);
    messageW->setText(text, time, size, type);  //调用这个的时候开始loading动画
    ui->chatMsgList->setItemWidget(item, messageW);//void setItemWidget(QListWidgetItem *item, QWidget *widget);
}

void MainChatWin::dealMessageTime(QString curMsgTime)
//把时间传到这里并显示
{
    qDebug()<<"传入dealMessageTime的时间是"<<curMsgTime;
    if(isShowTime) {  //显示时间的具体操作
        QNChatMessage* messageTime = new QNChatMessage(ui->chatMsgList->parentWidget());
        QListWidgetItem* itemTime = new QListWidgetItem(ui->chatMsgList);

        QSize size = QSize(this->width(), 30);  //QSize(int width, int height)
        messageTime->resize(size); //messageTime是从QListWidget(QNChatMessage)来的
        itemTime->setSizeHint(size);   //itemTime是从QListWidgeItem来的
        messageTime->setText(curMsgTime, curMsgTime, size, QNChatMessage::User_Time);
        ui->chatMsgList->setItemWidget(itemTime, messageTime);
        //setItemWidget( QListWidgetItem *item, QWidget *widget)函数接口
        //通常用来设置显示我们自定义的QWidget，然后将item与我们自定义的QWidget相关联
        //这里的逻辑总是写不清楚，itemTime是一个ListWidgetItem
    }
}

void MainChatWin::resizeEvent(QResizeEvent *event)
//重新设置大小
{
    Q_UNUSED(event);

    ui->textArea->resize(this->width() - 20, this->height() - 20);
    ui->textArea->move(10, 10);

//    ui->sendBtn->move(ui->textArea->width()+ui->textArea->x() - ui->sendBtn->width() - 10,
//                      ui->textArea->height()+ui->textArea->y() - ui->sendBtn->height() - 10);


    for(int i = 0; i < ui->chatMsgList->count(); i++) {  //获取列表项的总数目
        QNChatMessage* messageW = new QNChatMessage(ui->chatMsgList->itemWidget(ui->chatMsgList->item(i)));
        QListWidgetItem* item = ui->chatMsgList->item(i);

        dealMessage(messageW, item, messageW->text(), messageW->time(), messageW->userType());
    }
}

void MainChatWin::on_sendBtn_clicked()
//按了发送按钮之后这里要完成两件事情
//一个是用户自己的界面上要显示出消息
{  
    QString msg = ui->textArea->toPlainText();    //textEdit 用toPlainText()方法获取编辑框文本
//    QString type02 = "#02";

    QString type09 = "#09";
    QString type10 = "#10";

//    if(msg.left(5) == "login"){  //以login开头
//        slot_sendmessage(type02, msg);
//    }

    if(!msg.isEmpty()){  //既没有以login开头也不为空
        if(s_state == "0"){   //调用slot_sendmessage来发送消息
            slot_sendmessage(type09,msg);
        }
        else if(s_state == "1"){
            slot_sendmessage(type10,msg);
        }
    }
    else{  //为空
        QMessageBox::warning(this,"提示","消息不能为空！",QMessageBox::Ok);//19
    }

    ui->textArea->setText("");

    QDateTime curDateTime=QDateTime::currentDateTime();
    QString timestr = curDateTime.toString("yyyy-MM-dd hh:mm:ss");
    timenew = QString::number(curDateTime.toTime_t()); //时间戳，秒数
    qint32 timedis = timenew.toInt() - timeold.toInt();
    timeold = timenew;

    qint32 msgcount = ui->chatMsgList->count();
    qDebug()<<"addMessage" << msg << timestr << timestr << msgcount;  //输出文本内容及时间及消息数目

//    bool isSending = true; // 正在发送

    if(!msg.isEmpty()){
        isShowTime = false;
        if(ui->chatMsgList->count() > 0) { //如果聊天框里已经有消息了
//            QListWidgetItem* lastItem = ui->chatMsgList->item(ui->chatMsgList->count() - 1);
//            QNChatMessage* messageW = new QNChatMessage(ui->chatMsgList->itemWidget(lastItem));
//            int lastTime = messageW->time().toInt();  //上一条消息是啥时候发的
//            int curTime = timenew.toInt();  //这一条消息是啥时候
            qDebug() << "curTime lastTime distance:" << timedis;
            if(timedis > 60){   // 两个消息相差一分钟就显示时间
                isShowTime = true;
            }
        } else{  //如果没有消息，那么在一开始就显示时间
            isShowTime = true;
        }

        dealMessageTime(timestr);

        QNChatMessage* messageW = new QNChatMessage(ui->chatMsgList->parentWidget());
        QListWidgetItem* item = new QListWidgetItem(ui->chatMsgList);
        dealMessage(messageW, item, msg, timestr, QNChatMessage::User_Me);

        messageW->setTextSuccess();
    }

    ui->chatMsgList->setCurrentRow(ui->chatMsgList->count()-1);
}

void MainChatWin::slot_connected()//连接到服务器
{
    this->isconnection = true;
}

void MainChatWin::slot_disconnect()
{
//    QMessageBox::warning(this,"警告","与服务器的连接中断",QMessageBox::Ok);

    tcpSocket->close();      //关闭并随后删除socket
//    tcpSocket->deleteLater();
}

void MainChatWin::slot_sendmessage(QString type, QString message)
//一个是send的用户把消息发给服务器(调用slot_sendmessage)
{
    QString send_message = message;
    QString send_type = type;
    //QT_A -> Server：#10|0003|0004|hhhh

/*正则表达式提取（）中间的字符*/
//    QStringList list;
//    QString mySide = ui->uName->text();
//    QString pattern("\\(\\w*\\)");//正则表达式提取“(XXX)”,XXX为任意字符串
//    QRegExp rx(pattern);
//    int pos = 0;
//    while ((pos = rx.indexIn(mySide, pos)) != -1)
//    {
//           list.append(rx.cap(0)); //把提取到的字符串保存下来
//           pos += rx.matchedLength();
//    }
//    QString sheID = list[0].mid(1,4);//取出（xxxx）中间的四位数字

    QString sheID = s_thatID;
    QString myID = s_thisID;

    QString pattern_msg = "";
/*    if(send_type == "#02"){
        pattern_msg = send_type + "|"  + send_message.mid(6,4) + "|" + send_message.right(4);
    }
    else */

    if(send_type == "#10"){//在线消息
        pattern_msg = send_type + "|" + myID + "|" +sheID + "|" + send_message;
    }
    else if(send_type == "#09"){ //离线消息
        pattern_msg = send_type + "|" + myID + "|" +sheID + "|" + send_message;
    }
    else if(send_type == "#16"){  //向服务器请求历史消息
        pattern_msg = send_type + "|" + myID + "|" + sheID;
    }
    else if(send_type == "#17"){   //告诉服务器我把和对方的聊天框关了
        pattern_msg = send_type + "|" + myID + "|" + sheID;
    }

    qDebug()<<"传给服务器的串"<<pattern_msg;
    if(this->isconnection)//传过去
    {
//        QTextCodec *tc = QTextCodec::codecForName("GBK");
//        tcpSocket->write(tc->fromUnicode(pattern_msg));
        tcpSocket->write(pattern_msg.toUtf8());//给服务器发消息//这样中文是乱码
    }
//    else
//        QMessageBox::warning(this,"错误","未连接到服务器!",QMessageBox::Ok);
}

void MainChatWin::send_tochat(QString messageStr)
//就是把message转给我
{
    unprocessed_Data = messageStr;
    showSheMessage();
}

void MainChatWin::showSheMessage(){
    qDebug() << "从Userlist传进来的参数收到的：" << unprocessed_Data;
    if(unprocessed_Data.left(5) == "###00"){
        QStringList dataList = unprocessed_Data.mid(3).split("###");
        qDebug()<<dataList;
        int msgnum = dataList[0].mid(15).toInt();
        qDebug()<<msgnum;
        for(int i = msgnum; i >= 1; i--){
            QStringList msgList = dataList[i].split("|");
            QString sendtime = msgList[4];
            QString sendmsg = msgList[5];

            dealMessageTime(sendtime);
            QNChatMessage* messageW = new QNChatMessage(ui->chatMsgList->parentWidget());
            QListWidgetItem* item = new QListWidgetItem(ui->chatMsgList);
            dealMessage(messageW, item, sendmsg, sendtime, QNChatMessage::User_She);
        }
    }

    else if(unprocessed_Data.left(3) == "#16"){ //要显示历史消息
        while (ui->chatMsgList->count() > 0)
        {
            QListWidgetItem *item = ui->chatMsgList->takeItem(0);
            delete item;
        }
        QStringList dataList = unprocessed_Data.mid(1).split("#");
        int msgnum = dataList[0].mid(5).toInt();    //#16|0|num
        for(int i = msgnum; i >= 1; i--){
            QStringList msgList = dataList[i].split("|");
            QString sendID = msgList[3];
            QString recvID = msgList[4];
            QString sendtime = msgList[5];
            QString sendmsg = msgList[6];
            if(sendID == s_thisID){
                dealMessageTime(sendtime);
                QNChatMessage* messageW = new QNChatMessage(ui->chatMsgList->parentWidget());
                QListWidgetItem* item = new QListWidgetItem(ui->chatMsgList);
                dealMessage(messageW, item, sendmsg, sendtime, QNChatMessage::User_Me);
            }
            else if(sendID == s_thatID){
                dealMessageTime(sendtime);
                QNChatMessage* messageW = new QNChatMessage(ui->chatMsgList->parentWidget());
                QListWidgetItem* item = new QListWidgetItem(ui->chatMsgList);
                dealMessage(messageW, item, sendmsg, sendtime, QNChatMessage::User_She);
            }
        }
    }
}


//{
//    //Server -> QT_B：###00|0001|0002|1|3
//    //Server -> QT_B：###01|0001|0002|0|time|hhhh
//    //Server -> QT_B：###01|0001|0002|1|time|hhhh
//    //Server -> QT_B：###01|0001|0002|2|time|hhhh
//    QByteArray recvArray = this->tcpSocket->readAll();
//    QTextCodec *tc = QTextCodec::codecForName("UTF-8");
//    QString recvStr = tc->toUnicode(recvArray);
////    QString recvStr(recvArray);

//    QChar h0 = 0x00;
//    if(recvStr.contains(h0)){
//        recvStr.replace(h0,"");
//    }

//    qDebug()<<"从服务器收到的："<<recvStr;

//    if(recvStr.left(3) == "#02"){  //没解决登录成功会弹两次的问题，怀疑是数据粘包，希望用清空缓存区的方法试一下
//        QMessageBox::warning(this,"恭喜","登录成功!",QMessageBox::Ok);
//    }
//    else if(recvStr.left(3) == "#10"){

//    }
//    else if(recvStr.left(3) == "###"){
//        QStringList recvList = recvStr.mid(4).split("###");
//        int msgnum = recvList[0].right(1).toInt();
//    //    int sheID = recvList[0].mid(3,4).toInt();
//    //    int myID = recvList[0].mid(8,4).toInt();

//        for(int i = 1; i <= msgnum; i++ ){
//            QStringList msgList = recvList[i].split("|");
//            QString sendtime_ori = msgList[4];
//            QString sentence = msgList[5];
//            qDebug()<<sendtime_ori<<sentence;

//            QDateTime sendtime_trans = QDateTime::fromString(sendtime_ori, "yyyy-MM-dd hh:mm:ss");
//            QString sendtime = QString::number(sendtime_trans.toTime_t());

//            dealMessageTime(sendtime_ori);

//            QNChatMessage* messageW = new QNChatMessage(ui->chatMsgList->parentWidget());
//            QListWidgetItem* item = new QListWidgetItem(ui->chatMsgList);
//            dealMessage(messageW, item, sentence, sendtime_ori, QNChatMessage::User_She);
//        }
//    }

//}


void MainChatWin::on_closeBtn_clicked()
//#17|0001|0002
{
    QString type17 = "#17";
    slot_sendmessage(type17,"");
}


void MainChatWin::on_chathistoryBtn_clicked()
{
    QString type16 = "#16";
    slot_sendmessage(type16, "");
}
