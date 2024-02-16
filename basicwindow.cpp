#include "basicwindow.h"
#include "commonutils.h"
#include "notifymanager.h"

#include <QDesktopWidget>
#include <QFile>
#include <QStyleOption>
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>
#include <QSqlQuery>

extern QString gLoginEmployeeID;

BasicWindow::BasicWindow(QWidget *parent) : QDialog(parent)
{
    m_colorBackGround = CommonUtils::getDefaultSkinColor(); //获取默认皮肤颜色
    setWindowFlags(Qt::FramelessWindowHint);                //设置窗体风格为无边框风格
    setAttribute(Qt::WA_TranslucentBackground, true);       //设置窗体属性为透明属性

    //当接受到 signalSkinChanged(const QColor&)信号时 执行 onSignalSkinChanged(const QColor&)进行皮肤改变
    connect(NotifyManager::getInstance(), SIGNAL(signalSkinChanged(const QColor&)), this, SLOT(onSignalSkinChanged(const QColor&)));
}

BasicWindow::~BasicWindow()
{
}

//皮肤改变
void BasicWindow::onSignalSkinChanged(const QColor &color)
{
    m_colorBackGround = color;      //设置当前背景颜色
    loadStyleSheet(m_styleName);    //重新加载样式表
}

//设置标题栏标题
void BasicWindow::setTitleBarTitle(const QString &title, const QString &icon)
{
    _titleBar->setTitleIcon(icon);      //设置标题栏图标
    _titleBar->setTitleContent(title);  //设置标题栏内容
}

//初始化标题栏
void BasicWindow::initTitleBar(ButtonType buttontype)
{
    _titleBar = new TitleBar(this);         //初始化标题栏
    _titleBar->setButtonType(buttontype);   //设置标题栏类型
    _titleBar->move(0, 0);                  //标题栏位置移动到主窗口(0,0)位置

    //点击最小化按钮 执行onButtonMinClicked()函数
    connect(_titleBar, SIGNAL(signalButtonMinClicked()), this, SLOT(onButtonMinClicked()));
    //点击最大化还原按钮 执行onButtonRestoreClicked()函数
    connect(_titleBar, SIGNAL(signalButtonRestoreClicked()), this, SLOT(onButtonRestoreClicked()));
    //点击最大化按钮 执行onButtonMaxClicked()函数
    connect(_titleBar, SIGNAL(signalButtonMaxClicked()), this, SLOT(onButtonMaxClicked()));
    //点击关闭按钮 执行onButtonCloseClicked()函数
    connect(_titleBar, SIGNAL(signalButtonCloseClicked()), this, SLOT(onButtonCloseClicked()));
}

//加载样式表
void BasicWindow::loadStyleSheet(const QString &sheetName)
{
    m_styleName = sheetName;    //记录样式表名称
    QFile file(":/Resources/QSS/" + sheetName + ".css");    //操作对应样式表文件
    file.open(QFile::ReadOnly);     //读方式打开文件

    if (file.isOpen())  //判断是否打开成功
    {
        setStyleSheet("");  //设置空样式表
        QString qsstyleSheet = QLatin1String(file.readAll());   //读取样式表文件全部数据保存到样式表变量中

        //获取用户当前的皮肤RGB值
        QString r = QString::number(m_colorBackGround.red());   //红
        QString g = QString::number(m_colorBackGround.green()); //蓝
        QString b = QString::number(m_colorBackGround.blue());  //绿

        //在样式表变量后追加样式
        qsstyleSheet += QString("QWidget[titleskin=true]\
                                {background-color:rgb(%1,%2,%3);\
                                border-top-left-radius:4px;}\
                                QWidget[bottomskin=true]\
                                {border-top:1px solid rgba(%1,%2,%3,100);\
                                background-color:rgba(%1,%2,%3,50);\
                                border-bottom-left-radius:4px;\
                                border-bottom-right-radius:4px;}")
                                .arg(r).arg(g).arg(b);
        setStyleSheet(qsstyleSheet);    //设置样式表
    }

    file.close();   //关闭文件
}

//初始化背景颜色
void BasicWindow::initBackGroundColor()
{
    QStyleOption opt;   //风格
    opt.init(this);     //获取风格

    QPainter p(this);   //画家
    //QStyle::PE_Widget是边框 opt是风格 p是画家
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);  //背景图绘制
}

//子类化部件时，需要重写绘图事件设置背景图
//绘制事件
void BasicWindow::paintEvent(QPaintEvent* event)
{
    initBackGroundColor();      //初始化背景颜色

    QDialog::paintEvent(event);
}

