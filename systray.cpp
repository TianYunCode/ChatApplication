#include "systray.h"
#include "custommenu.h"

SysTray::SysTray(QWidget *parent) : m_parent(parent) ,QSystemTrayIcon(parent)
{
    initSystemTray();
    show();
}

SysTray::~SysTray()
{
}

void SysTray::initSystemTray()
{
    //设置系统托盘提示
    setToolTip(QStringLiteral("ChatApplication"));
    //设置系统托盘图标
    setIcon(QIcon(":/Resources/MainWindow/app/logo.ico"));
    connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(onIconActivated(QSystemTrayIcon::ActivationReason)));
}

//添加系统托盘菜单
void SysTray::addSystrayMenu()
{
    //创建自定义系统托盘菜单
    CustomMenu *customMenu = new CustomMenu(m_parent);
    //添加菜单
    customMenu->addCustomMenu("onShow", ":/Resources/MainWindow/app/logo.ico", QStringLiteral("显示"));
    customMenu->addCustomMenu("onQuit", ":/Resources/MainWindow/app/page_close_btn_hover.png", QStringLiteral("退出"));

    //如果系统托盘菜单“显示”被点击 执行 onShowNormal(bool)函数显示主窗口
    connect(customMenu->getAction("onShow"), SIGNAL(triggered(bool)), m_parent, SLOT(onShowNormal(bool)));
    //如果系统托盘菜单“退出”被点击 执行 onShowQuit(bool)函数退出程序
    connect(customMenu->getAction("onQuit"), SIGNAL(triggered(bool)), m_parent, SLOT(onShowQuit(bool)));

    customMenu->exec(QCursor::pos());   //菜单进入事件循环

    delete customMenu;
    customMenu = nullptr;
}

//点击系统托盘 槽函数
void SysTray::onIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    //如果系统托盘触发原因是 单击系统托盘
    if (reason == QSystemTrayIcon::Trigger)
    {
        m_parent->show();           //显示主窗口
        m_parent->activateWindow(); //激活主窗口
    }   //如果系统托盘触发原因是 系统托盘的上下文菜单请求(右键)
    else if (reason == QSystemTrayIcon::Context)
    {
        addSystrayMenu();           //添加(显示)系统托盘菜单
    }
}
