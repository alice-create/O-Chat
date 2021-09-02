#include "userlist.h"
#include "ui_userlist.h"
#include "info.h"
#include "addfriend.h"

userlist::userlist(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::userlist)
{
    ui->setupUi(this);
//    sock = new QTcpSocket(this);
//    lists = ui -> listWidget;
//    person = new user();
//    QListWidgetItem *itemN = new QListWidgetItem();
//    lists -> addItem(itemN);
//    lists -> setItemWidget(itemN, person);
//    QString IP = "192.168.43.143";
//    quint16 port = 1023;
//    sock->connectToHost(IP, port);
//    connect(sock, SIGNAL(connected()), this, SLOT(slot_connected()));
}

userlist::userlist(QTcpSocket *socket,QString id, QString name, quint16 People, QStringList IDList, QStringList NameList, QStringList onList, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::userlist)
{
    ui->setupUi(this);
//    this->setWindowFlags(Qt::FramelessWindowHint);
    sock = socket;
    lists = ui -> listWidget;   //框架
    /*生成个人信息*/
    off = ui -> pushButton_2;
    myname = ui -> label;
    myID = ui -> label_2;
    off -> setText("Off-Line");
    mID = id;
    mName = name;
    myID -> setText("ID:" + mID);
    myname -> setText(mName);
    /*好友列表*/
    people = People;
    IDlist = IDList;
    Namelist = NameList;
    onlist = onList;

    initList();

    /*重要：设置QListWidget的contextMenuPolicy属性，不然不能显示右键菜单*/
    ui -> widget -> setProperty("contextMenuPolicy", Qt::CustomContextMenu);

    /*初始化一个包含rename的菜单*/
    menu = new QMenu(this);
    rename = new QAction(tr("Config Username"), this);
    menu -> addAction(rename);

    /*绑定右键显示菜单：在单击右键之后会执行槽函数， 槽函数中负责弹出右键菜单*/
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(on_widget_customContextMenuRequested(const QPoint &pos)));

    /*为菜单上的Delete选项添加响应函数*/
    connect(this -> rename, SIGNAL(triggered()), this, SLOT(slot_changename()));
    connect(sock, SIGNAL(readyRead()), this, SLOT(slot_recvmessage()));
}

userlist::~userlist()
{
    delete ui;
}

void userlist::on_pushButton_clicked()
{
    QString b_id = ui -> lineEdit -> text();
    QString id = mID;
    QString sendData = "#04|" + id + "|" + b_id;
    qDebug() << sendData;
    sock -> write(sendData.toUtf8());

}

