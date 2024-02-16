#ifndef MSGWEBVIEW_H
#define MSGWEBVIEW_H

#include <QWebEngineView>
#include <QDomNode>

//网页对象类
class MsgHtmlObj :public QObject
{
    Q_OBJECT

    //动态属性 msgLHtmlTmpl表示左边发来的信息 msgRHtmlTmpl表示右边的信息(自己发出去的信息)
    Q_PROPERTY(QString msgLHtmlTmpl MEMBER m_msgLHtmlTmpl NOTIFY signalMsgHtml)
    Q_PROPERTY(QString msgRHtmlTmpl MEMBER m_msgRHtmlTmpl NOTIFY signalMsgHtml)

public:
    MsgHtmlObj(QObject* parent,QString msgLPicPath = "");   //发信息来的人的头像路径

signals:
    void signalMsgHtml(const QString& html);        //

private:
    void initHtmlTmpl();    //初始化聊天网页
    QString getMsgTmplHtml(const QString& code);    //从资源文件加载网页

private:
    QString m_msgLPicPath;
    QString m_msgLHtmlTmpl;//别人发来的信息
    QString m_msgRHtmlTmpl;//我发的信息
};

//网页类
class MsgWebPage :public QWebEnginePage
{
    Q_OBJECT

public:
    MsgWebPage(QObject* parent = nullptr) :QWebEnginePage(parent) {}

protected:
    //重写接受导航请求函数
    bool acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame);
};

//自定义网页视图类 QWebEngineView视图用于显示网页内容
class MsgWebView : public QWebEngineView
{
    Q_OBJECT

public:
    MsgWebView(QWidget *parent);
    ~MsgWebView();
    void appendMsg(const QString& html,QString strObj = "0");    //追加 html字符串

private:
    QList<QStringList> parseHtml(const QString& html);      //解析html
    //Qt中所有DOM节点（属性、说明、文本等）都可以使用QDomNode表示
    QList<QStringList> parseDocNode(const QDomNode& node);  //解析节点

signals:
    void signalSendMsg(QString& strData, int &msgType, QString sFile = ""); //发送数据信号(数据，数据类型，文件路径)

private:
    MsgHtmlObj* m_msgHtmlObj;   //网页对象
    QWebChannel* m_channel;     //通道
};

#endif // MSGWEBVIEW_H
