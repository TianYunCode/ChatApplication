#ifndef CUSTOMMENU_H
#define CUSTOMMENU_H

#include <QMenu>
#include <QMap>

//自定义系统托盘菜单类
class CustomMenu : public QMenu
{
    Q_OBJECT

public:
    CustomMenu(QWidget *parent = nullptr);
    ~CustomMenu();

public:
    //添加(创建)系统托盘菜单
    void addCustomMenu(const QString& text, const QString& icon, const QString& name);
    //获取菜单选项
    QAction* getAction(const QString& text);
private:
    QMap<QString, QAction*> m_menuActionMap;    //菜单映射变量 保存菜单选项(菜单键值，菜单)
};

#endif // CUSTOMMENU_H
