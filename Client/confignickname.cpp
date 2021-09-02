#include "confignickname.h"

confignickname::confignickname(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::confignickname)
{
    ui->setupUi(this);
}

confignickname::confignickname(QTcpSocket *socket, QString mid, QString uid, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::confignickname)
{
    ui->setupUi(this);
    sock = socket;
    mID = mid;
    uID = uid;

    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground,true);

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(20);
    shadow->setOffset(0,0);
    shadow->setColor(QColor(192,192,192));
    ui->frame->setGraphicsEffect(shadow);
}

confignickname::~confignickname()
{
    delete ui;
}

void confignickname::mousePressEvent(QMouseEvent *event)
{
    mMoveing = true;
    mMovePosition = event->globalPos() - pos();
    QWidget::mousePressEvent(event);
}

void confignickname::mouseMoveEvent(QMouseEvent *event)
{
    if(mMoveing & (event->buttons() & Qt::LeftButton)
            &(event->globalPos() - mMovePosition).manhattanLength()
            > QApplication::startDragDistance()) {
        move(event->globalPos() - mMovePosition);
        mMovePosition = event->globalPos() - pos();
    }
    QWidget::mouseMoveEvent(event);
}

void confignickname::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    mMoveing = false;
}


void confignickname::on_pushButton_clicked()
{
    nickname = ui -> lineEdit -> text();
    QString sendData = "#11|" + mID + "|" + uID + "|" + nickname;
    sock -> write(sendData.toUtf8());
    this -> hide();
}
