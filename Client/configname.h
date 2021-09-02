#ifndef CONFIGNAME_H
#define CONFIGNAME_H

#include <QWidget>
#include <QTcpSocket>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <ui_configname.h>
#include "info.h"

namespace Ui {
class configname;
}

class configname : public QWidget
{
    Q_OBJECT
    QString ID;
    QString Name;
    QString password;
    QString newname;

public:
    explicit configname(QWidget *parent = 0);
    explicit configname(QTcpSocket *socket = nullptr, QString mid = QString(), QString name = QString(), QWidget *parent = 0);
    ~configname() override;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void on_pushButton_clicked();

private:
    Ui::configname *ui;
    QTcpSocket * sock;
    bool mMoveing;
    QPoint mMovePosition;
};

#endif // CONFIGNAME_H
