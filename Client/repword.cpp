#include "repword.h"
#include "ui_repword.h"


repWord::repWord(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::repWord)
{
    ui->setupUi(this);
}

repWord::repWord(QTcpSocket *socket, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::repWord)
{
    ui->setupUi(this);
    sock = socket;
    back = ui -> pushButton_2;
    confirm = ui -> pushButton;
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


repWord::~repWord()
{
    delete ui;
}

void repWord::mousePressEvent(QMouseEvent *event)
{
    mMoveing = true;
    mMovePosition = event->globalPos() - pos();
    QWidget::mousePressEvent(event);
}

void repWord::mouseMoveEvent(QMouseEvent *event)
{
    if(mMoveing & (event->buttons() & Qt::LeftButton)
            &(event->globalPos() - mMovePosition).manhattanLength()
            > QApplication::startDragDistance()) {
        move(event->globalPos() - mMovePosition);
        mMovePosition = event->globalPos() - pos();
    }
    QWidget::mouseMoveEvent(event);
}

void repWord::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    mMoveing = false;
}

void repWord::on_pushButton_clicked()
{
    QString  ID = ui -> lineEdit-> text();
    QString _old = ui -> lineEdit_2 -> text();
    QString _new = ui -> lineEdit_3 -> text();
    QString _yes = ui -> lineEdit_4 -> text();
    if(_yes == _new) {
        if(_new.contains("|") || _new.contains("#")) {
            info *information = new info(sock, "#13", "1", "3");
            information -> show();
        } else {
            QString sendData = "#13|" + ID + "|" + _old + "|" +  _new;
            sock -> write(sendData.toUtf8());
        }
    } else {
        info *information = new info(sock, "#13", "1", "2");
        information -> show();
    }
}

void repWord::on_pushButton_2_clicked()
{
    disconnect(sock, SIGNAL(readyRead()), this, 0);
    login * loginwindow = new login(sock);
    loginwindow -> show();
    this -> hide();
}

void repWord::slot_recvmessage()
{
    QByteArray recvArray = sock -> readAll();
    QString recvStr(recvArray);
    QStringList recvList = recvStr.split("|");
    if(recvList[1] == "0") {
        disconnect(sock, SIGNAL(readyRead()),this, SLOT(slot_recvmessage()));
        info *information = new info(sock, "#13", "0", "0");
        information -> show();
        this -> hide();
    }
    else
    {
        if(recvList[2] == "0") {
            info *information = new info(sock, "#13", "1", "0");
            information -> show();

        }
        if(recvList[2] == "1") {
            info *information = new info(sock, "#13", "1", "1");
            information -> show();

        }
    }
}
