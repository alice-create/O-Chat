#include "signup.h"
#include "login.h"
#include "ui_signup.h"
#include <QMessageBox>

signup::signup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::signup)
{
    ui->setupUi(this);

}

signup::signup(QTcpSocket* socket, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::signup)
{
    ui->setupUi(this);
    sock = socket;
//    sock = new QTcpSocket(this);
//    QString IP = "192.168.43.143";
//    quint16 port = 1023;
//    sock->connectToHost(IP, port);

    connect(sock, SIGNAL(readyRead()), this, SLOT(slot_recvmessage()));

//    resize(400,300);
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground,true);

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(20);
    shadow->setOffset(0,0);
    shadow->setColor(QColor(192,192,192));
    ui->frame->setGraphicsEffect(shadow);
}

signup::~signup()
{
    delete ui;
}

void signup::mousePressEvent(QMouseEvent *event)
{
    mMoveing = true;
    mMovePosition = event->globalPos() - pos();
    QWidget::mousePressEvent(event);
}

void signup::mouseMoveEvent(QMouseEvent *event)
{
    if(mMoveing & (event->buttons() & Qt::LeftButton)
            &(event->globalPos() - mMovePosition).manhattanLength()
            > QApplication::startDragDistance()) {
        move(event->globalPos() - mMovePosition);
        mMovePosition = event->globalPos() - pos();
    }
    QWidget::mouseMoveEvent(event);
}

void signup::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    mMoveing = false;
}


//bool signup::signupConstruct() {
//    connect(sock, SIGNAL(connected()), this, SLOT(slot_connected()));
//    return true;
//}

//signup* signup::signupNewInstance(QTcpSocket *socket) {
//    signup* ret = new signup();
//    sock = socket;
//    if(!(ret && ret -> signupConstruct())) {
//        delete ret;
//        ret = NULL;
//    }
//    return ret;
//}

void signup::on_pushButton_clicked()
{
    disconnect(sock, SIGNAL(readyRead()), this, 0);
    login *loginwindow = new login(sock);
    loginwindow -> show();
    this -> hide();
}

void signup::on_pushButton_2_clicked()
{
    QString uName = ui -> lineEdit -> text();
    QString pWord = ui -> lineEdit_2 -> text();
    if(pWord.contains("|") || pWord.contains("#")) {
        info *information = new info(sock, "01", "1", "2");
        information -> show();

    } else {
        QString sendData = "#01|" + uName + "|" + pWord;
        sock -> write(sendData.toUtf8());
    }

}



void signup::slot_recvmessage() {
    QByteArray recvArray = sock -> readAll();
    qDebug() << recvArray;
    QString recvStr(recvArray);
    QStringList recvList = recvStr.split("|");

    if(recvList[1] == '0') {
        QString ID = recvList[3];
        info * information = new info(sock, "01", "0", ID);
        information -> show();
//        sock -> close();
    } else {
        if(recvList[2] == '0') {
            info *information = new info(sock, "01", "1", "0");
            information -> show();
        }
        if(recvList[2] == "1") {
            info *information = new info(sock, "01", "1", "1");
            information -> show();
        }
    }
}

