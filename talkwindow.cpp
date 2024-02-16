#include "talkwindow.h"
#include "rootcontatitem.h"
#include "contactitem.h"
#include "commonutils.h"
#include "windowmanager.h"
#include "sendfile.h"

#include <QToolTip>
#include <QFile>
#include <QMessageBox>
#include <QSqlQueryModel>
#include <QSqlQuery>

extern QString gLoginEmployeeID;

TalkWindow::TalkWindow(QWidget *parent, const QString& uid) : QWidget(parent), ui(new Ui::TalkWindow), m_talkId(uid)
{
    ui->setupUi(this);
    WindowManager::getInstance()->addWindowName(m_talkId, this);
    setAttribute(Qt::WA_DeleteOnClose); //设置窗口属性 关闭即释放
    initGroupTalkStatus();  //初始化群聊状态
    initControl();  //初始化控件(皮肤窗口)
}

TalkWindow::~TalkWindow()
{
    delete ui;
    WindowManager::getInstance()->deleteWindowName(m_talkId);
}

//把选中的表情添加到当前聊天窗口消息编辑框中
void TalkWindow::addEmotionImage(int emotionNum)
{
    ui->textEdit->setFocus();               //设置聊天窗口的文本编辑框聚焦
    ui->textEdit->addEmotionUrl(emotionNum);//给聊天窗口的文本编辑框添加表情
}

//设置当前聊天窗口的名称
void TalkWindow::setWindowName(const QString & name)
{
    ui->nameLabel->setText(name);
}

//获取当前聊天窗口ID
QString TalkWindow::getTalkId()
{
    return m_talkId;
}

//文件打开按钮被点击
void TalkWindow::onFileOpenBtnClicked(bool)
{
    SendFile *sendFile = new SendFile(this);
    sendFile->show();
}

//点击发送按钮 槽函数
void TalkWindow::onSendBtnClicked(bool)
{
    //将聊天窗口的文本编辑框的内容转换成纯文本 再判断是否为空
    if (ui->textEdit->toPlainText().isEmpty())
    {
        QToolTip::showText(this->mapToGlobal(QPoint(630, 660)), QString::fromLocal8Bit("发送的信息不能为空！"), this, QRect(0, 0, 120, 100), 2000);

        return;
    }

    QString html = ui->textEdit->document()->toHtml();  //获取聊天窗口的文本编辑框的内容转换成文档再转换成 html

    //文本html如果没有字体则添加字体
    if (!html.contains(".png") && !html.contains("</span>"))    //不包含 png 且 不包含 </span>
    {
        QString fontHtml;
        QString text = ui->textEdit->toPlainText();
        QFile file(":/Resources/MainWindow/MsgHtml/msgFont.txt");
        if (file.open(QIODevice::ReadOnly))
        {
            fontHtml = file.readAll();
            fontHtml.replace("%1", text);
            file.close();
        }
        else
        {
            QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("文件 msgFont.txt 不存在！"));
            return;
        }

        if (!html.contains(fontHtml))
        {
            html.replace(text, fontHtml);
        }
    }

    ui->textEdit->clear();                  //清空聊天窗口的文本编辑框的内容
    ui->textEdit->deletAllEmotionImage();   //删除全部表情图片(在文本编辑框里删除)

    ui->msgWidget->appendMsg(html); //收信息窗口添加信息
}

//双击聊天窗口右边群好友列表 槽函数
void TalkWindow::onItemDoubleClicked(QTreeWidgetItem* item, int column)
{
    bool bIsChild = item->data(0, Qt::UserRole).toBool();   //获取是否是子项
    if (bIsChild)   //如果是子项
    {
        QString talkId = item->data(0,Qt::UserRole + 1).toString();

        if(talkId == gLoginEmployeeID) return;  //如果双击的是自己 就不做任何处理

        //获取子项的名称
        QString strPeopleName = m_groupPeopleMap.value(item);
        //添加新的聊天窗口
        WindowManager::getInstance()->addNewTalkWindow(item->data(0,Qt::UserRole + 1).toString());
    }
}

