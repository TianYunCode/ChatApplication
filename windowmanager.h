#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QObject>

#include "talkwindowshell.h"

//聊天窗口管理类
class WindowManager : public QObject
{
    Q_OBJECT

public:
    WindowManager();
    ~WindowManager();

public:
    QWidget* findWindowName(const QString& qsWindowName);               //查找聊天窗口名称
    void deleteWindowName(const QString& qsWindowName);                 //删除聊天窗口名称
    void addWindowName(const QString& qsWindowName, QWidget* qWidget);  //添加聊天窗口名称(不新建)

    static WindowManager* getInstance();        //单例模式 返回实例
    TalkWindowShell* getTalkWindowShell();      //返回主聊天窗口成员变量
    QString getCreatingTalkId();                //获取正在创建的聊天窗口ID
    void addNewTalkWindow(const QString&uid);   //添加新聊天窗口(会新建)

private:
    TalkWindowShell* m_talkwindowshell;         //主聊天窗口
    QMap<QString, QWidget*> m_windowMap;        //聊天选项卡跟对应聊天窗口关联
    QString m_strCreatingTalkId = "";           //正在创建的聊天窗口ID（QQ号）
};

#endif // WINDOWMANAGER_H
