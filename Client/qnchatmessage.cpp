#include "qnchatmessage.h"
#include <QFontMetrics>
#include <QPaintEvent>
#include <QDateTime>
#include <QPainter>
#include <QMovie>
#include <QLabel>
#include <QDebug>

QNChatMessage::QNChatMessage(QWidget *parent) : QWidget(parent)
{
    QFont te_font = this->font();
    te_font.setFamily("MicrosoftYaHei");
    te_font.setPointSize(12);  //预设字号和字体
//    te_font.setWordSpacing(0);
//    te_font.setLetterSpacing(QFont::PercentageSpacing,0);
//    te_font.setLetterSpacing(QFont::PercentageSpacing, 100);          //300%,100为默认  //设置字间距%
//    te_font.setLetterSpacing(QFont::AbsoluteSpacing, 0);             //设置字间距为3像素 //设置字间距像素值
    this->setFont(te_font);
    m_leftPixmap = QPixmap(":/img/otherside.png"); //这是对方的头像
    m_rightPixmap = QPixmap(":/img/myside.png");   //这是我自己

    m_loadingMovie = new QMovie(this);  //loading动画的设置
    m_loadingMovie->setFileName(":/img/loading4.gif");
    m_loading = new QLabel(this);
    m_loading->setMovie(m_loadingMovie);
    m_loading->resize(16,16);
    m_loading->setAttribute(Qt::WA_TranslucentBackground , true);
    m_loading->setAutoFillBackground(false);
}

void QNChatMessage::setTextSuccess() 
//确认发送，loading标志hide，状态更改为已发送
{

    m_loading->hide();
    m_loadingMovie->stop();
    m_isSending = true;
}

void QNChatMessage::setText(QString text, QString time, QSize allSize, QNChatMessage::User_Type userType)
//这个函数里传入文本内容，时间，大小和用户类型并设置loading
{
    m_msg = text;
    m_time = time;  //message的时间是settext传进来的时间
    m_allSize = allSize;
    m_userType = userType;

//    QDateTime curDateTime = QDateTime::currentDateTime();
//    m_curTime = curDateTime.toString("yyyy-MM-dd hh:mm:ss");

    if(userType == User_Me) {  //本地用户发送信息
        if(!m_isSending) {
            m_loading->move(m_kuangRightRect.x() - m_loading->width() - 10, m_kuangRightRect.y()+m_kuangRightRect.height()/2- m_loading->height()/2);
            m_loading->show();
            m_loadingMovie->start();
        }
    }
    else if(userType == User_She) {   //对方用户发送信息
        m_loading->hide();
    }
    else if(userType == User_Time){
        m_curTime = m_time;
    }

    this->update();
}

