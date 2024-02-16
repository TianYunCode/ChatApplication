#ifndef CCMAINWINDOW_H
#define CCMAINWINDOW_H

#include "basicwindow.h"
#include "skinwindow.h"
#include "systray.h"

#include <qmap.h>

QT_BEGIN_NAMESPACE
namespace Ui { class CCMainWindow; }
QT_END_NAMESPACE

class QTreeWidgetItem;

class CCMainWindow : public BasicWindow
{
    Q_OBJECT

public:
    CCMainWindow(QString account,bool isAccountLogin, QWidget *parent = Q_NULLPTR);
    ~CCMainWindow();

public:
    void setUserName(const QString& username);          //设置用户名
    void setLevelPixmap(int level);                     //设置等级
    void setHeadPixmap(const QString& headPath);        //设置头像
    void setStatusMenuIcon(const QString& statusPath);  //设置状态
    //添加应用部件(app图片路径，app部件对象名)
    QWidget* addOtherAppExtension(const QString& appPath, const QString& appName);
    void initContactTree();                             //初始化联系人树(列表)

private:
    void initTimer();               //初始化计时器 改变等级
    void initControl();             //初始化控件(主窗口)
    QString getHeadPicturePath();   //获取头像路径(从数据库里获取)
    void updateSeachStyle();        //更新搜索样式
    void addCompanyDeps(QTreeWidgetItem* pRootGroupItem, int DepID);    //添加公司部门

private:
    void resizeEvent(QResizeEvent* event);          //重写重置大小事件
    bool eventFilter(QObject* obj, QEvent* event);  //重写事件过滤器
    void mousePressEvent(QMouseEvent* event);       //重写鼠标按下事件

private slots:
    void onItemClicked(QTreeWidgetItem* item, int column);          //树被单击 槽函数
    void onItemExpanded(QTreeWidgetItem* item);         //展开树
    void onItemCollapsed(QTreeWidgetItem* item);        //收缩树
    void onItemDoubleClicked(QTreeWidgetItem* item, int column);    //树被双击 槽函数
    void onAppIconClicked();        //小 app 部件点击 槽函数
    void onCloseSkinWindow();       //皮肤窗口关闭执行的槽函数 把 isCloseSkinWindow 设置 false

private:
    Ui::CCMainWindow *ui;
    bool m_isAccountLogin;      //是否是账号登录标志
    QString m_account;          //登录的账号或QQ号
    bool isCloseSkinWindow;     //皮肤窗口是否关闭标志
    SkinWindow *m_skinWindow;   //皮肤窗口
    SysTray *systray;           //系统托盘
};
#endif // CCMAINWINDOW_H
