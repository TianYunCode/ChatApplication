#include "talkwindowshell.h"
#include "ui_talkwindowshell.h"
#include "emotionwindow.h"
#include "talkwindow.h"
#include "commonutils.h"
#include "talkwindowitem.h"
#include "windowmanager.h"
#include "receivefile.h"
#include "qmsgtextedit.h"

#include <qlistwidget.h>
#include <QSqlQueryModel>
#include <QMessageBox>
#include <QFile>
#include <QSqlQuery>

QString gfileName;	//文件名称
QString gfileData;	//文件内容

const int gUdpPort = 6666;

extern QString gLoginEmployeeID;
class TalkWindow;

TalkWindowShell::TalkWindowShell(QWidget *parent) : BasicWindow(parent), ui(new Ui::TalkWindowShell)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose); //设置窗口属性为窗口关闭时删除(回收)
    initControl();      //初始化控件
    initTcpSocket();    //初始化TCP套接字
    initUdpSocket();    //初始化UDP套接字

    QFile file(":/Resources/MainWindow/MsgHtml/msgtmpl.js");
    //如果js文件没有内容才去写
    if (!file.size())
    {
        QStringList employeesIDList;        //保存使用员工QQ号的链表
        getEmployeesID(employeesIDList);    //获取所有员工QQ号存储在 employeesIDList链表中
        if (!createJSFile(employeesIDList)) //创建(写)js文件
        {
            //创建(写)js文件失败 提示报错
            QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("更新js文件数据失败！"));
        }
    }
}

TalkWindowShell::~TalkWindowShell()
{
    delete ui;
    delete m_emotionWindow;
    m_emotionWindow = nullptr;
}

//添加新的聊天窗口
void TalkWindowShell::addTalkWindow(TalkWindow * talkWindow, TalkWindowItem * talkWindowItem, const QString& uid)
{
    ui->rightStackedWidget->addWidget(talkWindow);  //新窗口

    //当表情窗口隐藏 设置表情按钮状态
    connect(m_emotionWindow, SIGNAL(signalEmotionWindowHide()), talkWindow, SLOT(onSetEmotionBtnStatus()));

    //创建一个列表项 以主聊天窗口左边的窗口作为父窗体
    QListWidgetItem* aItem = new QListWidgetItem(ui->listWidget);
    //将列表项与之对应的聊天窗口关联
    m_talkwindowItemMap.insert(aItem, talkWindow);

    aItem->setSelected(true);   //设置当前聊天项 选中

    //判断是群聊还是单聊
    QSqlQueryModel sqlDepModel;
    QString strQuery = QString("SELECT picture FROM tab_department WHERE departmentID = %1").arg(uid);
    sqlDepModel.setQuery(strQuery);
    int rows = sqlDepModel.rowCount();  //获取结果集行数

    if (rows == 0)  //0 行是单聊
    {
        strQuery = QString("SELECT picture FROM tab_employees WHERE employeeID = %1").arg(uid);
        sqlDepModel.setQuery(strQuery);
    }

    QModelIndex index;
    index = sqlDepModel.index(0, 0);//行，列

    QImage img;
    img.load(sqlDepModel.data(index).toString());

    talkWindowItem->setHeadPixmap(QPixmap::fromImage(img));//设置头像
    ui->listWidget->addItem(aItem); //添加一个聊天列表近主聊天窗口左侧窗口
    ui->listWidget->setItemWidget(aItem, talkWindowItem);   //设置聊天列表项部件

    onTalkWindowItemClicked(aItem); //点击了聊天列表项

    //如果聊天列表项关闭了
    connect(talkWindowItem, &TalkWindowItem::signalCloseClicked, [talkWindowItem, talkWindow, aItem, this]()
    {
        m_talkwindowItemMap.remove(aItem);  //把聊天列表项从保存打开的聊天窗口map中删除
        talkWindow->close();        //把右边的聊天窗口关闭
        ui->listWidget->takeItem(ui->listWidget->row(aItem));//将主聊天窗口左边列表项移除
        delete talkWindowItem;      //释放聊天列表项
        ui->rightStackedWidget->removeWidget(talkWindow);   //释放主聊天窗口右边窗口
        if (ui->rightStackedWidget->count() < 1)    //如果删除了主聊天窗口右边窗口 就不存在打开的了 就关闭主聊天窗口
        {
            close();
        }
    });
}

