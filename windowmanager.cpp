#include "windowmanager.h"
#include "talkwindow.h"
#include "talkwindowitem.h"
#include <QSqlQueryModel>
#include <QListWidgetItem>

//单例模式 创建全局静态对象
Q_GLOBAL_STATIC(WindowManager,theInstance)

WindowManager::WindowManager() :QObject(nullptr), m_talkwindowshell(nullptr)
{
}

WindowManager::~WindowManager()
{
}

//查找聊天窗口名称
QWidget * WindowManager::findWindowName(const QString & qsWindowName)
{
    //查找 map 的键里是否包含该窗体名称
    if (m_windowMap.contains(qsWindowName))
    {
        //返回键为 qsWindowName 的窗体
        return m_windowMap.value(qsWindowName);
    }

    //如果不包含 返回空指针
    return nullptr;
}

//删除聊天窗口名称
void WindowManager::deleteWindowName(const QString & qsWindowName)
{
    m_windowMap.remove(qsWindowName);
}

//添加聊天窗口名称(不新建)
void WindowManager::addWindowName(const QString & qsWindowName, QWidget * qWidget)
{
    //如果 map 里不包含名为 qsWindowName 的键
    if (!m_windowMap.contains(qsWindowName))
    {
        //插入 map
        m_windowMap.insert(qsWindowName, qWidget);
    }
}

//单例模式 返回实例
WindowManager * WindowManager::getInstance()
{
    return theInstance();
}

//返回主聊天窗口成员变量
TalkWindowShell * WindowManager::getTalkWindowShell()
{
    return m_talkwindowshell;
}

//获取正在创建的聊天窗口ID
QString WindowManager::getCreatingTalkId()
{
    return m_strCreatingTalkId;
}

//添加新聊天窗口(会新建)
void WindowManager::addNewTalkWindow(const QString & uid)
{
    //在添加新聊天窗口之前 需要保证父窗口已经存在
    if (m_talkwindowshell == nullptr)
    {

        m_talkwindowshell = new TalkWindowShell();    //新建父窗口(主聊天窗口)
        //如果父窗口销毁 将父窗口指针至空
        connect(m_talkwindowshell, &TalkWindowShell::destroyed, [this](QObject* obj)
        {
            m_talkwindowshell = nullptr;
        });
    }

    //查找 map 里是否存在该键 不存在返回 nullptr
    QWidget* widget = findWindowName(uid);
    if (!widget)    //uid 不存在
    {
        //创建聊天窗口 关联主聊天窗口
        TalkWindow* talkwindow = new TalkWindow(m_talkwindowshell, uid);
        //创建聊天选项卡 关联聊天窗口
        TalkWindowItem* talkwindowItem = new TalkWindowItem(talkwindow);

        //判断是群聊还是单聊
        QSqlQueryModel sqlDepModel; //模型
        QString strSql = QString("SELECT department_name,sign FROM tab_department WHERE departmentID = %1").arg(uid);
        sqlDepModel.setQuery(strSql);   //给模型设置查询语句
        int rows = sqlDepModel.rowCount();  //查找结果 获取结果集一共有几行

        QString strWindowName, strMsgLabel;

        if (rows == 0)//如果是 0 行 就是单聊 否则就是群聊
        {
            //获取单聊的对象名称、签名
            QString sql = QString("SELECT employee_name,employee_sign FROM tab_employees WHERE employeeID = %1").arg(uid);
            sqlDepModel.setQuery(sql);
        }

        //部门索引、签名索引
        QModelIndex indexDepIndex, signIndex;
        indexDepIndex = sqlDepModel.index(0, 0);	//0行0列
        signIndex = sqlDepModel.index(0, 1);		//0行1列
        strWindowName = sqlDepModel.data(signIndex).toString();
        strMsgLabel = sqlDepModel.data(indexDepIndex).toString();

        talkwindow->setWindowName(strWindowName);//窗口名称
        talkwindowItem->setMsgLabelContent(strMsgLabel);//左侧联系人文本显示
        m_talkwindowshell->addTalkWindow(talkwindow, talkwindowItem, uid);
    }
    else    //uid 存在
    {
        //设置左边聊天选项卡为点击的好友(群聊)对应的聊天选项卡(选中状态)
        QListWidgetItem* item = m_talkwindowshell->getTalkWindowItemMap().key(widget);
        item->setSelected(true);    //设置选中

        //设置右侧聊天窗口为点击的好友(群聊)对应的聊天窗口
        m_talkwindowshell->setCurrentWidget(widget);
    }

    m_talkwindowshell->show();              //显示
    m_talkwindowshell->activateWindow();    //激活(设为活动窗口)
}
