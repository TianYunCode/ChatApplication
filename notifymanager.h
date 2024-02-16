#ifndef NOTIFYMANAGER_H
#define NOTIFYMANAGER_H

#include <QObject>

//通知管理类 主要用于通知其他窗口更改皮肤
class NotifyManager : public QObject
{
    Q_OBJECT

public:
    NotifyManager();
    ~NotifyManager();

signals:
    void signalSkinChanged(const QColor& color);    //皮肤改变信号

public:
    static NotifyManager* getInstance();            //单例模式 获取实例

    void notifyOtherWindowChangeSkin(const QColor& color);  //通知其他窗口更改皮肤

private:
    static NotifyManager* instance;     //实例
};

#endif // NOTIFYMANAGER_H
