#ifndef ADDFRIEND_H
#define ADDFRIEND_H

#include <QWidget>
#include <QTcpSocket>
#include <QLabel>
#include <QPushButton>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>

namespace Ui {
class addFriend;
}

class addFriend : public QWidget
{
    Q_OBJECT
    QLabel *information;
    QPushButton *confirm;
    QPushButton *reject;
    QString No;
    QString ID;
    QString B_ID;

public:
    explicit addFriend(QWidget *parent = nullptr);
    explicit addFriend(QTcpSocket *socket = nullptr, QString no = QString(),QString id = QString(), QString b_id = QString(), QWidget *parent = nullptr);
    ~addFriend() override;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::addFriend *ui;
    QTcpSocket *sock;
    bool mMoveing;
    QPoint mMovePosition;
};

#endif // ADDFRIEND_H
