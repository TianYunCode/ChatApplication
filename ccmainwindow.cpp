#include "ccmainwindow.h"
#include "ui_ccmainwindow.h"
#include "skinwindow.h"
#include "systray.h"
#include "notifymanager.h"
#include "rootcontatitem.h"
#include "contactitem.h"
#include "windowmanager.h"
#include "talkwindowshell.h"

#include <QHBoxLayout>
#include <QProxyStyle>
#include <QPainter>
#include <QTimer>
#include <QEvent>
#include <QTreeWidgetItem>
#include <QMouseEvent>
#include <QApplication>
#include <QSqlQuery>
#include <QDebug>

QString gstrLoginHeadPath;
extern QString gLoginEmployeeID;    //userlogin.cpp 中的全局变量 需要外部声明 防止报错

//重写自定义风格类
class CustomProxyStyle :public QProxyStyle
{
public:
    virtual void drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget = nullptr) const
    {
        //去掉windows中部件默认的边框或虚线框，部件获取焦点时直接放回，不进行绘制
        if (element == PE_FrameFocusRect)
        {
            return;
        }
        else
        {
            QProxyStyle::drawPrimitive(element, option, painter, widget);
        }
    }
};

CCMainWindow::CCMainWindow(QString account, bool isAccountLogin, QWidget *parent) : BasicWindow(parent), ui(new Ui::CCMainWindow)
  ,m_isAccountLogin(isAccountLogin), m_account(account), isCloseSkinWindow(false), m_skinWindow(nullptr), systray(new SysTray(this))
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::Tool);   //设置窗体风格为工具风格 任务栏就不会显示该窗口
    loadStyleSheet("CCMainWindow");             //加载样式表(主窗口)
    setHeadPixmap(getHeadPicturePath());        //加载头像
    initControl();                              //初始化控件(主窗口)
    initTimer();                                //初始化计时器 改变等级
}

CCMainWindow::~CCMainWindow()
{
    delete ui;

    if(m_skinWindow)
    {
        delete m_skinWindow;
        m_skinWindow = nullptr;
    }

    if(systray)
    {
        delete systray;
        systray = nullptr;
    }
}

//初始化计时器 改变等级
void CCMainWindow::initTimer()
{
    QTimer *timer = new QTimer(this);   //创建计时器
    timer->setInterval(1000);           //设置计时器触发时间间隔为 1000 毫秒ms

    //计时器触发 执行 lambda表达式
    connect(timer, &QTimer::timeout,[this]{
        static int level = 0;   //设置等级数
        if (level == 99)        //如果等级到 99 级
        {
            level = 0;          //重置等级
        }
        level++;                //等级加 1
        setLevelPixmap(level);  //设置等级
    });

    timer->start();             //计时器开启
}

//初始化控件(主窗口)
void CCMainWindow::initControl()
{
    //树(treeWidget)获取焦点时不绘制边框
    ui->treeWidget->setStyle(new CustomProxyStyle);     //给 treeWidget 设置自定义风格
    setLevelPixmap(0);  //设置等级为 0
    setStatusMenuIcon(":/Resources/MainWindow/StatusSucceeded.png");    //设置登录状态图标(在线)

    QHBoxLayout *appupLayout = new QHBoxLayout;     //创建一个水平布局管理器
    appupLayout->setContentsMargins(0, 0, 0, 0);    //设置左上右底间隙为0
    //创建部件并添加到水平布局管理器
    //主窗口上边部件
    appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_7.png", "app_7"));
    appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_2.png", "app_2"));
    appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_3.png", "app_3"));
    appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_4.png", "app_4"));
    appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_5.png", "app_5"));
    appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_6.png", "app_6"));
    appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/skin.png", "app_skin"));
    appupLayout->addStretch();              //添加一个水平垫片(水平拉伸)
    appupLayout->setSpacing(2);             //设置部件间间隙为 2 像素
    ui->appWidget->setLayout(appupLayout);  //将水平布局管理器添加到 appWidget 中

    //主窗口底部部件
    ui->bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_10.png", "app_10"));
    ui->bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_8.png", "app_8"));
    ui->bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_11.png", "app_11"));
    ui->bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_9.png", "app_9"));
    ui->bottomLayout_up->addStretch();      //添加一个水平垫片(水平拉伸)

    initContactTree();      //初始化联系人树(列表)

    //给个性签名安装事件过滤器
    ui->lineEdit->installEventFilter(this);
    //给好友搜索安装事件过滤器
    ui->searchLineEdit->installEventFilter(this);

    //点击最小化按钮 执行 onShowHide(bool)函数 隐藏主窗口
    connect(ui->sysmin, SIGNAL(clicked(bool)), this, SLOT(onShowHide(bool)));
    //点击关闭按钮 执行 onShowClose(bool)函数 关闭主窗口
    connect(ui->sysclose, SIGNAL(clicked(bool)), this, SLOT(onShowClose(bool)));

    //如果接收到皮肤改变信号 更新搜索编辑框样式
    connect(NotifyManager::getInstance(), &NotifyManager::signalSkinChanged, [this]()
    {
        updateSeachStyle();
    });
}

