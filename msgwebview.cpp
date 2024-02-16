#include "msgwebview.h"
#include "talkwindowshell.h"
#include "windowmanager.h"

#include <QFile>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QWebChannel>
#include <QSqlQueryModel>

extern QString gstrLoginHeadPath;

MsgHtmlObj::MsgHtmlObj(QObject* parent, QString msgLPicPath) :QObject(parent)
{
    m_msgLPicPath = msgLPicPath;
    initHtmlTmpl(); //初始化网页
}

//初始化聊天网页
void MsgHtmlObj::initHtmlTmpl()
{
    //加载 msgleftTmpl.html
    m_msgLHtmlTmpl = getMsgTmplHtml("msgleftTmpl");
    m_msgLHtmlTmpl.replace("%1", m_msgLPicPath);

    //加载 msgrightTmpl.html
    m_msgRHtmlTmpl = getMsgTmplHtml("msgrightTmpl");
    m_msgRHtmlTmpl.replace("%1", gstrLoginHeadPath);
}

//从资源文件加载网页
QString MsgHtmlObj::getMsgTmplHtml(const QString& code)
{
    //获取到对应的 html文件
    QFile file(":/Resources/MainWindow/MsgHtml/" + code + ".html");
    file.open(QFile::ReadOnly); //读文件方式打开文件
    QString strData;    //保存 html变量
    if (file.isOpen())  //判断文件是否打开成功
    {
        //打开成功 读取文件全部内容保存到 html变量中
        strData = QLatin1String(file.readAll());
    }
    else    //文件打开失败
    {
        QMessageBox::information(nullptr, "Tips", "Failed to init html!");
        return QString("");
    }
    file.close();   //关闭文件

    return strData; //返回 html变量
}

//重写接受导航请求函数
bool MsgWebPage::acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame)
{
    //仅接受qrc:/*.html
    if (url.scheme() == QString("qrc")) //判断url类型
    {
        qDebug()<<"11111";
        return true;
    }

    return false;
}

