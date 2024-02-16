#include "qclicklabel.h"
#include <QMouseEvent>

QClickLabel::QClickLabel(QWidget *parent) : QLabel(parent)
{
}

QClickLabel::~QClickLabel()
{
}

//鼠标按下事件
void QClickLabel::mousePressEvent(QMouseEvent * event)
{
    //如果按下的按钮是左键
    if (event->button() == Qt::LeftButton)
    {
        emit clicked();     //发送点击了颜色标签信号
    }
}
