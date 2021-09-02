#include "user.h"
#include "ui_user.h"
#include <QMessageBox>
#include <QMouseEvent>

user::user(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::user)
{
    ui->setupUi(this);

    connect(this, SIGNAL(clicked()), this, SLOT(slot_chatbox()));
}

user::user(QTcpSocket *socket, QString mid, QString name, QString id, QString status, bool Sign, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::user)
{
    ui->setupUi(this);
    sock = socket;
    mID = mid;
    uID = id;
    sign = Sign;
    ID = ui -> label_2;
    ID ->setText("ID:" + id);
    uName =ui -> label;
    uName -> setText(name);
    head = ui -> widget_2;
    //好友状态
    if(status == "1")
    {
        on_off = ui -> label_3;
        on_off -> setText("ON");
    }
    else
    {
        on_off = ui -> label_3;
        on_off -> setText("OFF");
    }
    //是否有未读消息
    if(!Sign) {
        note = ui -> toolButton;
        note -> hide();
    }

//    qDebug() << ID;
    //
    /*重要：设置QListWidget的contextMenuPolicy属性，不然不能显示右键菜单*/
    this -> setProperty("contextMenuPolicy", Qt::CustomContextMenu);

    /*初始化一个包含两个Action(Delete和ClearAll)的菜单*/
    menu = new QMenu(this);
    del = new QAction(tr("Delete"), this);
    rename = new QAction(tr("Config Nickname"), this);
    menu -> addAction(del);
    menu -> addAction(rename);

    /*绑定右键显示菜单：在单击右键之后会执行槽函数， 槽函数中负责弹出右键菜单*/
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(on_user_customContextMenuRequested(const QPoint &pos)));

    //连接
    /*添加聊天窗口的响应函数*/
    connect(this, SIGNAL(clicked()), this, SLOT(slot_chatbox()));
    /*为菜单上的Delete选项添加响应函数*/
    connect(this -> del, SIGNAL(triggered()), this, SLOT(slot_delete()));
    connect(this -> rename, SIGNAL(triggered()), this, SLOT(slot_rename()));
}

user::~user()
{
    delete ui;
}

void user::slot_chatbox() {
//    QMessageBox::information(this, "yes", "leftbutton");
    sign = false;
    ui -> toolButton -> hide();
    QString sendData = "#07|" + mID + "|" + uID;
    sock -> write(sendData.toUtf8());
}

void user::slot_delete() {
    QString sendData = "#14|" + mID + "|" + uID;
    sock->write(sendData.toUtf8());
}

void user::slot_rename() {
    confignickname *info = new confignickname(sock, mID, uID);
    info -> show();
//    QString nickname = info -> nickname;
//    ui -> label -> setText(nickname);
}


void user::mousePressEvent(QMouseEvent *evt) {
    mousePos = QPoint(evt -> x(), evt -> y());

}

void user::mouseReleaseEvent(QMouseEvent *evt) {
    if(mousePos == QPoint(evt->x(), evt->y()) & evt -> button() & Qt::LeftButton)
    {
        emit clicked();
    }
//    if(mousePos == QPoint(evt->x(), evt->y()) & evt -> button & Qt::RightButton) {
//        emit clicked();
//    }
}

void user::on_user_customContextMenuRequested(const QPoint &pos)
{
    menu -> exec(QCursor::pos());
}



//void user::contextMenuEvent(QContextMenuEvent *evt) {

//    menu->addAction(new QAction(tr("&delete"), menu));
//    menu->move(cursor().pos()); //让菜单显示的位置在鼠标的坐标上
//    menu->show();
//}