MsgWebView::MsgWebView(QWidget *parent) : QWebEngineView(parent), m_channel(new QWebChannel(this))
{
    MsgWebPage* page = new MsgWebPage(this);    //创建一个网页
    setPage(page);  //将 page网页设置为当前网页

    m_msgHtmlObj = new MsgHtmlObj(this);
    m_channel->registerObject("external0", m_msgHtmlObj);  //注册对象(ID,对象) ID已在 msgtmpl.js文件里注册

    TalkWindowShell* talkWindowShell = WindowManager::getInstance()->getTalkWindowShell();
    //当 signalSendMsg发射 执行 signalSendMsg发送数据(文本消息、表情消息、文件)
    connect(this, &MsgWebView::signalSendMsg, talkWindowShell, &TalkWindowShell::updateSendTcpMsg);

    //当前正构建的聊天窗口的ID（QQ号）
    QString strTalkId = WindowManager::getInstance()->getCreatingTalkId();

    QSqlQueryModel queryEmpolyeeModel;
    QString strEmployeeID, strPicturePath;
    QString strExternal;
    bool isGroupTalk = false;

    //获取公司群ID
    queryEmpolyeeModel.setQuery(QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'").arg(QStringLiteral("公司群")));
    QModelIndex companyIndex = queryEmpolyeeModel.index(0, 0);
    QString strCompanyID = queryEmpolyeeModel.data(companyIndex).toString();

    if (strTalkId == strCompanyID)      //公司群聊
    {
        isGroupTalk = true;
        queryEmpolyeeModel.setQuery("SELECT employeeID,picture FROM tab_employees WHERE status = 1");
    }
    else
    {
        if (strTalkId.length() == 4)    //其他群聊
        {
            isGroupTalk = true;
            queryEmpolyeeModel.setQuery(QString("SELECT employeeID,picture FROM tab_employees WHERE status = 1 AND departmentID = %1").arg(strTalkId));
        }
        else//单独聊天
        {
            queryEmpolyeeModel.setQuery(QString("SELECT picture FROM tab_employees WHERE status = 1 AND employeeID = %1").arg(strTalkId));

            QModelIndex index = queryEmpolyeeModel.index(0, 0);
            strPicturePath = queryEmpolyeeModel.data(index).toString();

            //构建网页对象
            strExternal = "external_" + strTalkId;
            MsgHtmlObj *msgHtmlObj = new MsgHtmlObj(this, strPicturePath);
            m_channel->registerObject(strExternal, msgHtmlObj);
        }
    }

    if (isGroupTalk)    //判断是否是群聊
    {
        QModelIndex employeeModelIndex, pictureModelIndex;
        int rows = queryEmpolyeeModel.rowCount();
        for (int i = 0; i < rows; i++)
        {
            employeeModelIndex = queryEmpolyeeModel.index(i, 0);
            pictureModelIndex = queryEmpolyeeModel.index(i, 1);

            strEmployeeID = queryEmpolyeeModel.data(employeeModelIndex).toString();
            strPicturePath = queryEmpolyeeModel.data(pictureModelIndex).toString();

            strExternal = "external_" + strEmployeeID;

            MsgHtmlObj* msgHtmlObj = new MsgHtmlObj(this, strPicturePath);
            m_channel->registerObject(strExternal, msgHtmlObj);
        }
    }

    this->page()->setWebChannel(m_channel); //设置当前网页的通道为 m_channel
    //初始化收信息页面
    this->load(QUrl("qrc:/Resources/MainWindow/MsgHtml/msgTmpl.html"));
}

MsgWebView::~MsgWebView()
{
    if(m_msgHtmlObj)
    {
        delete m_msgHtmlObj;
        m_msgHtmlObj = nullptr;
    }
}

//追加 html字符串
void MsgWebView::appendMsg(const QString & html, QString strObj)
{
    QJsonObject msgObj;
    QString qsMsg;
    const QList<QStringList> msgLst = parseHtml(html);  //解析html

    int imageNum = 0;           //发送表情的数量
    int msgType = 1;            //信息类型：0是表情 1文本 2文件
    bool isImageMsg = false;    //是否是图片信息
    QString strData;            //发送的数据

    for (int i = 0; i < msgLst.size(); i++)
    {
        if (msgLst.at(i).at(0) == "img")    //如果链表第一个元素是 img图片
        {
            QString imagePath = msgLst.at(i).at(1); //获取并保存图片路径
            QPixmap pixmap;     //创建图片变量

            //获取表情名称的位置
            QString strEmotionPath = "qrc:/Resources/MainWindow/emotion/";
            int pos = strEmotionPath.size();    //表情文件名前缀的长度
            isImageMsg = true;

            //获取表情名称
            QString strEmotionName = imagePath.mid(pos);    //截取表情路径 从qrc:/Resources/MainWindow/emotion/开始截取 只要文件名
            strEmotionName.replace(".png", "");     //去掉文件名的后缀 将.png替换成空

            //根据表情名称的长度进行设置表情数据
            //不足3位则补足3位,如23则数据为023
            int emotionNameL = strEmotionName.length(); //获取表情名称的长度
            if (emotionNameL == 1)  //如果长度是 1 就在表情名称前面拼接 00
            {
                strData = strData + "00" + strEmotionName;
            }
            else if (emotionNameL == 2) //如果长度是 2 就在表情名称前面拼接 0
            {
                strData = strData + "0" + strEmotionName;
            }
            else if (emotionNameL == 3) //如果长度是 2 就不用拼接 直接使用
            {
                strData = strData + strEmotionName;
            }

            msgType = 0;    //信息类型改成表情信息
            imageNum++;     //每次循环进入到这里 就代表表情个数加 1

            if (imagePath.left(3) == "qrc") //如果路径前面 3 个字符是 qrc
            {
                pixmap.load(imagePath.mid(3));//去掉表情路径中qrc
            }
            else         //不是 qrc
            {
                pixmap.load(imagePath);      //直接加载到图片变量
            }

            //表情图片html格式文本组合
            QString imgPath = QString("<img src=\"%1\" width=\"%2\" height=\"%3\"/>").arg(imagePath).arg(pixmap.width()).arg(pixmap.height());
            qsMsg += imgPath;
        }   //不是 img图片 再判断是不是文本
        else if (msgLst.at(i).at(0) == "text")
        {
            qsMsg += msgLst.at(i).at(1);
            strData = qsMsg;
        }
    }

    msgObj.insert("MSG", qsMsg);    //插入资源文件

    //json对象转换成json文档 再转换成UTF-8编码的文档 QJsonDocument::Compact表示文档中紧凑的方式
    const QString& Msg = QJsonDocument(msgObj).toJson(QJsonDocument::Compact);
    if (strObj == "0")  //发信息
    {
        this->page()->runJavaScript(QString("appendHtml0(%1)").arg(Msg));   //获取当前网页 运行 Msg

        if (isImageMsg) //如果是图片信息
        {
            strData = QString::number(imageNum) + "images" + strData;
        }

        emit signalSendMsg(strData, msgType);       //发射数据信号(数据，数据类型)
    }
    else        //来信
    {
        this->page()->runJavaScript(QString("recvHtml_%1(%2)").arg(strObj).arg(Msg));   //获取当前网页 运行 Msg
    }
}

//解析html Qt中所有DOM节点（属性、说明、文本等）都可以使用QDomNode表示
QList<QStringList> MsgWebView::parseHtml(const QString & html)
{
    QDomDocument doc;       //创建DOM文本
    doc.setContent(html);   //设置内容为 html文本
    const QDomElement& root = doc.documentElement();        //节点元素
    const QDomNode& node = root.firstChildElement("body");  //获取节点第一个子元素

    return parseDocNode(node);  //返回解析后的 node节点
}

//解析节点 Qt中所有DOM节点（属性、说明、文本等）都可以使用QDomNode表示
QList<QStringList> MsgWebView::parseDocNode(const QDomNode & node)
{
    QList<QStringList> attribute;
    //节点链表
    const QDomNodeList& list = node.childNodes();   //返回所有子节点

    for (int i = 0; i < list.count(); i++)
    {
        //创建一个节点保存当前循环的链表节点
        const QDomNode& node = list.at(i);

        if (node.isElement())   //如果节点是元素
        {
            //转换元素 创建一个元素 保存节点的元素
            const QDomElement& element = node.toElement();
            if (element.tagName() == "img")//判断元素的标签名是不是"img"
            {
                QStringList attributeList;  //创建字符串链表
                attributeList << "img" << element.attribute("src");
                attribute << attributeList;
            }

            if (element.tagName() == "span")//判断元素的标签名是不是"span"
            {
                QStringList attributeList;
                attributeList << "text" << element.text();
                attribute << attributeList;
            }

            if (node.hasChildNodes())//如果这个节点还有子节点
            {
                attribute << parseDocNode(node);//再进行解析节点
            }
        }
    }

    return attribute;
}
