#include "CustomMenu.h"
#include "CommonUtils.h"

CustomMenu::CustomMenu(QWidget *parent) : QMenu(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);     //系统托盘菜单设置透明属性
    CommonUtils::loadStyleSheet(this, "Menu");      //加载样式表(系统托盘菜单)
}

CustomMenu::~CustomMenu()
{
}

//添加(创建)系统托盘菜单
void CustomMenu::addCustomMenu(const QString & text, const QString & icon, const QString & name)
{
    QAction* pAction = addAction(QIcon(icon), name);    //添加菜单选项
    m_menuActionMap.insert(text, pAction);              //将菜单选项插入菜单映射变量
}

//获取菜单选项
QAction * CustomMenu::getAction(const QString & text)
{
    return m_menuActionMap[text];
}
