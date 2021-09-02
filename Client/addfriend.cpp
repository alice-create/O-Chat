#include "addfriend.h"
#include "ui_addfriend.h"

addFriend::addFriend(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::addFriend)
{
    ui->setupUi(this);
}

addFriend::addFriend(QTcpSocket *socket, QString no, QString id, QString b_id, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::addFriend)
{
    ui->setupUi(this);
    sock = socket;

    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground,true);

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(20);
    shadow->setOffset(0,0);
    shadow->setColor(QColor(192,192,192));
    ui->frame->setGraphicsEffect(shadow);


    information = ui -> label;
    confirm = ui -> pushButton;
    reject = ui -> pushButton_2;
    No = no;
    ID = id;
    B_ID = b_id;
    QString otp = B_ID + " send a request!";
    information -> setText(otp);
}

addFriend::~addFriend()
{
    delete ui;
}

void addFriend::mousePressEvent(QMouseEvent *event)
{
    mMoveing = true;
    mMovePosition = event->globalPos() - pos();
    QWidget::mousePressEvent(event);
}

void addFriend::mouseMoveEvent(QMouseEvent *event)
{
    if(mMoveing & (event->buttons() & Qt::LeftButton)
            &(event->globalPos() - mMovePosition).manhattanLength()
            > QApplication::startDragDistance()) {
        move(event->globalPos() - mMovePosition);
        mMovePosition = event->globalPos() - pos();
    }
    QWidget::mouseMoveEvent(event);
}

void addFriend::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    mMoveing = false;
}

void addFriend::on_pushButton_clicked()
{
    QString sendData = "#05|" + ID + "|" + B_ID;
    sock->write(sendData.toUtf8());
    this -> hide();
}

void addFriend::on_pushButton_2_clicked()
{
    QString sendData = "#06|" + ID + "|" + B_ID;
    sock->write(sendData.toUtf8());
    this -> hide();
}
