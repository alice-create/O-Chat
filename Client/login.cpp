#include "login.h"
#include "signal.h"
#include "signup.h"
#include "ui_login.h"
#include "userlist.h"
#include "repword.h"
#include "mainchatwin.h"
#include <QDebug>
#include <QMessageBox>
#include <QApplication>

login::login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
//    this -> isconnection = false;
    sock = new QTcpSocket(this);  //初始化tcpSocket
    QString IP = "192.168.43.64";
    quint16 port = 8899;
    sock->connectToHost(IP, port);
    connect(sock,SIGNAL(connected()),this,SLOT(slot_connected()));
//    connect(sock, SIGNAL(readyRead()), this, SLOT(slot_recvmessage()));


//    this->resize(QSize(1000,1200));
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground,true);

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(20);
    shadow->setOffset(0,0);
    shadow->setColor(QColor(192,192,192));
    ui->frame->setGraphicsEffect(shadow);
}

login::login(QTcpSocket *socket, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
//    this -> isconnection = false;
    sock = socket;  //初始化tcpSocket
//    QString IP = "192.168.43.45";
//    quint16 port = 5678;
//    sock->connectToHost(IP, port);
//    connect(sock,SIGNAL(connected()),this,SLOT(slot_connected()));
    connect(sock, SIGNAL(readyRead()), this, SLOT(slot_recvmessage()));

//    this->resize(QSize(1000,1200));
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground,true);

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(20);
    shadow->setOffset(0,0);
    shadow->setColor(QColor(192,192,192));
    ui->frame->setGraphicsEffect(shadow);
}

login::~login()
{
    delete ui;
}

void login::mousePressEvent(QMouseEvent *event)
{
    mMoveing = true;
    mMovePosition = event->globalPos() - pos();
    QWidget::mousePressEvent(event);
}

void login::mouseMoveEvent(QMouseEvent *event)
{
    if(mMoveing & (event->buttons() & Qt::LeftButton)
            &(event->globalPos() - mMovePosition).manhattanLength()
            > QApplication::startDragDistance()) {
        move(event->globalPos() - mMovePosition);
        mMovePosition = event->globalPos() - pos();
    }
    QWidget::mouseMoveEvent(event);
}

void login::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    mMoveing = false;
}

void login::on_pushButton_2_clicked()
{
    //Sign up btn
    disconnect(sock, SIGNAL(readyRead()), this, nullptr);
    signup *signupwindow = new signup(sock);
    signupwindow -> show();
    this -> hide();
}

void login::on_pushButton_clicked()
{
    //Sign in btn
    QString uName = ui -> lineEdit -> text();
    QString pWord = ui -> lineEdit_2 -> text();
    if(pWord.contains("|") || pWord.contains("#")) {
        info * information = new info(sock, "02", "1", "3");
        information -> show();
    } else {
        QString sendData = "#02|" + uName + "|" + pWord ;
    //    qDebug() << sendData;
        sock -> write(sendData.toUtf8());
    }
}

void login::slot_connected()
{
    ui -> pushButton -> setEnabled(true);
    connect(sock, SIGNAL(readyRead()), this, SLOT(slot_recvmessage()));
}

void login::slot_recvmessage()
{
    QByteArray recvArray = sock -> readAll();
    qDebug() << "login rcvA:" << recvArray;
    QString recvStr(recvArray);
    if(recvStr != "")
    {
        QStringList recvList = recvStr.split("#");
        qDebug() << "login rcvL" << recvList;
        QStringList loginList = recvList[1].split("|");
        QString myName = loginList[2];
        QString myID = ui -> lineEdit -> text();
    //    quint16 friends = loginList[3].toUShort();
    //    qDebug() << friends;
        quint16 i;
        QStringList IDlist;
        QStringList Namelist;
        QStringList onlist;
        qDebug() <<"loginL:"<<loginList;
        if(loginList[1] == '0') {
//            QMessageBox::information(this, "Welcome", "登录成功");
            quint16 friends = loginList[3].toUShort();
//            qDebug() << friends;
            for(i = 2; i < friends + 2; i++) {
                QStringList indiList = recvList[i].split("|");
//                qDebug() << indiList;
                QString ID = indiList[2];
//                qDebug() << ID;
                QString Name = indiList[3];
//                qDebug() << Name;
                QString on_off = indiList[4];
//                qDebug() << on_off;
                IDlist.append(ID);
                Namelist.append(Name);
                onlist.append(on_off);
            }
            disconnect(sock, SIGNAL(readyRead()), this, nullptr);
            userlist *lists = new userlist(sock, myID, myName, friends, IDlist, Namelist, onlist);
            lists -> show();
            this -> hide();
    //        sock -> deleteLater();
        }
        else
        {
            if(loginList[2] == '0') {
                info *information = new info(sock, "02", "1", "0");
                information -> show();
            }
            if(loginList[2] == '1') {
                info *information = new info(sock, "02", "1", "1");
                information -> show();
            }
            if(loginList[2] == '2') {
                info *information = new info(sock, "02", "1", "2");
                information -> show();
            }
        }
    }


}


void login::on_pushButton_3_clicked()
{
    //password config btn
    disconnect(sock, SIGNAL(readyRead()), this, nullptr);
    repWord *repWordwindow = new repWord(sock);
    repWordwindow -> show();
    this -> hide();
}
