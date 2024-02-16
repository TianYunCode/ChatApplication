#include "skinwindow.h"
#include "ui_skinwindow.h"
#include "qclicklabel.h"
#include "notifymanager.h"

SkinWindow::SkinWindow(QWidget *parent) : BasicWindow(parent), ui(new Ui::SkinWindow)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::Tool);   //设置窗体风格为工具风格 任务栏就不会显示该窗口
    loadStyleSheet("SkinWindow");               //加载样式表(皮肤窗口)
    setAttribute(Qt::WA_DeleteOnClose);         //设置窗口属性为关闭即退出
    initControl();                              //初始化控件(皮肤窗口)
}

SkinWindow::~SkinWindow()
{
    delete ui;
}

//初始化控件(皮肤窗口)
void SkinWindow::initControl()
{
    //皮肤颜色标签列表
    QList<QColor> colorList =
    {
        QColor(22,154,218), QColor(40,138,221), QColor(49,166,107), QColor(218,67,68),
        QColor(177,99,158), QColor(107,81,92),  QColor(89,92,160),  QColor(21,156,199),
        QColor(79,169,172), QColor(155,183,154),QColor(128,77,77),  QColor(240,188,189)
    };

    //循环插入皮肤颜色标签
    for (int row = 0; row < 3; row++)
    {
        for (int column = 0; column < 4; column++)
        {
            //创建标签
            QClickLabel *label = new QClickLabel(this);
            //设置光标为手指形状
            label->setCursor(Qt::PointingHandCursor);

            //c++11 lambda表达式   点击颜色标签 执行 lambda表达式
            connect(label, &QClickLabel::clicked, [row, column, colorList]()
            {
                //通知其他窗口改变皮肤
                NotifyManager::getInstance()->notifyOtherWindowChangeSkin(colorList.at(row * 4 + column));
            });

            //设置皮肤标签固定大小
            label->setFixedSize(84, 84);

            //设置皮肤颜色标签为对应的颜色
            QPalette palette;   //创建调色板
            //设置调色板颜色
            palette.setColor(QPalette::Background, colorList.at(row * 4 + column));
            //设置皮肤标签为自动填充背景
            label->setAutoFillBackground(true);
            //设置皮肤标签颜色为调色板颜色
            label->setPalette(palette);

            //把皮肤标签插入网格布局管理器
            ui->gridLayout->addWidget(label, row, column);
        }
    }

    //点击最小化按钮 执行 onShowMin(bool) 函数进行最小化
    connect(ui->sysmin, SIGNAL(clicked(bool)), this, SLOT(onShowMin(bool)));
    //点击关闭按钮 执行 onShowClose() 函数进行关闭窗口
    connect(ui->sysclose, SIGNAL(clicked()), this, SLOT(onShowClose()));
}

//关闭皮肤窗口 槽函数
void SkinWindow::onShowClose()
{
    close();
    emit signalCloseSkinWindow();
}
