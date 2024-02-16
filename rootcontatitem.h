#ifndef ROOTCONTATITEM_H
#define ROOTCONTATITEM_H

#include <QLabel>
#include <QPropertyAnimation>

//联系人树展开收缩箭头类
class RootContatItem : public QLabel
{
    Q_OBJECT

    //箭头角度 动态属性
    Q_PROPERTY(int rotation READ rotation WRITE setRotation)
public:
    RootContatItem(bool hasArrow = true, QWidget *parent = nullptr);
    ~RootContatItem();

public:
    void setText(const QString& title); //设置显示的文本
    void setExpanded(bool expand);      //设置是否展开动画

private:
    int rotation();                     //获取箭头角度
    void setRotation(int rotation);     //设置箭头角度

protected:
    void paintEvent(QPaintEvent* event);//重写绘图事件 绘制箭头

private:
    QPropertyAnimation *m_animation;    //旋转动画
    QString m_titleText;                //显示的文本
    int m_rotation;                     //箭头的角度
    bool m_hasArrow;                    //是否有箭头
};

#endif // ROOTCONTATITEM_H
