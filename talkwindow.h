#ifndef TALKWINDOW_H
#define TALKWINDOW_H

#include <QWidget>
#include <QTreeWidgetItem>

#include "ui_talkwindow.h"
#include "talkwindowshell.h"

namespace Ui {
class TalkWindow;
}

//聊天窗口类(主窗口右边)
class TalkWindow : public QWidget
{
    Q_OBJECT

public:
    TalkWindow(QWidget *parent, const QString& uid);
    ~TalkWindow();

public:
    void addEmotionImage(int emotionNum);       //把选中的表情添加到当前聊天窗口消息编辑框中
    void setWindowName(const QString& name);    //设置当前聊天窗口的名称
    QString getTalkId();        //获取当前聊天窗口ID

private slots:
    void onFileOpenBtnClicked(bool);    //文件打开按钮被点击 槽函数
    void onSendBtnClicked(bool);        //点击发送按钮 槽函数
    void onItemDoubleClicked(QTreeWidgetItem* item, int column);     //双击聊天窗口右边群好友列表 槽函数

private:
    void initControl();         //初始化控件(聊天窗口)
    void initGroupTalkStatus(); //初始化群聊状态
    int  getCompDepID();        //获取公司ID(群聊号)
    void initTalkWindow();      //初始化群聊
    void initPtoPTalk();        //初始化单聊
    void addPeopInfo(QTreeWidgetItem* pRootGroupItem,int employeeID);  //添加群员信息

private:
    Ui::TalkWindow *ui;
    QString m_talkId;       //聊天窗口ID
    bool m_isGroupTalk;     //是否为群聊 群聊状态
    QMap<QTreeWidgetItem*, QString> m_groupPeopleMap;//所有分组联系人姓名
    friend class TalkWindowShell;
};

#endif // TALKWINDOW_H
