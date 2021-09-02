#ifndef USER_H
#define USER_H

#include <QWidget>
#include <QLabel>
#include <QWidget>
#include <QToolButton>
#include <QTcpSocket>
#include <QMenu>
#include <QAction>

#include "confignickname.h"


namespace Ui {
class user;
}

class user : public QWidget
{
    Q_OBJECT
    QLabel *uName;
    QLabel *ID;
    QLabel *on_off;
    QToolButton *note;
    QWidget *head;
    QString mID;
    QString uID;
    bool sign;


public:
    explicit user(QWidget *parent = nullptr);
    explicit user(QTcpSocket *socket = nullptr, QString mid = QString(), QString mname = QString(), QString id = QString(), QString status = QString(), bool Sign = false, QWidget *parent = nullptr);
    ~user();


signals:
    void clicked();

private slots:
    void slot_chatbox();
    void slot_delete();
    void slot_rename();
    void on_user_customContextMenuRequested(const QPoint &pos);


//    void on_label_objectNameChanged(const QString &objectName);

private:
    Ui::user *ui;
    QTcpSocket *sock;
    QPoint mousePos;
    QMenu *menu;
    QAction *del;
    QAction *rename;

protected:
    virtual void mousePressEvent(QMouseEvent *evt);
    virtual void mouseReleaseEvent(QMouseEvent *evt);
//    virtual void contextMenuEvent(QContextMenuEvent *evt);


};

#endif // USER_H
