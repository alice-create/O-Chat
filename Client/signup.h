#ifndef SIGNUP_H
#define SIGNUP_H

#include <QWidget>
#include <QTcpSocket>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>

#include "info.h"


namespace Ui {
class signup;
}

class signup : public QWidget
{
    Q_OBJECT

public:
    explicit signup(QWidget *parent = nullptr);
    explicit signup(QTcpSocket *socket = nullptr, QWidget *parent = nullptr);
    ~signup() override;
//    bool signupConstruct();
//    static signup* signupNewInstance(QTcpSocket *socket);


protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
//    void slot_connected();
    void slot_recvmessage();

private:
    Ui::signup *ui;
    QTcpSocket *sock;
    bool mMoveing;
    QPoint mMovePosition;



};


#endif // SIGNUP_H
