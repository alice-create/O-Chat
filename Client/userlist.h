#ifndef USERLIST_H
#define USERLIST_H

#include <QWidget>
#include <QLabel>
#include <QListWidget>
#include <QWidget>
#include <user.h>
#include <QLayout>
#include <QLayoutItem>
#include <QMessageBox>
#include <QMap>
#include <QAction>
#include <QMenu>

#include "mainchatwin.h"
#include "configname.h"
#include "info.h"

namespace Ui {
class userlist;
}

class userlist : public QWidget
{
    Q_OBJECT
    MainChatWin *chatwindow;
    QListWidget *lists; //容器列表
    QWidget *person;    //每一个用户
    QLabel *myname; //本人姓名
    QLabel *myID;   //本人ID
    QPushButton *off;
    quint16 people;     //好友人数
    QString mID;     //我的ID
    QString mName;      //我的姓名
    QStringList IDlist; //好友ID列表
    QStringList Namelist;   //好友姓名列表
    QStringList onlist; //好友在线状态


public:
    explicit userlist(QWidget *parent = nullptr);
    explicit userlist(QTcpSocket *socket = nullptr, QString id = QString(), QString name = QString(), quint16 people = 0, QStringList IDlist = QStringList(), QStringList Namelist = QStringList(), QStringList onlist = QStringList(), QWidget *parent = nullptr);
    ~userlist();
    void initList();

private slots:
    void on_pushButton_clicked();
    void slot_recvmessage();
    void slot_changename();
    void on_pushButton_2_clicked();
    void on_widget_customContextMenuRequested(const QPoint &pos);

signals:
    void clicked();

protected:
    virtual void mousePressEvent(QMouseEvent *evt);
    virtual void mouseReleaseEvent(QMouseEvent *evt);

private:
    Ui::userlist *ui;
    QTcpSocket *sock;
    QMenu *menu;
    QAction *rename;
    QPoint mousePos;
};

#endif // USERLIST_H
