#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QtNetwork/QTcpSocket>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <ui_login.h>

#include <info.h>

namespace Ui {
class login;
}

class login : public QWidget
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    explicit login(QTcpSocket * socket, QWidget *parent = nullptr);
    ~login() override;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void slot_connected();    //连接到服务器
    void slot_recvmessage(); //接收来自服务器的消息

    void on_pushButton_3_clicked();

private:
    Ui::login *ui;
    QTcpSocket *sock;
    bool mMoveing;
    QPoint mMovePosition;

};

#endif // LOGIN_H