//获取头像路径(从数据库里获取)
QString CCMainWindow::getHeadPicturePath()
{
    QString strPicturePath;

    if (!m_isAccountLogin)  //QQ号登录
    {
        QSqlQuery queryPicture(QString("SELECT picture FROM tab_employees WHERE employeeID = %1").arg(gLoginEmployeeID));
        queryPicture.exec();
        queryPicture.next();

        strPicturePath = queryPicture.value(0).toString();
    }
    else    //账号登录
    {
        QSqlQuery queryEmployeeID(QString("SELECT employeeID FROM tab_accounts WHERE account = '%1'").arg(m_account));
        queryEmployeeID.exec();
        queryEmployeeID.next();

        int employeeID = queryEmployeeID.value(0).toInt();

        QSqlQuery queryPicture(QString("SELECT picture FROM tab_employees WHERE employeeID = %1").arg(employeeID));
        queryPicture.exec();
        queryPicture.next();

        strPicturePath = queryPicture.value(0).toString();
    }

    gstrLoginHeadPath = strPicturePath;

    return strPicturePath;
}


//更新搜索样式
void CCMainWindow::updateSeachStyle()
{
    //设置(恢复)搜索编辑框所在窗体的样式
    ui->searchWidget->setStyleSheet(QString("QWidget#searchWidget{background-color:rgba(%1,%2,%3,50);border-bottom:1px solid rgba(%1,%2,%3,30)}\
                                            QPushButton#searchBtn{border-image:url(:/Resources/MainWindow/search/search_icon.png)}")
                                        .arg(m_colorBackGround.red())
                                        .arg(m_colorBackGround.green())
                                        .arg(m_colorBackGround.blue()));
}

//添加公司部门
void CCMainWindow::addCompanyDeps(QTreeWidgetItem * pRootGroupItem, int DepID)
{
    QTreeWidgetItem* pChild = new QTreeWidgetItem;      //联系人

    QPixmap pix;        //创建位图
    pix.load(":/Resources/MainWindow/head_mask.png");   //加载位图

    //添加子节点
    pChild->setData(0, Qt::UserRole, 1);            //子项数据设为1
    pChild->setData(0, Qt::UserRole + 1, DepID);    //给 Qt::UserRole + 1 窗口设置唯一ID

    //获取公司、部门头像
    QPixmap groupPix;
    QSqlQuery queryPicture(QString("SELECT picture FROM tab_department WHERE departmentID = %1").arg(DepID));
    queryPicture.exec();
    queryPicture.next();    //指向第一条数据 在这里 next 跟 first 功能一致
    groupPix.load(queryPicture.value(0).toString());

    //获取部门名称
    QString strDepName;
    QSqlQuery querDepName(QString("SELECT department_name FROM tab_department WHERE departmentID = %1").arg(DepID));
    querDepName.exec();
    querDepName.first();
    strDepName = querDepName.value(0).toString();

    //创建联系人选项
    ContactItem* pContactItem = new ContactItem(ui->treeWidget);
    //设置联系人头像
    pContactItem->setHeadPixmap(getRoundImage(groupPix,pix,pContactItem->getHeadLabelSize()));
    //设置联系人用户名
    pContactItem->setUserName(strDepName);

    pRootGroupItem->addChild(pChild);   //添加子节点
    ui->treeWidget->setItemWidget(pChild, 0, pContactItem);
}

