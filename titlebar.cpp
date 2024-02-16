#include "titlebar.h"

#include <QHBoxLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QFile>

#define BUTTON_HEIGHT 27	//按钮高度
#define BUTTON_WIDTH  27	//按钮宽度
#define TITLE_HEIGHT  27	//标题栏高度

TitleBar::TitleBar(QWidget *parent) : QWidget(parent) ,m_isPressed(false) ,m_buttonType(MIN_MAX_BUTTON)
{
    initControl();          //初始化控件
    initConnections();      //初始化信号与槽的连接
    loadStyleSheet("Title");//加载标题栏样式表
}

TitleBar::~TitleBar()
{
}

//初始化控件
void TitleBar::initControl()
{
    //创建控件
    m_pIcon = new QLabel(this);                 //创建标题栏图标
    m_pTitleContent = new QLabel(this);         //创建标题栏内容

    m_pButtonMin = new QPushButton(this);       //创建最小化按钮
    m_pButtonRestore = new QPushButton(this);   //创建最大化还原按钮
    m_pButtonMax = new QPushButton(this);       //创建最大化按钮
    m_pButtonClose = new QPushButton(this);     //创建关闭按钮

    //按钮设置固定大小
    m_pButtonMin->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));         //设置最小化按钮固定大小为(27,27)
    m_pButtonRestore->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));     //设置最大化还原按钮固定大小为(27,27)
    m_pButtonMax->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));         //设置最大化按钮固定大小为(27,27)
    m_pButtonClose->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));       //设置关闭按钮固定大小为(27,27)

    //设置对象名
    m_pTitleContent->setObjectName("TitleContent");     //设置标题内容的对象名
    m_pButtonMin->setObjectName("ButtonMin");           //设置最小化按钮的对象名
    m_pButtonRestore->setObjectName("ButtonRestore");   //设置最大化还原的对象名
    m_pButtonMax->setObjectName("ButtonMax");           //设置最大化按钮的对象名
    m_pButtonClose->setObjectName("ButtonClose");       //设置关闭按钮的对象名

    //设置布局
    QHBoxLayout *mylayout = new QHBoxLayout(this);      //创建水平布局管理器
    mylayout->addWidget(m_pIcon);                       //添加标题栏图标部件到水平布局管理器
    mylayout->addWidget(m_pTitleContent);               //添加标题栏内容部件到水平布局管理器

    mylayout->addWidget(m_pButtonMin);                  //添加最小化按钮部件到水平布局管理器
    mylayout->addWidget(m_pButtonRestore);              //添加最大化还原按钮部件到水平布局管理器
    mylayout->addWidget(m_pButtonMax);                  //添加最大化按钮部件到水平布局管理器
    mylayout->addWidget(m_pButtonClose);                //添加关闭按钮部件到水平布局管理器

    mylayout->setContentsMargins(5, 0, 0, 0);           //设置布局的间隙
    mylayout->setSpacing(0);                            //设置布局里部件间的间隙

    //设置标题栏内容水平策略为可扩展 垂直策略为固定
    m_pTitleContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    //设置标题栏固定高度为27
    setFixedHeight(TITLE_HEIGHT);
    //设置窗体为无边框模式
    setWindowFlags(Qt::FramelessWindowHint);
}

//信号与槽的连接
void TitleBar::initConnections()
{
    //最小化按钮点击 执行onButtonMinClicked()函数
    connect(m_pButtonMin, SIGNAL(clicked()), this, SLOT(onButtonMinClicked()));
    //最大化还原按钮点击 执行onButtonRestoreClicked()函数
    connect(m_pButtonRestore, SIGNAL(clicked()), this, SLOT(onButtonRestoreClicked()));
    //最大化按钮点击 执行onButtonMaxClicked()函数
    connect(m_pButtonMax, SIGNAL(clicked()), this, SLOT(onButtonMaxClicked()));
    //关闭按钮点击 执行onButtonCloseClicked()函数
    connect(m_pButtonClose, SIGNAL(clicked()), this, SLOT(onButtonCloseClicked()));
}

//设置标题栏图标
void TitleBar::setTitleIcon(const QString &filePath)
{
    QPixmap titleIcon(filePath);            //创建图片
    m_pIcon->setFixedSize(titleIcon.size());//设置标题栏图标固定大小为图片大小
    m_pIcon->setPixmap(titleIcon);          //设置标题栏图标
}

//设置标题栏内容
void TitleBar::setTitleContent(const QString &titleContent)
{
    m_pTitleContent->setText(titleContent);
    m_titleContent = titleContent;
}

//设置标题栏的长度
void TitleBar::setTitleWidth(int width)
{
    setFixedWidth(width);
}

//设置标题栏按钮类型
void TitleBar::setButtonType(ButtonType buttontype)
{
    m_buttonType = buttontype;  //记录按钮类型

    switch (buttontype)
    {
        case MIN_BUTTON:        //只显示最小化及关闭按钮
        {
            m_pButtonRestore->setVisible(false);
            m_pButtonMax->setVisible(false);
        }
        break;
        case MIN_MAX_BUTTON:    //显示最小化、最大化及关闭按钮
        {
            m_pButtonRestore->setVisible(false);
        }
        break;
        case ONLY_CLOSE_BUTTON: //只显示关闭按钮
        {
            m_pButtonRestore->setVisible(false);
            m_pButtonMax->setVisible(false);
            m_pButtonMin->setVisible(false);
        }
        break;
    }
}