//初始化控件(皮肤窗口)
void TalkWindow::initControl()
{
    QList<int> rightWidgetSize;                 //右边部件的尺寸
    rightWidgetSize << 600 << 138;              //设置为600*138
    ui->bodySplitter->setSizes(rightWidgetSize);//右边的分裂器设置大小

    ui->textEdit->setFontPointSize(10);         //设置聊天窗口的文本编辑框字体大小
    ui->textEdit->setFocus();                   //设置聊天窗口的文本编辑框聚焦

    //点击最小化聊天窗口 父窗体执行最小化
    connect(ui->sysmin, SIGNAL(clicked(bool)), parent(), SLOT(onShowMin(bool)));
    //点击关闭聊天窗口 父窗体执行关闭
    connect(ui->sysclose, SIGNAL(clicked(bool)), parent(), SLOT(onShowClose(bool)));
    connect(ui->closeBtn, SIGNAL(clicked(bool)), parent(), SLOT(onShowClose(bool)));
    //点击表情按钮
    connect(ui->faceBtn, SIGNAL(clicked(bool)), parent(), SLOT(onEmotionBtnClicked(bool)));
    //点击发送按钮
    connect(ui->sendBtn, SIGNAL(clicked(bool)), this, SLOT(onSendBtnClicked(bool)));
    //点击文件打开按钮
    connect(ui->fileopenBtn, SIGNAL(clicked(bool)), this, SLOT(onFileOpenBtnClicked(bool)));
    //双击聊天窗口右边群好友列表
    connect(ui->treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));

    //如果是群聊
    if (m_isGroupTalk)      //群聊
    {
        initTalkWindow();   //初始化群聊
    }
    else    //不是群聊
    {
        initPtoPTalk();     //初始化单聊
    }
}

//初始化群聊状态
void TalkWindow::initGroupTalkStatus()
{
    QSqlQueryModel sqlDepModel;
    //查询群聊 ID 的 SQL 语句
    QString strSql = QString("SELECT * FROM tab_department WHERE departmentID = %1").arg(m_talkId);
    sqlDepModel.setQuery(strSql);   //执行查询

    int rows = sqlDepModel.rowCount();  //获取结果集行数
    //如果行数为 0
    if (rows == 0)  //单独聊天
    {
        m_isGroupTalk = false;
    }
    else    //不是 0 就是群聊
    {
        m_isGroupTalk = true;
    }
}

//获取公司ID(群聊号)
int TalkWindow::getCompDepID()
{
    QSqlQuery queryDepID(QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'").arg(QString::fromLocal8Bit("公司群")));
    queryDepID.exec();
    queryDepID.next();
    int companyID = queryDepID.value(0).toInt();

    return companyID;
}

//初始化群聊
void TalkWindow::initTalkWindow()
{
    QTreeWidgetItem *pRootItem = new QTreeWidgetItem();
    pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);

    //设置data，用于区分根项，子项
    pRootItem->setData(0, Qt::UserRole, 0);
    RootContatItem *pItemName = new RootContatItem(false, ui->treeWidget);

    ui->treeWidget->setFixedHeight(646);    //shell高度 - shell头高（talkwindow titleWidget)

    //当前聊天的群组名
    QString strGroupName;
    //查询群聊名称 SQL 语句
    QSqlQuery queryGroupName(QString("SELECT department_name FROM tab_department WHERE departmentID = %1").arg(m_talkId));
    queryGroupName.exec();  //执行查询
    //获取第一条结果集
    if (queryGroupName.next())
    {
        //取出结果 查询结果下标从 0 开始
        strGroupName = queryGroupName.value(0).toString();
    }

    QSqlQueryModel queryEmployeeModel;
    if (getCompDepID() == m_talkId.toInt())//公司群
    {
        //查询公司群所有群员ID
        queryEmployeeModel.setQuery("SELECT employeeID FROM tab_employees WHERE status = 1");
    }
    else
    {
        queryEmployeeModel.setQuery(QString("SELECT employeeID FROM tab_employees WHERE status = 1 AND departmentID = %1").arg(m_talkId));
    }
    int nEmployeeNum = queryEmployeeModel.rowCount();

    QString qsGroupName = QString::fromLocal8Bit("%1 %2/%3").arg(strGroupName).arg(0).arg(nEmployeeNum);    //%1是群聊名称 %2是在线人数 %3是群聊人数

    pItemName->setText(qsGroupName);

    //插入分组节点
    ui->treeWidget->addTopLevelItem(pRootItem);
    ui->treeWidget->setItemWidget(pRootItem, 0, pItemName);

    //展开
    pRootItem->setExpanded(true);

    for (int i = 0; i < nEmployeeNum; ++i)
    {
        QModelIndex modelIndex = queryEmployeeModel.index(i, 0);
        int employeeID = queryEmployeeModel.data(modelIndex).toInt();

        //if(employeeID == gLoginEmployeeID.toInt()) continue;    //如果是自己 就不添加进群员列表里

        //添加子节点
        addPeopInfo(pRootItem,employeeID);
    }
}