QSize QNChatMessage::fontRect(QString str)
{
    m_msg = str;
    int minHei = 35;
    int iconWH = 25;  //头像大小
    int iconSpaceW = 35;  //头像和边框的距离
    int iconRectW = 5;
    int iconTMPH = 5;
    int sanJiaoW = 6;
    int kuangTMP = 20;
    int textSpaceRect = 12;
    m_kuangWidth = this->width() - kuangTMP - 2*(iconWH+iconSpaceW+iconRectW);
    m_textWidth = m_kuangWidth - 2*textSpaceRect; //我给它的文本宽度
    m_spaceWid = this->width() - m_textWidth;
    m_iconLeftRect = QRect(iconSpaceW, iconTMPH, iconWH, iconWH);  //左头像坐标设置
    m_iconRightRect = QRect(this->width() - iconSpaceW - iconWH, iconTMPH, iconWH, iconWH);//右头像坐标设置

    QSize size = getRealString(m_msg); // msg整个的size

    qDebug() << "fontRect Size:" << size;
    int hei = size.height() < minHei ? minHei : size.height();

    m_sanjiaoLeftRect = QRect(iconWH+iconSpaceW+iconRectW, m_lineHeight/2, sanJiaoW, hei - m_lineHeight);
    m_sanjiaoRightRect = QRect(this->width() - iconRectW - iconWH - iconSpaceW - sanJiaoW, m_lineHeight/2, sanJiaoW, hei - m_lineHeight);

    if(size.width() < (m_textWidth+m_spaceWid)) {//它的真实宽度小于（我给的宽度加上气泡两侧留白）
        m_kuangLeftRect.setRect(m_sanjiaoLeftRect.x()+m_sanjiaoLeftRect.width(), m_lineHeight/4*3, size.width()-m_spaceWid+2*textSpaceRect, hei-m_lineHeight);
        m_kuangRightRect.setRect(this->width() - size.width() + m_spaceWid - 2*textSpaceRect - iconWH - iconSpaceW - iconRectW - sanJiaoW,
                                 m_lineHeight/4*3, size.width()-m_spaceWid+2*textSpaceRect, hei-m_lineHeight);
    } else {
        m_kuangLeftRect.setRect(m_sanjiaoLeftRect.x()+m_sanjiaoLeftRect.width(), m_lineHeight/4*3, m_kuangWidth, hei-m_lineHeight);
        m_kuangRightRect.setRect(iconWH + kuangTMP + iconSpaceW + iconRectW - sanJiaoW, m_lineHeight/4*3, m_kuangWidth, hei-m_lineHeight);
    }
    m_textLeftRect.setRect(m_kuangLeftRect.x()+textSpaceRect,m_kuangLeftRect.y()+iconTMPH,
                           m_kuangLeftRect.width()-2*textSpaceRect,m_kuangLeftRect.height()-2*iconTMPH);
    m_textRightRect.setRect(m_kuangRightRect.x()+textSpaceRect,m_kuangRightRect.y()+iconTMPH,
                            m_kuangRightRect.width()-2*textSpaceRect,m_kuangRightRect.height()-2*iconTMPH);

    return QSize(size.width(), hei);
}

QSize QNChatMessage::getRealString(QString src) //return QSize
{
    QFontMetricsF fm(this->font());
    m_lineHeight = int(fm.lineSpacing());  //returns the distance from one base line to the next
    int nCount = src.count("\n");//出现了几次回车
    int nMaxWidth = 0;
    if(nCount == 0) {
        nMaxWidth = int(fm.width(src));
        QString value = src;
        if(nMaxWidth > m_textWidth) {
            nMaxWidth = m_textWidth;
            int size = int(m_textWidth / fm.width(" "));
            int num = int(fm.width(value) / m_textWidth);
//            int ttmp = int(num*fm.width(" "));
            num = int(( fm.width(value) ) / m_textWidth);
            nCount += num;
            QString temp = "";
            for(int i = 0; i < num; i++) {
                temp += value.mid(i*size, (i+1)*size) + "\n";
            }
            src.replace(value, temp);
        }
    } else {
        for(int i = 0; i < (nCount + 1); i++) {
            QString value = src.split("\n").at(i);
            nMaxWidth = int(fm.width(value) > nMaxWidth ? fm.width(value) : nMaxWidth);
            if(fm.width(value) > m_textWidth) {
                nMaxWidth = m_textWidth;
                int size = int(m_textWidth / fm.width(" "));
                int num = int(fm.width(value) / m_textWidth);
                num = int(((i+num)*fm.width(" ") + fm.width(value)) / m_textWidth);
                nCount += num;
                QString temp = "";
                for(int i = 0; i < num; i++) {
                    temp += value.mid(i*size, (i+1)*size) + "\n";
                }
                src.replace(value, temp);
            }
        }
    }
    return QSize(nMaxWidth+m_spaceWid, (nCount + 1) * m_lineHeight+2*m_lineHeight);
}

