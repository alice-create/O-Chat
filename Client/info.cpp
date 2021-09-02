#include "info.h"
#include "ui_info.h"
#include "login.h"

info::info(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::info)
{
    ui->setupUi(this);
}

info::info(QTcpSocket *socket, QString no, QString id, QString b_id, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::info)
{
    ui -> setupUi(this);
    sock = socket;

    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground,true);

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(20);
    shadow->setOffset(0,0);
    shadow->setColor(QColor(192,192,192));
    ui->frame->setGraphicsEffect(shadow);

    infomation = ui -> label;
    confirm = ui -> pushButton;
    No = no;
    ID = id;
    B_ID = b_id;
    initContent();
}


info::~info()
{
    delete ui;
}

void info::mousePressEvent(QMouseEvent *event)
{
    mMoveing = true;
    mMovePosition = event->globalPos() - pos();
    QWidget::mousePressEvent(event);
}

void info::mouseMoveEvent(QMouseEvent *event)
{
    if(mMoveing & (event->buttons() & Qt::LeftButton)
            &(event->globalPos() - mMovePosition).manhattanLength()
            > QApplication::startDragDistance()) {
        move(event->globalPos() - mMovePosition);
        mMovePosition = event->globalPos() - pos();
    }
    QWidget::mouseMoveEvent(event);
}

void info::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    mMoveing = false;
}

void info::initContent() {
    if(No == "01") {
        if(ID == "0") {
            QFont reset_font;
            reset_font.setFamily("AvenirNext LT Pro Bold");
            reset_font.setPointSize(12);
            infomation->setFont(reset_font);
            QString otp = "Sign Up Successfully\nYour ID: " + B_ID + "\nKeep in Mind";
            infomation -> setText(otp);
        } else {
            if(B_ID == "0") {
                QString otp = "User Exists";
                infomation -> setText(otp);
            }
            if(B_ID == "1") {
                QString otp = "DataType Error";
                infomation -> setText(otp);
            }
            if(B_ID == "2") {
                QString otp = "| and # are invalid";
                infomation -> setText(otp);
            }
       }
    }
    if(No == "02") {
        if(B_ID == "0") {
            QString otp = "User Not Exists";
            infomation -> setText(otp);
        }
        if(B_ID == "1") {
            QString otp = "Wrong PassWord";
            infomation -> setText(otp);
        }
        if(B_ID == "2") {
            QString otp = "DataType Error";
            infomation -> setText(otp);
        }
        if(B_ID == "3") {
            QString otp = "| and # are invalid";
            infomation -> setText(otp);
        }
    }
    if(No == "04") {
        if(ID == "0") {
            QFont reset_font;
            reset_font.setFamily("AvenirNext LT Pro Bold");
            reset_font.setPointSize(12);
            infomation->setFont(reset_font);
            QString otp = "Your Request Send Successfully";
            infomation -> setText(otp);
        } else {
            if(B_ID == "0") {
                QFont reset_font;
                reset_font.setFamily("AvenirNext LT Pro Bold");
                reset_font.setPointSize(13);
                infomation->setFont(reset_font);
                QString otp = "You are already firends";
                infomation -> setText(otp);
            }
            if(B_ID == "1") {
                QString otp = "User Not Exists";
                infomation -> setText(otp);
            }
            if(B_ID == "2") {
                QString otp = "DataType Error";
                infomation -> setText(otp);
            }
       }
    }
    if(No == "05") {
        QString otp = B_ID + " reject the apply!";
        infomation -> setText(otp);
    }
    if(No == "07") {
        if(ID == "1") {
//            QFont reset_font;
//            reset_font.setFamily("AvenirNext LT Pro Bold");
//            reset_font.setPointSize(12);
//            infomation->setFont(reset_font);
            QString otp = "Unknown Error";
            infomation -> setText(otp);
        }
    }
    if(No == "11") {
        if(ID == "0") {
            QFont reset_font;
            reset_font.setFamily("AvenirNext LT Pro Bold");
            reset_font.setPointSize(14);
            infomation->setFont(reset_font);
            QString otp = "Note Successfully!";
            infomation -> setText(otp);
        } else {
            QString otp = "Note Fail!";
            infomation -> setText(otp);
        }
    }
    if(No == "12") {
        if(ID == "0") {
            QFont reset_font;
            reset_font.setFamily("AvenirNext LT Pro Bold");
            reset_font.setPointSize(14);
            infomation->setFont(reset_font);
            QString otp = "Rename Successfully!";
            infomation -> setText(otp);
        } else {
            if(B_ID == "0") {
                QFont reset_font;
                reset_font.setFamily("AvenirNext LT Pro Bold");
                reset_font.setPointSize(12);
                infomation->setFont(reset_font);
                QString otp = "Rename Fail!\nUsername Exists";
                infomation -> setText(otp);
            }
            if(B_ID == "1") {
                QFont reset_font;
                reset_font.setFamily("AvenirNext LT Pro Bold");
                reset_font.setPointSize(12);
                infomation->setFont(reset_font);
                QString otp = "Rename Fail!\nWrong PassWord";
                infomation -> setText(otp);
            }
            if(B_ID == "2") {
                QFont reset_font;
                reset_font.setFamily("AvenirNext LT Pro Bold");
                reset_font.setPointSize(12);
                infomation->setFont(reset_font);
                QString otp = "Rename Fail!\nDataType Error";
                infomation -> setText(otp);
            }
       }
    }
    if(No == "#13") {
        if(ID == "0") {
            QFont reset_font;
            reset_font.setFamily("AvenirNext LT Pro Bold");
            reset_font.setPointSize(14);
            infomation->setFont(reset_font);
            QString otp = "Config Successfully";
            infomation -> setText(otp);
        } else {
            if(B_ID == "0") {
                QString otp = "Wrong Password";
                infomation -> setText(otp);
            }
            if(B_ID == "1") {
                QString otp = "DataType Error";
                infomation -> setText(otp);
            }
            if(B_ID == "2") {
                QFont reset_font;
                reset_font.setFamily("AvenirNext LT Pro Bold");
                reset_font.setPointSize(14);
                infomation->setFont(reset_font);
                QString otp = "NEW PassWord Unmatch";
                infomation -> setText(otp);
            }
            if(B_ID == "3") {
                QString otp = "| and # are invalid";
                infomation -> setText(otp);
            }
        }
    }
    if(No == "#14") {
        if(ID == "0") {
            QFont reset_font;
            reset_font.setFamily("AvenirNext LT Pro Bold");
            reset_font.setPointSize(14);
            infomation->setFont(reset_font);
            QString otp = "Delete Successfully";
            infomation -> setText(otp);
        } else {
            QString otp = "Delete Fails";
            infomation -> setText(otp);
        }
    }
    if(No == "###02") {
        QString otp = "User " + B_ID + " OnLine";
        infomation -> setText(otp);
    }
    if(No == "###03") {
        QString otp = "User " + B_ID + " OffLine";
        infomation -> setText(otp);
    }
}

void info::on_pushButton_clicked()
{
    if(No == "05")
    {
        QString sendData= "#08|" + ID + "|" + B_ID;
        sock->write(sendData.toUtf8());
        this -> hide();
    }
    if(No == "#13") {
        if(ID == "0") {
            login *loginwindow = new login(sock);
            loginwindow -> show();
            this -> hide();
        } else {
            this -> hide();
        }
    }
    else
    {
        this -> hide();
    }
}
