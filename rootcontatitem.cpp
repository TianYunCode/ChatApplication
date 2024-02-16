#include "rootcontatitem.h"

#include <QPainter>

RootContatItem::RootContatItem(bool hasArrow,QWidget *parent) : QLabel(parent) ,m_rotation(0) ,m_hasArrow(hasArrow)
{
    setFixedHeight(32);     //设置固定高度
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);  //设置大小策略为宽度扩展 高度固定

    //初始化属性动画
    m_animation = new QPropertyAnimation(this, "rotation"); //针对动态属性 rotation(角度)做动画
    m_animation->setDuration(50);                           //设置单次动画时长50ms
    m_animation->setEasingCurve(QEasingCurve::InQuad);      //设置动画缓和曲线类型 t^2从0加速
}

RootContatItem::~RootContatItem()
{
}

//设置显示的文本
void RootContatItem::setText(const QString & title)
{
    m_titleText = title;
    update();       //更新新的文字
}

//设置是否展开动画
void RootContatItem::setExpanded(bool expand)
{
    if (expand)
    {
        m_animation->setEndValue(90);   //设置属性动画的结束值 旋转 90°
    }
    else
    {
        m_animation->setEndValue(0);    //不展开 旋转 0°
    }

    m_animation->start();               //动画开始
}

//获取箭头角度
int RootContatItem::rotation()
{
    return m_rotation;
}

//设置箭头角度
void RootContatItem::setRotation(int rotation)
{
    m_rotation = rotation;
    update();
}

//重写绘图事件 绘制箭头
void RootContatItem::paintEvent(QPaintEvent * event)
{
    QPainter painter(this); //画家 操作箭头
    painter.setRenderHint(QPainter::TextAntialiasing, true);    //设置渲染提示

    QFont font;     //字体
    font.setPointSize(10);  //设置字体大小
    painter.setFont(font);  //给画家设置字体大小
    painter.drawText(24, 0, width() - 24, height(), Qt::AlignLeft | Qt::AlignVCenter, m_titleText); //绘制文本 对齐方式为左对齐且中心对齐
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);   //设置渲染 平滑像素转换
    painter.save();         //保存画家设置

    if (m_hasArrow) //如果存在箭头
    {
        QPixmap pixmap; //位图
        pixmap.load(":/Resources/MainWindow/arrow.png");    //加载位图

        QPixmap tmpPixmap(pixmap.size());   //临时位图
        tmpPixmap.fill(Qt::transparent);    //设置临时位图属性为透明属性

        QPainter p(&tmpPixmap); //画家 操作临时位图
        p.setRenderHint(QPainter::SmoothPixmapTransform, true); //设置渲染 平滑像素转换

        //坐标系偏移（x方向偏移，y反向偏移）
        p.translate(pixmap.width() / 2, pixmap.height() / 2);
        p.rotate(m_rotation);	//旋转坐标系（顺时针）
        p.drawPixmap(0 - pixmap.width() / 2, 0 - pixmap.height() / 2, pixmap);

        painter.drawPixmap(6, (height() - pixmap.height()) / 2, tmpPixmap);
        painter.restore();	//恢复画家设置
    }

    QLabel::paintEvent(event);
}