//设置用户名
void CCMainWindow::setUserName(const QString & username)
{
    ui->nameLabel->adjustSize();    //设置标签根据内容调整自己的尺寸

    //设置文本过长则进行省略
    //fontMetrics()返回QFontMetrics类(字体体积)对象  elidedText()设置文本省略模式 Qt::ElideRight为右省略
    QString name =  ui->nameLabel->fontMetrics().elidedText(username, Qt::ElideRight, ui->nameLabel->width());

    ui->nameLabel->setText(name);   //设置用户名
}

//设置等级
void CCMainWindow::setLevelPixmap(int level)
{
    QPixmap levelPixmap(ui->levelBtn->size());  //创建一个等级位图 大小为等级按钮的大小
    levelPixmap.fill(Qt::transparent);          //给等级位图设置透明属性

    QPainter painter(&levelPixmap);             //画家
    painter.drawPixmap(0, 4, QPixmap(":/Resources/MainWindow/lv.png")); //绘制 lv 图标

    int unitNum = level % 10;   //个位数
    int tenNum = level / 10;    //十位数

    //十位,截取图片中的部分进行绘制
    //drawPixmap(绘制点x，绘制点y,图片，图片左上角x,图片左上角y,拷贝的宽度，拷贝的高度)
    painter.drawPixmap(10, 4, QPixmap(":/Resources/MainWindow/levelvalue.png"), tenNum * 6, 0, 6, 7);

    //个位
    painter.drawPixmap(16, 4, QPixmap(":/Resources/MainWindow/levelvalue.png"), unitNum * 6, 0, 6, 7);

    ui->levelBtn->setIcon(levelPixmap); //设置等级图片
    ui->levelBtn->setIconSize(ui->levelBtn->size());    //设置等级图片大小为按钮大小
}

//设置头像
void CCMainWindow::setHeadPixmap(const QString & headPath)
{
    QPixmap pix;    //创建空位图
    pix.load(":/Resources/MainWindow/head_mask.png");   //给空位图加载一直空白圆形头像
    //获取(制作)圆头像 并设置头像
    ui->headLabel->setPixmap(getRoundImage(QPixmap(headPath), pix, ui->headLabel->size()));
}

//设置状态(在线)
void CCMainWindow::setStatusMenuIcon(const QString & statusPath)
{
    QPixmap statusBtnPixmap(ui->stausBtn->size());  //创建一个状态位图 大小为按钮大小
    statusBtnPixmap.fill(Qt::transparent);          //设置状态位图为透明属性

    QPainter painter(&statusBtnPixmap);             //画家
    painter.drawPixmap(4, 4, QPixmap(statusPath));  //绘制状态位图

    ui->stausBtn->setIcon(statusBtnPixmap);         //设置状态图标
    ui->stausBtn->setIconSize(ui->stausBtn->size());//设置状态图标大小为按钮大小
}

