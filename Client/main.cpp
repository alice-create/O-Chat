#include "mainchatwin.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    //Qt提供了支持高Dpi的接口，可以让界面随着dpi的变化进行缩放,该设置应该放在主函数的最前面，放在QGuiApplication构造之前
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    MainChatWin w;
    w.show();
    return a.exec();
    //程序进入消息循环，等待可能输入进行响应。
    //这里main()把控制权转交给Qt，Qt完成事件处理工作，当应用程序退出的时候exec()的值就会返回。
    //在exec()中，Qt接受并处理用户和系统的事件并且把它们传递给适当的窗口部件。
}
