#ifndef EMOTIONLABELITEM_H
#define EMOTIONLABELITEM_H

#include "qclicklabel.h"

class QMovie;

class EmotionLabelItem : public QClickLabel
{
    Q_OBJECT

public:
    EmotionLabelItem(QWidget *parent);
    ~EmotionLabelItem();

    void setEmotionName(int emotionName);   //设置表情名称

private:
    void initControl();     //初始化控件

signals:
    void emotionClicked(int emotionNum);    //点击表情信号

private:
    int m_emotionName;      //设置表情名称ID
    QMovie* m_apngMovie;    //表情动画
};

#endif // EMOTIONLABELITEM_H