//设置当前聊天窗口
void TalkWindowShell::setCurrentWidget(QWidget * widget)
{
    ui->rightStackedWidget->setCurrentWidget(widget);
}

//
const QMap<QListWidgetItem*, QWidget*>& TalkWindowShell::getTalkWindowItemMap() const
{
    return m_talkwindowItemMap;
}

//初始化控件
void TalkWindowShell::initControl()
{
    loadStyleSheet("TalkWindow");   //加载样式表(聊天窗口)
    setWindowTitle(QString::fromLocal8Bit("天云-聊天窗口"));  //设置窗口标题

    m_emotionWindow = new EmotionWindow;    //构造表情窗口
    m_emotionWindow->hide();                //隐藏表情窗口

    QList<int> leftWidgetSize;
    leftWidgetSize << 154 << width() - 154;
    ui->splitter->setSizes(leftWidgetSize);    //分类器设置尺寸

    ui->listWidget->setStyle(new CustomProxyStyle(this));   //给聊天窗口左边的聊天列表设置自定义风格(选中时不会(聚焦)出现边框)

    //当左边的聊天列表被点击 出现对应聊天窗口
    connect(ui->listWidget, &QListWidget::itemClicked, this, &TalkWindowShell::onTalkWindowItemClicked);
    //当表情窗口被点击 表情被选中
    connect(m_emotionWindow, SIGNAL(signalEmotionItemClicked(int)), this, SLOT(onEmotionItemClicked(int)));
}

//初始化TCP
void TalkWindowShell::initTcpSocket()
{
    m_tcpClientSocket = new QTcpSocket(this);
    m_tcpClientSocket->connectToHost("127.0.0.1", gtcpPort);
}

//初始化UDP
void TalkWindowShell::initUdpSocket()
{
    m_udpReceiver = new QUdpSocket(this);
    for (quint16 port = gUdpPort; port < gUdpPort + 200; ++port)
    {
        if (m_udpReceiver->bind(port, QUdpSocket::ShareAddress))    //绑定端口 模式为共享方式
        {
            break;  //端口绑定成功就终止循环
        }
    }

    //当 udp连接有数据到来 执行 processPendingData()函数处理UDP广播收到的数据
    connect(m_udpReceiver, &QUdpSocket::readyRead, this, &TalkWindowShell::processPendingData);
}

//获取所有员工QQ号
void TalkWindowShell::getEmployeesID(QStringList& employeesList)
{
    QSqlQueryModel queryModel;
    //查询员工ID 在员工表中 条件是 status = 1(意思是账号没有被注销)
    queryModel.setQuery("SELECT employeeID FROM tab_employees WHERE status = 1");

    //返回模型的总行数(员工的总数)
    int employeesNum = queryModel.rowCount();   //获取结果集的行数(一行一个员工信息 行数即员工总数)
    QModelIndex index;
    for (int i = 0; i < employeesNum; ++i)
    {
        //保存第 i 行 第 0 列的数据(员工QQ号)
        index = queryModel.index(i, 0); //行，列
        //将员工QQ号数据转成字符串插入链表
        employeesList << queryModel.data(index).toString();
    }
}

