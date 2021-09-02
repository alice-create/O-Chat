#ifndef REPWORD_H
#define REPWORD_H

#include <QWidget>
#include <QTcpSocket>
#include <QPushButton>
#include <QMessageBox>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <login.h>

#include "info.h"

namespace Ui {
class repWord;
}

class repWord : public QWidget
{
    Q_OBJECT
    QPushButton *back;
    QPushButton *confirm;

public:
    explicit repWord(QWidget *parent = nullptr);
    explicit repWord(QTcpSocket *socket, QWidget *parent = nullptr);
    ~repWord();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void on_pushButton_clicked();
    void slot_recvmessage();
    void on_pushButton_2_clicked();

private:
    Ui::repWord *ui;
    QTcpSocket *sock;

    bool mMoveing;
    QPoint mMovePosition;
};

#endif // REPWORD_H