void QNChatMessage::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);//消锯齿，平滑传输
    painter.setPen(Qt::NoPen);  //无画笔，表示不绘制边框
    painter.setBrush(QBrush(Qt::gray));

    if(m_userType == User_Type::User_She) { //对方
        //头像
//        painter.drawRoundedRect(m_iconLeftRect,m_iconLeftRect.width(),m_iconLeftRect.height());
        painter.drawPixmap(m_iconLeftRect, m_leftPixmap);

        //框加边
        QColor col_KuangB(234, 234, 234);
        painter.setBrush(QBrush(col_KuangB));
        painter.drawRoundedRect(m_kuangLeftRect.x()-1,m_kuangLeftRect.y()-1,m_kuangLeftRect.width()+2,m_kuangLeftRect.height()+2,4,4);

        //框
        QColor col_Kuang(255,255,255);
        painter.setBrush(QBrush(col_Kuang));
        painter.drawRoundedRect(m_kuangLeftRect,4,4);

        //三角
        QPointF points[3] = {
            QPointF(m_sanjiaoLeftRect.x(), 30),
            QPointF(m_sanjiaoLeftRect.x()+m_sanjiaoLeftRect.width(), 25),
            QPointF(m_sanjiaoLeftRect.x()+m_sanjiaoLeftRect.width(), 33),
        };
        QPen pen;
        pen.setColor(col_Kuang);
        painter.setPen(pen);
        painter.drawPolygon(points, 3);

        //三角加边
        QPen penSanJiaoBian;
        penSanJiaoBian.setColor(col_KuangB);
        painter.setPen(penSanJiaoBian);
        painter.drawLine(QPointF(m_sanjiaoLeftRect.x() - 1, 30), QPointF(m_sanjiaoLeftRect.x()+m_sanjiaoLeftRect.width(), 24));
        painter.drawLine(QPointF(m_sanjiaoLeftRect.x() - 1, 30), QPointF(m_sanjiaoLeftRect.x()+m_sanjiaoLeftRect.width(), 34));

        //内容
        QPen penText;
        penText.setColor(QColor(51,51,51));
        painter.setPen(penText);
        QTextOption option(Qt::AlignLeft | Qt::AlignVCenter);//左边缘对齐且垂直方向居中
        option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);  //文本换行模式设定
        painter.setFont(this->font());
        painter.drawText(m_textLeftRect, m_msg,option);
    }  else if(m_userType == User_Type::User_Me) { // 自己
        //头像
//        painter.drawRoundedRect(m_iconRightRect,m_iconRightRect.width(),m_iconRightRect.height());
        painter.drawPixmap(m_iconRightRect, m_rightPixmap);

        //框
        QColor col_Kuang(75,164,242);
        painter.setBrush(QBrush(col_Kuang));
        painter.drawRoundedRect(m_kuangRightRect,4,4);

        //三角
        QPointF points[3] = {
            QPointF(m_sanjiaoRightRect.x()+m_sanjiaoRightRect.width(), 30),
            QPointF(m_sanjiaoRightRect.x(), 25),
            QPointF(m_sanjiaoRightRect.x(), 33),
        };
        QPen pen;
        pen.setColor(col_Kuang);
        painter.setPen(pen);
        painter.drawPolygon(points, 3);

        //三角加边
        QColor col_KuangB(75,164,242);
        QPen penSanJiaoBian;
        penSanJiaoBian.setColor(col_KuangB);
        painter.setPen(penSanJiaoBian);
        painter.drawLine(QPointF(m_sanjiaoRightRect.x() + m_sanjiaoRightRect.width() + 1, 30), QPointF(m_sanjiaoRightRect.x(), 24));
        painter.drawLine(QPointF(m_sanjiaoRightRect.x() + m_sanjiaoRightRect.width() + 1, 30), QPointF(m_sanjiaoRightRect.x() - 1, 34));

        //内容
        QPen penText;
        penText.setColor(Qt::white);
        painter.setPen(penText);
        QTextOption option(Qt::AlignLeft | Qt::AlignVCenter);
        option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        painter.setFont(this->font());
        painter.drawText(m_textRightRect,m_msg,option);
    }  else if(m_userType == User_Type::User_Time) { // 时间
        QPen penText;
        penText.setColor(QColor(153,153,153));
        painter.setPen(penText);
        QTextOption option(Qt::AlignCenter);
        option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        QFont te_font = this->font();
        te_font.setFamily("MicrosoftYaHei");
        te_font.setPointSize(10);
        painter.setFont(te_font);
        painter.drawText(this->rect(),m_curTime,option);
    }
}
