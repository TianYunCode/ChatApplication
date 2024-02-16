#ifndef SKINWINDOW_H
#define SKINWINDOW_H

#include "basicwindow.h"

namespace Ui {
class SkinWindow;
}

class SkinWindow : public BasicWindow
{
    Q_OBJECT

public:
    SkinWindow(QWidget *parent = Q_NULLPTR);
    ~SkinWindow();

public:
    void initControl();     //初始化控件(皮肤窗口)

signals:
    void signalCloseSkinWindow();   //关闭了皮肤窗口信号

public slots:
    void onShowClose();     //关闭皮肤窗口 槽函数

private:
    Ui::SkinWindow *ui;
};

#endif // SKINWINDOW_H