//获取(制作)圆头像 头像转圆头像 src为矩形头像 mask空白圆形头像 maskSize空白圆形头像大小
QPixmap BasicWindow::getRoundImage(const QPixmap &src, QPixmap &mask, QSize maskSize)
{
    if (maskSize == QSize(0, 0))
    {
        maskSize = mask.size();
    }
    else
    {
        //缩放 以maskSize大小进行缩放 Qt::KeepAspectRatio代表尽可能大的矩形 Qt::SmoothTransformation代表转换模式为平滑模式
        mask = mask.scaled(maskSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    //保存转换后的图像
    QImage resultImage(maskSize, QImage::Format_ARGB32_Premultiplied);  //头像大小为maskSize 转换格式 QImage::Format_ARGB32_Premultiplied 该格式转换后会比较清晰
    QPainter painter(&resultImage);     //画家
    painter.setCompositionMode(QPainter::CompositionMode_Source);       //设置叠加模式
    painter.fillRect(resultImage.rect(), Qt::transparent);              //填充矩形 大小为resultImage.rect() 模式为Qt::transparent 透明模式
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawPixmap(0, 0, mask);     //与 mask 进行叠加
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.drawPixmap(0, 0, src.scaled(maskSize,Qt::KeepAspectRatio, Qt::SmoothTransformation));
    painter.end();

    return QPixmap::fromImage(resultImage);
}

//关闭窗口
void BasicWindow::onShowClose(bool)
{
    close();
}

//最小化窗口
void BasicWindow::onShowMin(bool)
{
    showMinimized();
}

//隐藏窗口
void BasicWindow::onShowHide(bool)
{
    hide();
}

//显示正常窗口
void BasicWindow::onShowNormal(bool)
{
    show();
    activateWindow();   //激活窗口
}

//退出程序
void BasicWindow::onShowQuit(bool)
{
    //更新登录状态为离线
    QString strSqlStatus = QString("UPDATE tab_employees SET online_status = 1 WHERE employeeID = %1").arg(gLoginEmployeeID);
    QSqlQuery sqlStatus(strSqlStatus);
    sqlStatus.exec();

    QApplication::quit();
}

//鼠标移动事件
void BasicWindow::mouseMoveEvent(QMouseEvent *e)
{
    //判断鼠标是否按下 并且按下的是不是左键
    if (m_mousePressed && (e->buttons() && Qt::LeftButton))
    {
        //e->globalPos() 事件发生时全局坐标，相对于屏幕左上角(0,0)
        move(e->globalPos() - m_mousePoint);    //移动窗口
        e->accept();
    }
}

//鼠标按下事件
void BasicWindow::mousePressEvent(QMouseEvent *e)
{
    //判断按下的是不是左键
    if (e->button() == Qt::LeftButton)
    {
        //pos() 事件发生时相对于窗口左上角(0,0)的偏移
        m_mousePressed = true;  //设置鼠标按下标志为true
        m_mousePoint = e->globalPos() - pos();  //设置鼠标当前坐标
        e->accept();
    }
}

//鼠标释放事件
void BasicWindow::mouseReleaseEvent(QMouseEvent*)
{
    //设置鼠标按下标志为false
    m_mousePressed = false;
}

//最小化按钮被点击 槽函数
void BasicWindow::onButtonMinClicked()
{
    //判断当前窗体是不是工具风格
    if (Qt::Tool == (windowFlags() & Qt::Tool))
    {
        hide(); //隐藏窗口
    }
    else    //如果不是工具风格
    {
        showMinimized();    //最小化窗口
    }
}

//最大化还原按钮被点击 槽函数
void BasicWindow::onButtonRestoreClicked()
{
    QPoint windowPos;   //原窗口位置
    QSize windowSize;   //原窗口大小
    _titleBar->getRestoreInfo(windowPos, windowSize);   //获取原窗口位置和大小并保存
    setGeometry(QRect(windowPos, windowSize));  //设置(几何属性)窗口位置和大小
}

//最大化按钮被点击 槽函数
void BasicWindow::onButtonMaxClicked()
{
    _titleBar->saveRestoreInfo(pos(), QSize(width(), height()));        //保存当前窗口位置和大小
    QRect desktopRect = QApplication::desktop()->availableGeometry();   //获取桌面矩形
    QRect factRect = QRect(desktopRect.x() - 3, desktopRect.y() - 3, desktopRect.width() + 6, desktopRect.height() + 6);    //最大化矩形信息
    setGeometry(factRect);  //设置(几何属性)窗口位置和大小
}

//关闭按钮被点击 槽函数
void BasicWindow::onButtonCloseClicked()
{
    close();
}
