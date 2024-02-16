#include "emotionwindow.h"
#include "ui_emotionwindow.h"
#include "commonutils.h"
#include "emotionlabelitem.h"

#include <QStyleOption>
#include <QPainter>

const int emotionColumn = 14;   //表情列数
const int emotionRow = 12;      //表情行数

EmotionWindow::EmotionWindow(QWidget *parent) : QWidget(parent), ui(new Ui::EmotionWindow)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow);    //设置窗口风格为无边框并且是子窗口
    setAttribute(Qt::WA_TranslucentBackground);                 //设置透明属性
    setAttribute(Qt::WA_DeleteOnClose);                         //设置资源回收 当应用程序最后一个窗体关闭 则回收表情窗口

    ui->setupUi(this);

    initControl();  //初始化控件(表情窗口)
}

EmotionWindow::~EmotionWindow()
{
    delete ui;
}

//初始化控件(表情窗口)
void EmotionWindow::initControl()
{
    CommonUtils::loadStyleSheet(this, "EmotionWindow");     //加载样式表(表情窗口)

    //添加表情
    for (int row = 0; row < emotionRow; row++)
    {
        for (int column = 0; column < emotionColumn; column++)
        {
            EmotionLabelItem* label = new EmotionLabelItem(this);   //创建表情标签
            label->setEmotionName(row * emotionColumn + column);    //设置表情名称

            //当表情标签被点击 执行 addEmotion()将表情添加到对话编辑框
            connect(label, &EmotionLabelItem::emotionClicked, this, &EmotionWindow::addEmotion);
            //把标签添加到网格布局管理器
            ui->gridLayout->addWidget(label, row, column);
        }
    }
}

//添加表情到对话编辑框
void EmotionWindow::addEmotion(int emotionNum)
{
    hide(); //先关闭表情窗口
    emit signalEmotionWindowHide();     //发送表情窗口关闭信号
    emit signalEmotionItemClicked(emotionNum);  //发送表情点击信号(表情ID)
}

 //重写鼠标点击事件
void EmotionWindow::paintEvent(QPaintEvent * event)
{
    QStyleOption opt;   //风格
    opt.init(this);     //初始化为当前窗体风格

    QPainter painter(this); //画家

    //重绘当前风格
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    __super::paintEvent(event);
}
