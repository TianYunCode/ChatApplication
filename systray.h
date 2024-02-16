#ifndef SYSTRAY_H
#define SYSTRAY_H

#include <QSystemTrayIcon>
#include <QWidget>

//系统托盘类
class SysTray : public QSystemTrayIcon
{
    Q_OBJECT

public:
    SysTray(QWidget *parent);
    ~SysTray();

public slots:
    //点击系统托盘 槽函数
    void onIconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    void initSystemTray();  //初始化系统托盘
    void addSystrayMenu();  //添加系统托盘菜单
private:
    QWidget* m_parent;      //父部件指针
};

#endif // SYSTRAY_H
