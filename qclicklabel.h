#ifndef QCLICKLABEL_H
#define QCLICKLABEL_H

#include <QLabel>

//自定义标签类
class QClickLabel : public QLabel
{
    Q_OBJECT

public:
    QClickLabel(QWidget *parent);
    ~QClickLabel();

protected:
    void mousePressEvent(QMouseEvent* event);   //鼠标按下事件

signals:
    void clicked();     //点击了颜色标签信号
};

#endif // QCLICKLABEL_H