//初始化单聊
void TalkWindow::initPtoPTalk()
{
    QPixmap pixSkin;    //创建位图
    pixSkin.load(":/Resources/MainWindow/skin.png");    //加载位图

    ui->widget->setFixedSize(pixSkin.size());   //设置聊天窗口右边窗口大小为位图大小

    //创建标签 父窗口为聊天窗口右边窗口
    QLabel* skinLabel = new QLabel(ui->widget);
    skinLabel->setPixmap(pixSkin);  //设置标签图片
    skinLabel->setFixedSize(ui->widget->size());    //设置标签图片大小为窗口大小
}

//添加群员信息
void TalkWindow::addPeopInfo(QTreeWidgetItem * pRootGroupItem, int employeeID)
{
    //创建子项
    QTreeWidgetItem* pChild = new QTreeWidgetItem();
     //添加子节点
    pChild->setData(0, Qt::UserRole, 1);     //设置子项数据 第 0 列 将Qt::UserRole设置为 1(1代表子项 0代表根项)
    pChild->setData(0, Qt::UserRole + 1, employeeID);
    //创建联系人选项 父窗体为聊天窗口右边的树
    ContactItem* pContactItem = new ContactItem(ui->treeWidget);

    QPixmap pix1;    //位图
    pix1.load(":/Resources/MainWindow/head_mask.png");  //加载位图(空白圆头像)

    //获取名、签名、头像
    QString strName, strSign, strPicturePath;
    QSqlQueryModel queryInfoModel;
    queryInfoModel.setQuery(QString("SELECT employee_name,employee_sign,picture FROM tab_employees WHERE employeeID = %1").arg(employeeID));

    QModelIndex nameIndex, signIndex, pictureIndex;
    nameIndex = queryInfoModel.index(0, 0);//行，列
    signIndex = queryInfoModel.index(0, 1);
    pictureIndex = queryInfoModel.index(0, 2);

    strName = queryInfoModel.data(nameIndex).toString();
    strSign = queryInfoModel.data(signIndex).toString();
    strPicturePath = queryInfoModel.data(pictureIndex).toString();

    QImage imageHead;
    imageHead.load(strPicturePath);

    //设置头像、名称、签名
    pContactItem->setHeadPixmap(CommonUtils::getRoundImage(QPixmap::fromImage(imageHead), pix1, pContactItem->getHeadLabelSize()));
    pContactItem->setUserName(strName);
    pContactItem->setSignName(strSign);

    //给根选项添加子项
    pRootGroupItem->addChild(pChild);
    ui->treeWidget->setItemWidget(pChild, 0, pContactItem); //设置子项的部件的联系人选项

    //把子项添加到群员组map里
    QString str = pContactItem->getUserName();
    m_groupPeopleMap.insert(pChild, str);
}
