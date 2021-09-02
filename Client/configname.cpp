#include "configname.h"
//#include "ui_configname.h"

configname::configname(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::configname)
{
    ui->setupUi(this);
}

configname::configname(QTcpSocket *socket, QString mid, QString name, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::configname)
{
    ui->setupUi(this);
    sock = socket;
    ID = mid;
    Name = name;

    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground,true);

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(20);
    shadow->setOffset(0,0);
    shadow->setColor(QColor(192,192,192));
    ui->frame->setGraphicsEffect(shadow);
}

configname::~configname()
{
    delete ui;
}

void configname::mousePressEvent(QMouseEvent *event)
{
    mMoveing = true;
    mMovePosition = event->globalPos() - pos();
    QWidget::mousePressEvent(event);
}

void configname::mouseMoveEvent(QMouseEvent *event)
{
    if(mMoveing & (event->buttons() & Qt::LeftButton)
            &(event->globalPos() - mMovePosition).manhattanLength()
            > QApplication::startDragDistance()) {
        move(event->globalPos() - mMovePosition);
        mMovePosition = event->globalPos() - pos();
    }
    QWidget::mouseMoveEvent(event);
}

void configname::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    mMoveing = false;
}


void configname::on_pushButton_clicked()
{
    password = ui -> lineEdit -> text();
    newname = ui -> lineEdit_2 -> text();
    QString sendData = "#12|" + ID + "|" + password+ "|" + newname;
    sock -> write(sendData.toUtf8());
    this -> hide();
}