void userlist::slot_recvmessage() {
    QByteArray recvArray = sock -> readAll();
    qDebug() << recvArray;
    QString recvStr(recvArray);
    qDebug() << recvStr;
    QStringList recvList = recvStr.split("|");
    if(recvList[0] == "#04") {//发送好友邀请
//        qDebug() << recvStr;
        if(recvList[1] == "0") {
            info *information = new info(sock, "04", "0", "0");
            information -> show();
        }
        else
        {
            if(recvList[2] == "0") {
                info *information = new info(sock, "04", "1", "0");
                information -> show();
            }
            if(recvList[2] == "1") {
                info *information = new info(sock, "04", "1", "1");
                information -> show();
            }
            if(recvList[2] == "2") {
                info *information = new info(sock, "04", "1", "2");
                information -> show();
            }
        }
    }
    if(recvList[0] == "###05") {//展示邀请
        QStringList noList = recvList[0].split("###");
        QString no = noList[1];
        QString id = recvList[1];
        QString b_id = recvList[2];
        info *infomation = new info(sock, no, id, b_id);
        infomation -> show();
    }
    if(recvList[0] == "###04")
    {
        QStringList noList = recvList[0].split("###");
        QString no = noList[1];
        QString id = recvList[1];
        QString b_id = recvList[2];
        addFriend *infomation = new addFriend(sock, no, id, b_id);
        infomation -> show();
    }
    if(recvList[0] == "###07")//同意
    {
        qDebug() << "###07";
        QStringList noList = recvList[0].split("###");
        QString no = noList[1];
        QString id = recvList[1];
        QString b_id = recvList[2];
        QString Name = recvList[3];
        QString status = recvList[4];
        people += 1;
        IDlist.append(b_id);
        Namelist.append(Name);
        onlist.append(status);
        person = new user(sock, mID, Name, b_id, status, false);
        QListWidgetItem *itemN = new QListWidgetItem();
        itemN -> setSizeHint(QSize(280, 80));
        lists -> addItem(itemN);
        lists -> setItemWidget(itemN, person);
//        info *infomation = new info(sock, no, id, b_id);
//        infomation -> show();
    }
    if(recvList[0] == "###00")
        //chatting
    {
            quint16 row = 0;
            quint16 i = 0;
            QString b_id = recvList[2];
            QString is_chat = recvList[3];
            QString s;
            QString n;
            if(is_chat == "0") {
                for(i = 0; i < people; i++) {
                    if(IDlist[i] == b_id) {
                        row = i;

                        s = onlist[i];
                        n = Namelist[i];

                        if(s == "0") {
                            onlist.removeAt(row);
                            Namelist.removeAt(row);
                            IDlist.removeAt(row);
                            QListWidgetItem* item = lists -> takeItem(row);
                            lists -> removeItemWidget(item);
                            onlist.append(s);
                            Namelist.append(n);
                            IDlist.append(b_id);
                            person = new user(sock, mID, n, b_id, s, true);
                            QListWidgetItem *itemN = new QListWidgetItem();
                            itemN -> setSizeHint(QSize(280, 80));
                            lists -> addItem(itemN);
                            lists -> setItemWidget(itemN, person);
                            break;

                        }
                        else {
                            QListWidgetItem* item = lists -> takeItem(row);
                            lists -> removeItemWidget(item);
                            person = new user(sock, mID, n, b_id, s, true);
                            QListWidgetItem *itemN = new QListWidgetItem();
                            itemN -> setSizeHint(QSize(280, 80));
                            lists -> insertItem(row, itemN);
                            lists -> setItemWidget(itemN, person);
                            break;
                        }

                    }
                }
            }
            else {
                //传消息
                chatwindow -> MainChatWin::send_tochat(recvStr);
            }
     }
    if(recvList[0] == "#07") {//点开聊天框
        if(recvList[1] == "1") {
            info *information = new info(sock, "07", "1", "0");
            information -> show();
        } else {
            quint16 i;
            QString uID = recvList[2];
            QString uName = recvList[3];
            QString s;
            for(i = 0; i < people; i++) {
                if(IDlist[i] == uID) {
                    s = onlist[i];
                    break;
                }
            }
            chatwindow = new MainChatWin(sock, mID, mName, uID, uName, s);
            chatwindow -> show();
        }
    }
    if(recvList[0] == "###03") {//好友申请
            QString b_id = recvList[2];
            QString have_info = recvList[3];
            bool flag = false;
            if(have_info == "0") flag = false;
            else flag = true;
            info *information = new info(sock, "###03", "0", b_id);
            information -> show();
            QString s = "0";
            QString n;
            quint16 row = 0;
            quint16 i = 0;
            for(i = 0; i < people; i++) {
                if(IDlist[i] == b_id) {
                    row = i;
                    n = Namelist[i];
                    if(s == "0") {
                        onlist.removeAt(row);
                        Namelist.removeAt(row);
                        IDlist.removeAt(row);
                        QListWidgetItem* item = lists -> takeItem(row);
                        lists -> removeItemWidget(item);
                        onlist.append("0");
                        Namelist.append(n);
                        IDlist.append(b_id);
                        person = new user(sock, mID, n, b_id, s, flag);
                        QListWidgetItem *itemN = new QListWidgetItem();
                        itemN -> setSizeHint(QSize(280, 80));
                        lists -> addItem(itemN);
                        lists -> setItemWidget(itemN, person);
                        break;
                    }
                }
            }
    }
    if(recvList[0] == "###02") {//下线
            qDebug() << "###02";
            QString b_id = recvList[2];
            QString have_info = recvList[3];
            bool flag = false;
            if(have_info == "0") flag = false;
            else flag = true;
            info *information = new info(sock, "###02", "0", b_id);
            information -> show();
            QString s = "1";
            QString n;
            quint16 row = 0;
            quint16 i = 0;
            for(i = 0; i < people; i++) {
                qDebug() << "i:" << i;
                if(IDlist[i] == b_id) {
                    row = i;
                    n = Namelist[i];
                    if(s == "1") {
                        onlist[i] = "1";
                        QListWidgetItem* item = lists -> takeItem(row);
                        lists -> removeItemWidget(item);
                        person = new user(sock, mID, n, b_id, s, flag);
                        QListWidgetItem *itemN = new QListWidgetItem();
                        itemN -> setSizeHint(QSize(280, 80));
                        lists -> insertItem(row, itemN);
                        lists -> setItemWidget(itemN, person);
                        break;
                    }
                }
            }

        }
    if(recvList[0] == "#14") {//删除好友
        QString if_success = recvList[1];
        QString b_id = recvList[2];
        if(if_success == "0") {
            info *information = new info(sock, "#14", "0", "0");
            information -> show();
            QString n;
            quint16 row = 0;
            quint16 i = 0;
            for(i = 0; i < people; i++) {
                if(IDlist[i] == b_id) {
                    row = i;
                    n = Namelist[i];
                    onlist.removeAt(row);
                    Namelist.removeAt(row);
                    IDlist.removeAt(row);
                    QListWidgetItem* item = lists -> takeItem(row);
                    lists -> removeItemWidget(item);
                    people -= 1;
                    break;
                }
            }

        }
        else {
            info * information = new info(sock, "#14", "1", "0");
            information -> show();
        }
    }
    if(recvList[0] == "#11") {//修改备注
        QString is_success = recvList[1];
        QString id = recvList[2];
        QString nickname = recvList[3];
        QString s;
        QString is_info = recvList[4];
        bool flag = false;
        if(is_info == "0") flag = false;
        else flag = true;
        if(is_success == "0") {
            info *information = new info(sock, "11", "0", "0");
            information -> show();
            for(quint16 i = 0; i < people; i++) {
                if(IDlist[i] == id) {
                    Namelist[i] = nickname;
                    s = onlist[i];
                    QListWidgetItem* item = lists -> takeItem(i);
                    lists -> removeItemWidget(item);
                    person = new user(sock, mID, nickname, id, s, flag);
                    QListWidgetItem *itemN = new QListWidgetItem();
                    itemN -> setSizeHint(QSize(280, 80));
                    lists -> insertItem(i, itemN);
                    lists -> setItemWidget(itemN, person);
                    break;
                }
            }
        } else {
            info *information = new info(sock, "11", "1", "0");
            information -> show();
        }

    }
    if(recvList[0] == "#12") {//修改昵称
         QString is_success = recvList[1];
         if(is_success == "0") {
             info *information = new info(sock, "12", "0", "0");
             information -> show();
             QString newname = recvList[4];
             myname -> setText(newname);
         } else {
             QString errortype = recvList[2];
             if(errortype == "0") {
                 info *information = new info(sock, "12", "1", "0");
                 information -> show();
             }
             if(errortype == "1") {
                 info *information = new info(sock, "12", "1", "1");
                 information -> show();
             }
             if(errortype == "2") {
                 info *information = new info(sock, "12", "1", "2");
                 information -> show();
             }
         }

    }
    if(recvList[0] == "###06") {
        QString uid = recvList[2];
        QString n;
        quint16 row = 0;
        quint16 i = 0;
        for(i = 0; i < people; i++) {
            if(IDlist[i] == uid) {
                row = i;
                n = Namelist[i];
                onlist.removeAt(row);
                Namelist.removeAt(row);
                IDlist.removeAt(row);
                QListWidgetItem* item = lists -> takeItem(row);
                lists -> removeItemWidget(item);
                people -= 1;
                break;
             }
         }
    }
    if(recvList[0] == "#16") {//历史消息
         chatwindow -> send_tochat(recvStr);
    }
}

