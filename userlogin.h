#ifndef USERLOGIN_H
#define USERLOGIN_H

#include "basicwindow.h"

namespace Ui {
class UserLogin;
}

//登录窗口类
class UserLogin : public BasicWindow
{
    Q_OBJECT

public:
    UserLogin(QWidget *parent = Q_NULLPTR);
    ~UserLogin();

private slots:
    void onLoginBtnClicked();       //登录按钮点击 槽函数

private:
    void initControl();             //初始化控件（登录界面）
    bool connectMySql();            //连接数据库
    bool veryfyAccountCode(bool &isAccountLogin,QString &strAccount);   //验证账号密码

private:
    Ui::UserLogin *ui;
};

#endif // USERLOGIN_H