//创建(写)js文件
bool TalkWindowShell::createJSFile(QStringList & employeesList)
{
    //读取txt文件数据
    QString strFileTxt = ":/Resources/MainWindow/MsgHtml/msgtmpl.txt";
    QFile fileRead(strFileTxt); //操作 msgtmpl.txt
    QString strFile;

    //只读方式打开 msgtmpl.txt
    if (fileRead.open(QIODevice::ReadOnly))
    {
        //读取 msgtmpl.txt全部内容
        strFile = fileRead.readAll();
        //关闭 msgtmpl.txt文件
        fileRead.close();
    }
    else    //打开失败 提示出错
    {
        QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("读取 msgtmpl.txt 失败！"));

        return false;   //函数返回
    }

    //替换（external0，appendHtml0用作自己发信息使用）
    QFile fileWrite(":/Resources/MainWindow/MsgHtml/msgtmpl.js");   //操作 msgtmpl.js文件
    if (fileWrite.open(QIODevice::WriteOnly | QIODevice::Truncate)) //只写方式和覆盖方式打开文件
    {
        //更新空值
        QString strSourceInitNull = "var external = null;";

        //更新初始化
        QString strSourceInit = "external = channel.objects.external;";

        //更新newWebChannel
        QString strSourceNew =
            "new QWebChannel(qt.webChannelTransport,\
            function(channel) {\
            external = channel.objects.external;\
        }\
        ); \
        ";

        //更新追加recvHtml，脚本中有双引号无法直接进行赋值，采用读文件方式
        QString strSourceRecvHtml;
        QFile fileRecvHtml(":/Resources/MainWindow/MsgHtml/recvHtml.txt");
        if (fileRecvHtml.open(QIODevice::ReadOnly))
        {
            strSourceRecvHtml = fileRecvHtml.readAll();
            fileRecvHtml.close();
        }
        else
        {
            QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("读取 recvHtml.txt 失败！"));

            return false;
        }

        //保存替换后的脚本
        QString strReplaceInitNull;
        QString strReplaceInit;
        QString strReplaceNew;
        QString strReplaceRecvHtml;

        for (int i = 0; i < employeesList.length(); i++)
        {
            //编辑替换后的空值
            QString strInitNull = strSourceInitNull;
            strInitNull.replace("external", QString("external_%1").arg(employeesList.at(i)));
            strReplaceInitNull += strInitNull;
            strReplaceInitNull += "\n";

            //编辑替换后的初始值
            QString strInit = strSourceInit;
            strInit.replace("external", QString("external_%1").arg(employeesList.at(i)));
            strReplaceInit += strInit;
            strReplaceInit += "\n";

            //编辑替换后的 newWebChannel
            QString strNew = strSourceNew;
            strNew.replace("external", QString("external_%1").arg(employeesList.at(i)));
            strReplaceNew += strNew;
            strReplaceNew += "\n";

            //编辑替换后的 recvHtml
            QString strRecvHtml = strSourceRecvHtml;
            strRecvHtml.replace("external", QString("external_%1").arg(employeesList.at(i)));
            strRecvHtml.replace("recvHtml", QString("recvHtml_%1").arg(employeesList.at(i)));
            strReplaceRecvHtml += strRecvHtml;
            strReplaceRecvHtml += "\n";
        }

        strFile.replace(strSourceInitNull, strReplaceInitNull);
        strFile.replace(strSourceInit, strReplaceInit);
        strFile.replace(strSourceNew, strReplaceNew);
        strFile.replace(strSourceRecvHtml, strReplaceRecvHtml);

        QTextStream stream(&fileWrite);
        stream << strFile;
        fileWrite.close();

        return true;
    }
    else    //打开 msgtmpl.js文件失败
    {
        QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("写 msgtmpl.js 失败！"));

        return false;
    }
}

