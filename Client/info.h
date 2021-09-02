#ifndef INFO_H
#define INFO_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTcpSocket>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>


namespace Ui {
class info;
}

class info : public QWidget
{
    Q_OBJECT
    QLabel *infomation;
    QPushButton *confirm;
    QString No;
    QString ID;
    QString B_ID;

public:
    explicit info(QWidget *parent = nullptr);
    explicit info(QTcpSocket* socket = nullptr, QString no = QString(), QString id = QString(), QString b_id = QString(), QWidget *parent = nullptr);
    void initContent();
    ~info() override;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void on_pushButton_clicked();

private:
    Ui::info *ui;
    QTcpSocket *sock;
    bool mMoveing;
    QPoint mMovePosition;
};

#endif // INFO_H
