#include "notifymanager.h"
#include "commonutils.h"

NotifyManager* NotifyManager::instance = nullptr;   //初始化实例 静态成员变量只能在类外初始化

NotifyManager::NotifyManager() : QObject(nullptr)
{
}

NotifyManager::~NotifyManager()
{
}

//单例模式 获取实例
NotifyManager* NotifyManager::getInstance()
{
    if (instance == nullptr)    //如果实例为空
    {
        instance = new NotifyManager(); //创建实例
    }

    return instance;    //返回实例
}

//通知其他窗口更改皮肤
void NotifyManager::notifyOtherWindowChangeSkin(const QColor& color)
{
    emit signalSkinChanged(color);              //发送皮肤改变信号
    CommonUtils::setDefaultSkinColor(color);    //设置默认皮肤颜色
}