//保存窗口最大化前窗口的位置及大小
void TitleBar::saveRestoreInfo(const QPoint &point, const QSize &size)
{
    m_restorePos = point;   //窗口最大化前的位置
    m_restoreSize = size;   //窗口最大化前的大小
}

//获取窗口最大化前窗口的位置及大小
void TitleBar::getRestoreInfo(QPoint &point, QSize &size)
{
    point = m_restorePos;   //窗口最大化前的位置
    size = m_restoreSize;   //窗口最大化前的大小
}

//绘制标题栏
void TitleBar::paintEvent(QPaintEvent* event)
{
    //设置背景色
    QPainter painter(this); //画家 对标题栏进行绘画
    QPainterPath pathBack;  //绘图路径 用于绘制背景
    pathBack.setFillRule(Qt::WindingFill);	//设置填充规则
    pathBack.addRoundedRect(QRect(0, 0, width(), height()), 3, 3);  //添加圆角矩形到绘图路径
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);   //设置渲染 像素转换采用平滑转换

    //当窗口最大化或还原后，窗口长度改变，标题栏相应做出改变
    //parentWidget()返回父部件
    if (width() != parentWidget()->width())     //如果标题栏当前宽度跟父部件宽度不一致
    {
        setFixedWidth(parentWidget()->width()); //设置标题栏宽度跟父部件宽度一致
    }

    QWidget::paintEvent(event);
}

//双击响应事件，主要实现双击标题栏进行最大化最小化的操作
void TitleBar::mouseDoubleClickEvent(QMouseEvent* event)
{
    //只有存在最大化、最小化按钮才有效
    if (m_buttonType == MIN_MAX_BUTTON)
    {
        //判断目前是正常窗口还是最大化窗口
        if (m_pButtonMax->isVisible())  //如果最大化存在 则当前是正常窗口
        {
            onButtonMaxClicked();       //窗口最大化
        }
        else    //目前是最大化窗口
        {
            onButtonRestoreClicked();   //窗口还原到正常
        }
    }

    return QWidget::mouseDoubleClickEvent(event);
}

//通过鼠标按下、鼠标移动、鼠标释放事件实现拖动标题栏达到移动窗口效果
void TitleBar::mousePressEvent(QMouseEvent* event)
{
    //判断当前按钮类型是否存在最小化&最大化
    if (m_buttonType == MIN_MAX_BUTTON)
    {
        //在窗口最大化时禁止拖动窗口
        if (m_pButtonMax->isVisible())  //最大化存在 说明窗口为正常大小
        {
            m_isPressed = true;                     //鼠标按下标志设为true
            //globalPos()返回事件发生时鼠标坐在的全局位置
            m_startMovePos = event->globalPos();    //设置当前位置
        }
    }
    else    //窗口不存在最小化或最大化
    {
        m_isPressed = true;
        m_startMovePos = event->globalPos();
    }

    return QWidget::mousePressEvent(event);
}

//鼠标移动事件
void TitleBar::mouseMoveEvent(QMouseEvent* event)
{
    if (m_isPressed)    //判断鼠标是否按下
    {
        QPoint movePoint = event->globalPos() - m_startMovePos;
        QPoint widgetPos = parentWidget()->pos();
        m_startMovePos = event->globalPos();
        parentWidget()->move(widgetPos.x() + movePoint.x(), widgetPos.y() + movePoint.y());
    }

    return QWidget::mouseMoveEvent(event);
}

//鼠标松开事件
void TitleBar::mouseReleaseEvent(QMouseEvent* event)
{
    m_isPressed = false;    //鼠标按下标志设为false

    return QWidget::mouseReleaseEvent(event);
}

//加载样式表
void TitleBar::loadStyleSheet(const QString &sheetName)
{
    QFile file(":/Resources/QSS/" + sheetName + ".css");    //选择操作的文件
    file.open(QFile::ReadOnly);     //读方式打开文件
    if (file.isOpen())  //判断是否打开成功
    {
        QString styleSheet = this->styleSheet();        //获取当前窗口样式 保存到样式表变量
        styleSheet += QLatin1String(file.readAll());    //获取文件数据 追加到样式表变量
        setStyleSheet(styleSheet);      //设置样式表
    }
}

//最小化按钮 槽函数
void TitleBar::onButtonMinClicked()
{
    emit signalButtonMinClicked();          //发射最小化按钮被点击信号
}

//最大化还原按钮 槽函数
void TitleBar::onButtonRestoreClicked()
{
    m_pButtonRestore->setVisible(false);    //设置最大化还原按钮不可见
    m_pButtonMax->setVisible(true);         //设置最大化按钮可见
    emit signalButtonRestoreClicked();      //发射最大化还原按钮被点击信号
}

//最大化按钮 槽函数
void TitleBar::onButtonMaxClicked()
{
    m_pButtonMax->setVisible(false);        //设置最大化按钮不可见
    m_pButtonRestore->setVisible(true);     //设置最大化还原按钮可见
    emit signalButtonMaxClicked();          //发射最大化按钮被点击信号
}

//关闭按钮 槽函数
void TitleBar::onButtonCloseClicked()
{
    emit signalButtonCloseClicked();        //发射关闭按钮被点击信号
}