//处理收到的信息(文本和表情 不包括文件)
void TalkWindowShell::handleReceivedMsg(int senderEmployeeID, int msgType, QString strMsg)
{
    QMsgTextEdit msgTextEdit;
    msgTextEdit.setText(strMsg);

    if (msgType == 1)       //文本信息
    {
        msgTextEdit.document()->toHtml();   //将文本信息转换成 html
    }
    else if (msgType == 0)  //表情信息
    {
        const int emotionWidth = 3; //表情宽度
        int emotionNum = strMsg.length() / emotionWidth;

        for (int i = 0; i < emotionNum; i++)
        {
            msgTextEdit.addEmotionUrl(strMsg.mid(i * emotionWidth, emotionWidth).toInt());
        }
    }

    QString html = msgTextEdit.document()->toHtml();

    //文本html如果没有字体则添加字体
    if (!html.contains(".png") && !html.contains("</span>"))
    {
        QString fontHtml;
        QFile file(":/Resources/MainWindow/MsgHtml/msgFont.txt");
        if (file.open(QIODevice::ReadOnly))
        {
            fontHtml = file.readAll();
            fontHtml.replace("%1", strMsg);
            file.close();
        }
        else
        {
            QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("文件 msgFont.txt 不存在！"));
            return;
        }

        if (!html.contains(fontHtml))
        {
            html.replace(strMsg, fontHtml);
        }
    }

    TalkWindow* talkWindow = dynamic_cast<TalkWindow*>(ui->rightStackedWidget->currentWidget());
    talkWindow->ui->msgWidget->appendMsg(html, QString::number(senderEmployeeID));
}

//表情按钮点击后执行的槽函数
void TalkWindowShell::onEmotionBtnClicked(bool)
{
    //设置表情窗口不可见
    m_emotionWindow->setVisible(!m_emotionWindow->isVisible());
    //将当前控件的相对位置转换为屏幕的绝对位置
    QPoint emotionPoint = this->mapToGlobal(QPoint(0, 0));

    //将坐标偏移
    emotionPoint.setX(emotionPoint.x() + 170);
    emotionPoint.setY(emotionPoint.y() + 220);
    //设置表情窗口坐标
    m_emotionWindow->move(emotionPoint);
}

//文本数据包格式：群聊标志 + 发信息员工QQ号 + 收信息员工QQ号(群QQ号) + 信息类型 + 数据长度 + 数据
//表情数据包格式：群聊标志 + 发信息员工QQ号 + 收信息员工QQ号(群QQ号) + 信息类型 + 表情个数 + images + 数据
//msgType 0表情信息 1文本信息 2文件信息
//客户端发送Tcp数据（数据，数据类型，文件）
void TalkWindowShell::updateSendTcpMsg(QString & strData, int & msgType, QString fileName)
{
    //获取当前活动聊天窗口
    TalkWindow* curTalkWindow = dynamic_cast<TalkWindow*>(ui->rightStackedWidget->currentWidget());
    QString talkId = curTalkWindow->getTalkId();

    QString strGroupFlag;   //群聊标志
    QString strSend;

    if (talkId.length() == 4)   //群QQ的长度
    {
        strGroupFlag = "1";
    }
    else
    {
        strGroupFlag = "0";
    }

    int nstrDataLength = strData.length();
    int dataLength = QString::number(nstrDataLength).length();
    QString strdataLength;

    if (msgType == 1)//发送文本信息
    {
        //文本信息(数据长度)的长度约定为5位
        if (dataLength == 1)
        {
            strdataLength = "0000" + QString::number(nstrDataLength);
        }
        else if(dataLength == 2)
        {
            strdataLength = "000" + QString::number(nstrDataLength);
        }
        else if (dataLength == 3)
        {
            strdataLength = "00" + QString::number(nstrDataLength);
        }
        else if (dataLength == 4)
        {
            strdataLength = "0" + QString::number(nstrDataLength);
        }
        else if (dataLength == 5)
        {
            strdataLength = QString::number(nstrDataLength);
        }
        else
        {
            QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("不合理的数据长度！"));
        }

        //文本数据包格式：群聊标志 + 发信息员工QQ号 + 收信息员工QQ号(群QQ号) + 信息类型(1) + 数据长度 + 数据
        strSend = strGroupFlag + gLoginEmployeeID + talkId + "1" + strdataLength + strData;
    }
    else if (msgType == 0)//表情信息
    {
        //表情数据包格式：群聊标志 + 发信息员工QQ号 + 收信息员工QQ号(群QQ号) + 信息类型(0) + 表情个数 + images + 数据
        strSend = strGroupFlag + gLoginEmployeeID + talkId + "0" + strData;
    }
    else if (msgType == 2)//文件
    {
        //文件数据包格式：群聊标志 + 发信息员工QQ号 + 收信息员工QQ号(群QQ号) + 信息类型(2) + 文件长度 + "bytes" + 文件名称 + "data_begin" + 文件内容

        QByteArray bt = strData.toUtf8();
        QString strLength = QString::number(bt.length());

        strSend = strGroupFlag + gLoginEmployeeID + talkId + "2" + strLength + "bytes" + fileName + "data_begin" + strData;
    }

    QByteArray dataBt;
    dataBt.resize(strSend.length());
    dataBt = strSend.toUtf8();
    m_tcpClientSocket->write(dataBt);   //发送数据
}

