#include "emotionlabelitem.h"

#include <QMovie>

EmotionLabelItem::EmotionLabelItem(QWidget *parent) : QClickLabel(parent)
{
    initControl();

    //表情被点击 执行发送表情点击信号(表情ID)
    connect(this, &QClickLabel::clicked, [this] {
        emit emotionClicked(m_emotionName);
    });
}

EmotionLabelItem::~EmotionLabelItem()
{
}

//设置表情名称
void EmotionLabelItem::setEmotionName(int emotionName)
{
    m_emotionName = emotionName;    //记录表情ID
    //获取表情图片路径 %1(emotionName).png
    QString imageName = QString(":/Resources/MainWindow/emotion/%1.png").arg(emotionName);

    //创建动画 apng表示动图
    m_apngMovie = new QMovie(imageName, "apng", this);
    m_apngMovie->start();   //播放动图
    m_apngMovie->stop();    //停止动图
    setMovie(m_apngMovie);  //设置当前动画
}

//初始化控件
void EmotionLabelItem::initControl()
{
    setAlignment(Qt::AlignCenter);      //设置表情居中对齐
    setObjectName("emotionLabelItem");  //设置表情对象名
    setFixedSize(32, 32);               //设置表情标签固定大小
}
