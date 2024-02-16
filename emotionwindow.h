#ifndef EMOTIONWINDOW_H
#define EMOTIONWINDOW_H

#include <QWidget>

namespace Ui {
class EmotionWindow;
}

//表情窗口类
class EmotionWindow : public QWidget
{
    Q_OBJECT

public:
    EmotionWindow(QWidget *parent = Q_NULLPTR);
    ~EmotionWindow();

private:
    void initControl();     //初始化控件(表情窗口)

private slots:
    void addEmotion(int emotionNum);    //添加表情到对话编辑框

signals:
    void signalEmotionWindowHide();                 //表情窗口隐藏信号
    void signalEmotionItemClicked(int emotionNum);  //点击表情信号

private:
    void paintEvent(QPaintEvent* event) override;   //重写鼠标点击事件

private:
    Ui::EmotionWindow *ui;
};

#endif // EMOTIONWINDOW_H
