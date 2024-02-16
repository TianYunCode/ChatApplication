#ifndef TALKWINDOWITEM_H
#define TALKWINDOWITEM_H

#include <QWidget>

namespace Ui {
class TalkWindowItem;
}

//聊天选项卡类(主聊天窗口左边)
class TalkWindowItem : public QWidget
{
    Q_OBJECT

public:
    TalkWindowItem(QWidget *parent = Q_NULLPTR);
    ~TalkWindowItem();

    void setHeadPixmap(const QPixmap& pixmap);      //设置聊天选项卡头像
    void setMsgLabelContent(const QString& msg);    //设置聊天选项卡内容(聊天人名称)
    QString getMsgLabelText();                      //获取聊天选项卡内容(聊天人名称)

private:
    void initControl();                             //初始化控件(聊天选项卡)

signals:
    void signalCloseClicked();                      //关闭聊天选项卡信号

private:
    void enterEvent(QEvent* event);                 //重写进入事件
    void leaveEvent(QEvent* event);                 //重写离开事件
    void resizeEvent(QResizeEvent* event);          //重写调整大小事件

private:
    Ui::TalkWindowItem *ui;
};

#endif // TALKWINDOWITEM_H