void userlist::initList() {
    quint16 i;
    for(i = 0; i < people; i++) {
        QString Name = Namelist[i];
        QString ID = IDlist[i];
        QString status =onlist[i];
        person = new user(sock, mID, Name, ID, status, false);
        QListWidgetItem *itemN = new QListWidgetItem();
        itemN -> setSizeHint(QSize(280, 80));
        lists -> addItem(itemN);
        lists -> setItemWidget(itemN, person);

    }

}

void userlist::on_pushButton_2_clicked()
{
    QString sendData = "#03|" + mID;
    sock->write(sendData.toUtf8());
    this -> close();
}


//修改名字
void userlist::slot_changename() {

    configname *info = new configname(sock, mID, mName);
    info -> show();
//    QString nickname = info -> nickname;
//    ui -> label -> setText(nickname);
}

void userlist::mousePressEvent(QMouseEvent *evt) {
    mousePos = QPoint(evt -> x(), evt -> y());

}

void userlist::mouseReleaseEvent(QMouseEvent *evt) {
    if(mousePos == QPoint(evt->x(), evt->y()) & evt -> button() & Qt::LeftButton)
    {
        emit clicked();
    }
}

void userlist::on_widget_customContextMenuRequested(const QPoint &pos)
{
    menu -> exec(QCursor::pos());
}