//添加应用部件(app图片路径，app部件对象名)
QWidget * CCMainWindow::addOtherAppExtension(const QString & appPath, const QString & appName)
{
    QPushButton* btn = new QPushButton(this);   //创建按钮部件
    btn->setFixedSize(20, 20);                  //设置固定大小

    QPixmap pixmap(btn->size());                //创建部件位图 大小为部件大小
    pixmap.fill(Qt::transparent);               //设置部件位图为透明属性

    QPainter painter(&pixmap);                  //画家
    QPixmap appPixmap(appPath);                 //拿到部件图标
    //使用拿到的部件图标对部件位图进行绘制
    painter.drawPixmap( (btn->width() - appPixmap.width()) / 2, (btn->height() - appPixmap.height()) / 2, appPixmap);
    btn->setIcon(pixmap);                       //给部件设置图标
    btn->setIconSize(btn->size());              //设置部件图标大小为部件大小
    btn->setObjectName(appName);                //设置部件对象名
    btn->setProperty("hasborder", true);        //设置边框属性 有边框

    //点击任意部件 会执行 onAppIconClicked()函数来判断是哪个部件点击
    connect(btn, &QPushButton::clicked, this, &CCMainWindow::onAppIconClicked);

    return btn;
}

//初始化联系人树(列表)
void CCMainWindow::initContactTree()
{
    //展开与收缩时的信号
    connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onItemClicked(QTreeWidgetItem*, int)));              //点击信号
    connect(ui->treeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(onItemExpanded(QTreeWidgetItem*)));                      //展开树
    connect(ui->treeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(onItemCollapsed(QTreeWidgetItem*)));                    //收缩树
    connect(ui->treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));  //双击信号

    //根节点
    QTreeWidgetItem* pRootGroupItem = new QTreeWidgetItem;
    pRootGroupItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);    //设置根节点展开收缩箭头
    pRootGroupItem->setData(0, Qt::UserRole, 0);                                //根节点数据设为 0
    RootContatItem* pItemName = new RootContatItem(true, ui->treeWidget);       //创建箭头

    //获取公司部门ID(公司群号)
    QSqlQuery queryCompDepID(QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'").arg(QString::fromLocal8Bit("公司群")));
    queryCompDepID.exec();  //执行查询语句
    queryCompDepID.first(); //指向结果集第一条
    int CompDepID = queryCompDepID.value(0).toInt();    //取出第 0 列的值

    //获取QQ登录者所在的部门ID（部门群号）
    QSqlQuery querySelfDepID(QString("SELECT departmentID FROM tab_employees WHERE employeeID = %1").arg(gLoginEmployeeID));
    querySelfDepID.exec();
    querySelfDepID.first();
    int SelfDepID = querySelfDepID.value(0).toInt();

    //初始化公司群及登录者所在的群
    addCompanyDeps(pRootGroupItem, CompDepID);
    addCompanyDeps(pRootGroupItem, SelfDepID);

    QString strGroupName = QString::fromLocal8Bit("天云科技");  //创建组
    pItemName->setText(strGroupName);   //给箭头设置名字

    //插入分组节点
    ui->treeWidget->addTopLevelItem(pRootGroupItem);
    ui->treeWidget->setItemWidget(pRootGroupItem, 0, pItemName);    //根节点 第 0 个 箭头
}

//重写重置大小事件
void CCMainWindow::resizeEvent(QResizeEvent * event)
{
    //QString::fromLocal8Bit() 从本地编码转换为 Unicode 编码
    //setUserName(QString::fromLocal8Bit("天云科技-越分享越拥有")); //设置用户名

    BasicWindow::resizeEvent(event);
}

//重写事件过滤器
bool CCMainWindow::eventFilter(QObject * obj, QEvent * event)
{
    //判断搜索编辑框是否是被监视的对象
    if (ui->searchLineEdit == obj)
    {
        //判断当前接受到的事件是不是键盘焦点事件
        if (event->type() == QEvent::FocusIn)
        {
            //对搜索编辑框所在的窗体设置样式
            ui->searchWidget->setStyleSheet(QString("QWidget#searchWidget{background-color:rgb(255,255,255);border-bottom:1px solid rgba(%1,%2,%3,100)}\
                                                    QPushButton#searchBtn{border-image:url(:/Resources/MainWindow/search/main_search_deldown.png)} \
                                                    QPushButton#searchBtn:hover{border-image:url(:/Resources/MainWindow/search/main_search_delhighlight.png)} \
                                                    QPushButton#searchBtn:pressed{border-image:url(:/Resources/MainWindow/search/main_search_delhighdown.png)}")
                                                .arg(m_colorBackGround.red())
                                                .arg(m_colorBackGround.green())
                                                .arg(m_colorBackGround.blue()));
        }   //如果不是键盘焦点事件
        else if (event->type() == QEvent::FocusOut)
        {
            updateSeachStyle(); //更新搜索样式(恢复搜索编辑框所在窗体的样式)
        }
    }

    return false;
}

