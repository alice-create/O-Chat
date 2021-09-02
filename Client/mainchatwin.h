#ifndef MAINCHATWIN_H
#define MAINCHATWIN_H

#include <qnchatmessage.h>

#include <QWidget>
#include <QMainWindow>
#include <QDialog>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QListWidgetItem>   //列表框空间头文件

#include <QTcpSocket>
#include <QHostAddress>
#include <QtNetwork>

#include <QDebug>
#include <QSize>
#include <QColor>
#include <QString>
#include <QTextCodec>
#include <QDateTime>
#include <QMouseEvent>
#include <QRandomGenerator>
#include <QGraphicsDropShadowEffect>



QT_BEGIN_NAMESPACE
namespace Ui { class MainChatWin; }
QT_END_NAMESPACE

class MainChatWin : public QWidget
{
    Q_OBJECT

public:
    explicit MainChatWin(QWidget *parent = nullptr);
    explicit MainChatWin(QTcpSocket *sock = nullptr, /*QString messageStr = QString(),*/ QString thisID = QString(), QString thisName = QString(), QString thatID = QString(), QString thatName = QString(), QString state = QString(), QWidget *parent = nullptr);

    //父组件被delete时，Qt可以保证所有子组件也就是将parent指针指向当前组件的所有组件都会被delete掉
    ~MainChatWin() override;

    void dealMessage(QNChatMessage *messageW, QListWidgetItem *item, QString text, QString time, QNChatMessage::User_Type type);
    void dealMessageTime(QString curMsgTime);
    void send_tochat(QString messageStr);



protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

    void showSheMessage();

public slots:
    void on_sendBtn_clicked();  //发送按钮
    void slot_connected();    //连接到服务器
    void slot_sendmessage(QString type, QString message); //发送消息到服务器
//    void slot_recvmessage(); //接收来自服务器的消息
    void slot_disconnect();

private slots:
    void on_closeBtn_clicked();  //告诉服务器我把这个聊天框给关了


    void on_chathistoryBtn_clicked();

private:
    Ui::MainChatWin *ui;

    bool isconnection; //判断是否连接到服务器
    bool mMoveing;
    bool isShowTime = true;

    QString timeold;
    QString timenew;

    QString unprocessed_Data;
    QString s_thisID;
    QString s_thisName;
    QString s_thatID;
    QString s_thatName;
    QString s_state;

    QPoint mMovePosition;
    QTcpSocket *tcpSocket;
};
#endif // MAINCHATWIN_H
