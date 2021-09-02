#ifndef CONFIGNICKNAME_H
#define CONFIGNICKNAME_H

#include <QWidget>
#include <QTcpSocket>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>

#include "info.h"
#include "ui_confignickname.h"

namespace Ui {
class confignickname;
}

class confignickname : public QWidget
{
    Q_OBJECT
    QString mID;
    QString uID;
    QString nickname;

public:
//    QString nickname;
    explicit confignickname(QWidget *parent = 0);
    explicit confignickname(QTcpSocket *socket = nullptr, QString mid = QString(), QString mname = QString(), QWidget *parent = 0);
    ~confignickname() override;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void on_pushButton_clicked();

private:
    Ui::confignickname *ui;
    QTcpSocket *sock;
    bool mMoveing;
    QPoint mMovePosition;
};

#endif // CONFIGNICKNAME_H
