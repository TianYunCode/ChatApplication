#ifndef BASICWINDOW_H
#define BASICWINDOW_H

#include <QDialog>

#include "titlebar.h"

//窗口基类
class BasicWindow : public QDialog
{
    Q_OBJECT

public:
    BasicWindow(QWidget *parent = nullptr);
    virtual ~BasicWindow();

public:
    //加载样式表
    void loadStyleSheet(const QString &sheetName);

    //获取(制作)圆头像
    QPixmap getRoundImage(const QPixmap &src, QPixmap &mask, QSize masksize = QSize(0, 0));

private:
    void initBackGroundColor();                 //初始化背景颜色

protected:
    void paintEvent(QPaintEvent*);				//绘制事件
    void mousePressEvent(QMouseEvent* event);	//鼠标按下事件
    void mouseMoveEvent(QMouseEvent* event);	//鼠标移动事件
    void mouseReleaseEvent(QMouseEvent*);		//鼠标松开事件

protected:
    void initTitleBar(ButtonType buttontype = MIN_BUTTON);                  //初始化标题栏
    void setTitleBarTitle(const QString& title, const QString& icon = "");  //设置标题栏标题

public slots:
    void onShowClose(bool);     //关闭窗口
    void onShowMin(bool);       //最小化窗口
    void onShowHide(bool);      //隐藏窗口
    void onShowNormal(bool);    //显示正常窗口
    void onShowQuit(bool);      //退出程序
    void onSignalSkinChanged(const QColor &color);  //皮肤改变

    void onButtonMinClicked();      //最小化按钮被点击 槽函数
    void onButtonRestoreClicked();  //最大化还原按钮被点击 槽函数
    void onButtonMaxClicked();      //最大化按钮被点击 槽函数
    void onButtonCloseClicked();    //关闭按钮被点击 槽函数

protected:
    QPoint m_mousePoint;        //鼠标位置
    bool m_mousePressed;        //鼠标按下标志
    QColor m_colorBackGround;	//背景色
    QString m_styleName;        //样式文件名称
    TitleBar* _titleBar;        //标题栏
};

#endif // BASICWINDOW_H
