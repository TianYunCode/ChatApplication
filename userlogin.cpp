#include "userlogin.h"
#include "ui_userlogin.h"
#include "ccmainwindow.h"

#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>

QString gLoginEmployeeID;   //登录者QQ好(员工号)

UserLogin::UserLogin(QWidget *parent) : BasicWindow(parent), ui(new Ui::UserLogin)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);     //设置属性为关闭时退出
    initTitleBar();                         //初始化标题栏
    setTitleBarTitle("", ":/Resources/MainWindow/qqlogoclassic.png");   //设置标题栏标题(图标)
    loadStyleSheet("UserLogin");            //加载样式表(登录界面)
    initControl();                          //初始化控件(登录界面)
}

UserLogin::~UserLogin()
{
    delete ui;
}

//初始化控件(登录界面)
void UserLogin::initControl()
{
    QLabel *headlabel = new QLabel(this);   //标题栏标签(图标)
    headlabel->setFixedSize(68, 68);        //设置标题栏标签(图标)固定大小

    QPixmap pix(":/Resources/MainWindow/head_mask.png");    //获取空白圆头像
    //获取(制作)圆头像 并设置到标题栏标签(图标)
    headlabel->setPixmap(getRoundImage(QPixmap(":/Resources/MainWindow/app/logo.ico"), pix, headlabel->size()));
    //把头像移动到界面中间
    headlabel->move(width() / 2 - 34, ui->titleWidget->height() - 34);
    //点击登录按钮 执行 onLoginBtnClicked()函数
    connect(ui->loginBtn, &QPushButton::clicked, this, &UserLogin::onLoginBtnClicked);

    //连接数据库
    if (!connectMySql())
    {
        QMessageBox::information(NULL, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("连接数据库失败！"));
        close();
    }

    ui->editUserAccount->setText("10000");
    ui->editPassword->setText("142857");
}

//连接数据库
bool UserLogin::connectMySql()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName("chatapplicationdatabase");	//数据库名称
    db.setHostName("localhost");    //主机名
    db.setUserName("root");         //用户名
    db.setPassword("wtymm142857");	//密码
    db.setPort(3306);               //端口

    if (db.open())  //打开数据库
    {
        return true;        //成功 返回 true
    }
    else
    {
        return false;       //失败 返回 false
    }
}

//验证账号密码
bool UserLogin::veryfyAccountCode(bool &isAccountLogin, QString &strAccount)
{
    //拿到输入的账号
    QString strAccountInput = ui->editUserAccount->text();
    //拿到输入的密码
    QString strCodeInput = ui->editPassword->text();

    //输入员工号(QQ号登陆)
    QString strSqlCode = QString("SELECT code FROM tab_accounts WHERE employeeID = %1").arg(strAccountInput);   //创建 SQL 语句
    QSqlQuery queryEmployeeID(strSqlCode);   //查询
    queryEmployeeID.exec();     //执行 SQL 语句

    if (queryEmployeeID.first())//指向结果集第一条
    {
        //数据库中qq号对应的密码
        QString strCode = queryEmployeeID.value(0).toString();

        //判断数据库中拿到的密码是否跟输入的密码一致
        if (strCode == strCodeInput)
        {
            gLoginEmployeeID = strAccountInput;

            isAccountLogin = false;
            strAccount = strAccountInput;

            return true;     //一致 返回 true
        }
        else
        {
            return false;   //不一致 返回 false
        }
    }

    //账号登录
    strSqlCode = QString("SELECT code,employeeID FROM tab_accounts WHERE account = '%1'").arg(strAccountInput);  //创建 SQL 语句
    QSqlQuery queryAccount(strSqlCode);     //查询
    queryAccount.exec();        //执行 SQL 语句

    if (queryAccount.first())   //指向结果集第一条
    {
        //数据库中账号对应的密码
        QString strCode = queryAccount.value(0).toString();

         //判断数据库中拿到的密码是否跟输入的密码一致
        if (strCode == strCodeInput)
        {
            gLoginEmployeeID = queryAccount.value(1).toString();

            strAccount = strAccountInput;
            isAccountLogin = true;

            return true;     //一致 返回 true
        }
        else
        {
            return false;   //不一致 返回 false
        }
    }

    return false;
}

//登录按钮点击 槽函数
void UserLogin::onLoginBtnClicked()
{
    bool isAccountLogin;    //是否是账号登录
    QString strAccount;     //账号或QQ号

    if (!veryfyAccountCode(isAccountLogin,strAccount))
    {
        QMessageBox::information(NULL, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("您输入的账号或密码有误，请重新输入！"));
        //ui->editPassword->setText("");
        //ui->editPassword->setText("");
        return;
    }

    //更新登录状态为登录
    QString strSqlStatus = QString("UPDATE tab_employees SET online_status = 2 WHERE employeeID = %1").arg(gLoginEmployeeID);
    QSqlQuery sqlStatus(strSqlStatus);
    sqlStatus.exec();

    //关闭登录窗口
    close();
    //创建主窗口
    CCMainWindow* mainwindow = new CCMainWindow(strAccount,isAccountLogin);
    //显示主窗口
    mainwindow->show();
}
