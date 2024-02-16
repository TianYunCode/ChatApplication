#include "userlogin.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);     //设置最后一个窗口退出不会关闭程序

    UserLogin *userlogin = new UserLogin(); //创建登录界面
    userlogin->show();                      //显示登录界面

    return a.exec();
}