//重写鼠标按下事件
void CCMainWindow::mousePressEvent(QMouseEvent * event)
{
    //获取当前鼠标按下位置不是主窗口搜索编辑框并且如果还存在焦点
    if (qApp->widgetAt(event->pos()) != ui->searchLineEdit && ui->searchLineEdit->hasFocus())
    {
        ui->searchLineEdit->clearFocus();   //清除主窗口搜索编辑框焦点
    }//获取当前鼠标按下位置不是个性签名编辑框并且如果还存在焦点
    else if (qApp->widgetAt(event->pos()) != ui->lineEdit && ui->lineEdit->hasFocus())
    {
        ui->lineEdit->clearFocus();         //清除个性签名编辑框焦点
    }

    BasicWindow::mousePressEvent(event);
}

//树被单击 槽函数
void CCMainWindow::onItemClicked(QTreeWidgetItem * item, int column)
{
    bool bIsChild = item->data(0, Qt::UserRole).toBool();   //获取是否是子项
    if (!bIsChild)  //不是子项
    {
        //展开 提前是还没展开
        item->setExpanded(!item->isExpanded());     //未展开则展开子项
    }
}

//展开树
void CCMainWindow::onItemExpanded(QTreeWidgetItem * item)
{
    bool bIsChild = item->data(0, Qt::UserRole).toBool();   //获取是否是子项
    if (!bIsChild)  //不是子项
    {
        //dynamic_cast 将基类对象指针(或引用)转换到继承类指针
        RootContatItem* prootItem = dynamic_cast<RootContatItem *>(ui->treeWidget->itemWidget(item, 0));
        if(prootItem)
        {
            prootItem->setExpanded(true);
        }

    }
}

//收缩树
void CCMainWindow::onItemCollapsed(QTreeWidgetItem * item)
{
    bool bIsChild = item->data(0, Qt::UserRole).toBool();   //获取是否是子项
    if (!bIsChild)  //不是子项
    {
        //dynamic_cast 将基类对象指针(或引用)转换到继承类指针
        RootContatItem* prootItem = dynamic_cast<RootContatItem *>(ui->treeWidget->itemWidget(item, 0));
        if (prootItem)
        {
            prootItem->setExpanded(false);

        }

    }
}

//树被双击 槽函数
void CCMainWindow::onItemDoubleClicked(QTreeWidgetItem * item, int column)
{
    bool bIsChild = item->data(0, Qt::UserRole).toBool();   //获取是否是子项
    if (bIsChild)   //是子项
    {
        //添加新的聊天窗口
        WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString());
    }
}

//皮肤窗口关闭执行的槽函数 把 isCloseSkinWindow 设置 false
void CCMainWindow::onCloseSkinWindow()
{
    delete m_skinWindow;
    m_skinWindow = nullptr;
    isCloseSkinWindow = false;
}

void CCMainWindow::onAppIconClicked()
{
    //判断信号发送者的对象名是否是app_skin
    if (sender()->objectName() == "app_skin")
    {
        //如果标志 false
        if(!isCloseSkinWindow)
        {
            m_skinWindow = new SkinWindow;  //创建皮肤窗口
            isCloseSkinWindow = true;       //标志设为 true
            //当皮肤窗口点击关闭时 出发 signalCloseSkinWindow()信号 执行 onCloseSkinWindow()函数
            connect(m_skinWindow,&SkinWindow::signalCloseSkinWindow,this,&CCMainWindow::onCloseSkinWindow);
        }
        m_skinWindow->show();   //显示窗口
        m_skinWindow->onShowNormal(true);   //激活窗口

    }
}