//左侧列表点击后执行的槽函数
void TalkWindowShell::onTalkWindowItemClicked(QListWidgetItem * item)
{
    //获取聊天窗口 键对应的值
    QWidget* talkwindowWidget = m_talkwindowItemMap.find(item).value();
    //设置当前聊天窗口(右边窗口)
    ui->rightStackedWidget->setCurrentWidget(talkwindowWidget);
}

//表情被选中
void TalkWindowShell::onEmotionItemClicked(int emotionNum)
{
    //获取当前聊天窗口
    TalkWindow* curTalkWindow = dynamic_cast<TalkWindow*>(ui->rightStackedWidget->currentWidget());
    if (curTalkWindow)  //判断是否获取成功 不是空指针就是获取成功
    {
        curTalkWindow->addEmotionImage(emotionNum); //给当前聊天窗口的对话编辑框添加选中的表情
    }
}

/*****************************************************************************************************************************
    数据包格式
    文本数据包格式：群聊标志+发信息员工QQ号+收信息员工QQ号（群QQ号）+信息类型（1）+数据长度+数据
    表情数据包格式：群聊标志+发信息员工QQ号+收信息员工QQ号（群QQ号）+信息类型（0）+表情个数+images+表情名称
    文件数据包格式：群聊标志+发信息员工QQ号+收信息员工QQ号（群QQ号）+信息类型（2）+文件字节数+bytes+文件名+data_begin+文件数据

    群聊标志占1位，0表示单聊，1表示群聊
    信息类型占1位，0表示表情信息，1表示文本信息，2表示文件信息

    QQ号占5位，群QQ号占4位，数据长度占5位，表情名称占3位
    （注意：当群聊标志为1时，则数据包没有收信息员工QQ号，而是收信息群QQ号，当群聊标志为0时，则数据包没有收信息群QQ号，而是收信息员工QQ号)

    群聊文本信息如：1100012001100005Hello  表示QQ10001向群2001发送文本信息，长度是5，数据为Hello
    单聊图片信息如：0100011000201images060 表示QQ10001向QQ10002发送1个表情60.png
    群聊文件信息如：1100052000210bytestest.txtdata_beginhelloworld 表示QQ10005向群2000发送文件信息，文件是test.txt，文件内容长度10，内容helloworld

    群聊文件信息解析：1 10001 2001 1 00005 Hello
    单聊图片信息解析：0 10001 10002 0 060
    群聊文件信息解析：1 10005 2000 2 10 bytes test.txt data_begin helloword

******************************************************************************************************************************/
//处理UDP广播收到的数据
void TalkWindowShell::processPendingData()
{
    //端口中有未处理的数据
    while (m_udpReceiver->hasPendingDatagrams())
    {
        const static int groupFlagWidth = 1;	//群聊标志占位
        const static int groupWidth = 4;		//群QQ号宽度
        const static int employeeWidth = 5;		//员工QQ号宽度
        const static int msgTypeWidth = 1;		//信息类型宽度
        const static int msgLengthWidth = 5;	//文本信息长度的宽度
        const static int pictureWidth = 3;		//表情图片的宽度

        //读取udp数据
        QByteArray btData;
        btData.resize(m_udpReceiver->pendingDatagramSize());    //获取未处理的数据大小 设置为 btData的大小
        m_udpReceiver->readDatagram(btData.data(), btData.size());  //读取数据包 存放在 btData里

        QString strData = btData.data();    //取出数据
        QString strWindowID;    //聊天窗口ID 群聊则是群号 单聊则是员工QQ号
        QString strSendEmployeeID, strRecevieEmployeeID;//发送及接收端的QQ号
        QString strMsg;         //数据

        int msgLen;     //数据长度
        int msgType;    //数据类型

        strSendEmployeeID = strData.mid(groupFlagWidth, employeeWidth); //从数据中取出员工QQ号

        //自己发的信息不做处理 因为发送 udp广播自己也会收到
        if (strSendEmployeeID == gLoginEmployeeID)
        {
            return;
        }

        if (btData[0] == '1')//群聊
        {
            //从数据中取出群QQ号
            strWindowID = strData.mid(groupFlagWidth + employeeWidth, groupWidth);

            //从数据中取出消息类型    1表示文本信息 0表示表情信息 2表示文件信息
            QChar cMsgType = QChar::fromLatin1(btData[groupFlagWidth + employeeWidth + groupWidth]);
            //判断消息类型
            if (cMsgType == '1')        //文本信息
            {
                msgType = 1;
                //从数据中取出数据长度
                msgLen = strData.mid(groupFlagWidth + employeeWidth + groupWidth + msgTypeWidth, msgLengthWidth).toInt();
                //从数据中取出文本信息
                strMsg = strData.mid(groupFlagWidth + employeeWidth + groupWidth + msgType + msgLengthWidth, msgLen);
            }
            else if (cMsgType == '0')   //表情信息
            {
                msgType = 0;
                int posImages = strData.indexOf("images");
                //截取右边数据 从数据总长度 - "images"之前长度 - "images"自身长度
                //例：0100011000201images060 总长度为22 indexOf("images")为13 images长度为6 那么截取剩下的就是 060
                strMsg = strData.right(strData.length() - posImages - QString("images").length());
            }
            else if (cMsgType == '2')   //文件信息
            {
                msgType = 2;
                int bytesWidth = QString("bytes").length();         //bytes标志的长度 5
                int posBytes = strData.indexOf("bytes");            //bytes在数据中的索引位置
                int posData_begin = strData.indexOf("data_begin");  //data_begin在数据中的索引位置

                //从数据中取出文件名称
                QString fileName = strData.mid(posBytes + bytesWidth, posData_begin - posBytes - bytesWidth);
                gfileName = fileName;

                //从数据中取出文件内容
                int dataLengthWidth;
                //data_begin在数据中的索引位置 + data_begin的长度就是文件内容开始的索引位置
                int posData = posData_begin + QString("data_begin").length();
                strMsg = strData.mid(posData);  //截取从 posData开始到数据结尾位置 就是文件内容
                gfileData = strMsg;

                //根据employeeID获取发送者姓名
                QString sender;
                int employeeID = strSendEmployeeID.toInt();
                QSqlQuery querySenderName(QString("SELECT employee_name FROM tab_employees WHERE employeeID = %1").arg(employeeID));
                querySenderName.exec();

                if (querySenderName.first())
                {
                    sender = querySenderName.value(0).toString();
                }

                ReceiveFile *recvFile = new ReceiveFile(this);
                connect(recvFile, &ReceiveFile::refuseFile, [this]()
                {
                    return;
                });
                QString msgLabel = QString::fromLocal8Bit("收到来自") + sender + QString::fromLocal8Bit("发来的文件，是否接收？");
                recvFile->setMsg(msgLabel);
                recvFile->show();
            }
        }
        else//单聊
        {
            //从数据中取出发送者QQ号
            strRecevieEmployeeID = strData.mid(groupFlagWidth + employeeWidth, employeeWidth);
            strWindowID = strSendEmployeeID;

            //不是发给我的信息不做处理
            if (strRecevieEmployeeID != gLoginEmployeeID)
            {
                return;
            }

            //获取信息的类型
            QChar cMsgType = QChar::fromLatin1(btData[groupFlagWidth + employeeWidth + employeeWidth]);
            if (cMsgType == '1')        //文本信息
            {
                msgType = 1;

                //从数据中取出文本信息长度
                msgLen = strData.mid(groupFlagWidth + employeeWidth + employeeWidth + msgTypeWidth, msgLengthWidth).toInt();

                //从数据中取出文本信息
                strMsg = strData.mid(groupFlagWidth + employeeWidth + employeeWidth + msgTypeWidth + msgLengthWidth, msgLen);
            }
            else if (cMsgType == '0')   //表情信息
            {
                msgType = 0;

                int posImages = strData.indexOf("images");      //获得 images标志的索引位置
                int imagesWidth = QString("images").length();   //获得 images标志的长度
                //例：0100011000201images060 得到 060
                strMsg = strData.mid(posImages + imagesWidth);  //获得 images标志后面的数据
            }
            else if (cMsgType == '2')   //文件信息
            {
                msgType = 2;

                int bytesWidth = QString("bytes").length();             //获得 bytes标志的长度
                int posBytes = strData.indexOf("bytes");                //获得 bytes标志的索引位置
                int data_beginWidth = QString("data_begin").length();   //获得 data_begin标志的长度
                int posData_begin = strData.indexOf("data_begin");      //获取 data_begin标志的索引位置

                //从数据中取出文件名称
                QString fileName = strData.mid(posBytes + bytesWidth, posData_begin - posBytes - bytesWidth);
                gfileName = fileName;

                //从数据中取出文件内容
                strMsg = strData.mid(posData_begin + data_beginWidth);
                gfileData = strMsg;

                //根据employeeID获取发送者姓名
                QString sender;
                int employeeID = strSendEmployeeID.toInt(); //发送者QQ号
                QSqlQuery querySenderName(QString("SELECT employee_name FROM tab_employees WHERE employeeID = %1").arg(employeeID));
                querySenderName.exec();

                if (querySenderName.first())
                {
                    sender = querySenderName.value(0).toString();
                }

                ReceiveFile *recvFile = new ReceiveFile(this);
                connect(recvFile, &ReceiveFile::refuseFile, [this]()
                {
                    return;
                });
                QString msgLabel = QString::fromLocal8Bit("收到来自") + sender + QString::fromLocal8Bit("发来的文件，是否接收？");
                recvFile->setMsg(msgLabel);
                recvFile->show();
            }
        }

        //将聊天窗口设为活动的窗口
        QWidget* widget = WindowManager::getInstance()->findWindowName(strWindowID);
        if (widget)//聊天窗口存在
        {
            this->setCurrentWidget(widget);

            //同步激活左侧聊天窗口
            QListWidgetItem* item = m_talkwindowItemMap.key(widget);
            item->setSelected(true);
        }
        else//聊天窗口未打开
        {
            return;
        }

        //文件信息另作处理
        if (msgType != 2)   //不处理文件
        {
            int sendEmployeeID = strSendEmployeeID.toInt();
            handleReceivedMsg(sendEmployeeID, msgType, strMsg); //处理收到的信息(文本和表情 不包括文件)
        }

    }
}
