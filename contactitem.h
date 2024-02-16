#ifndef CONTACTITEM_H
#define CONTACTITEM_H

#include <QWidget>

namespace Ui {
class ContactItem;
}

//联系人选项类
class ContactItem : public QWidget
{
    Q_OBJECT

public:
    ContactItem(QWidget *parent = Q_NULLPTR);
    ~ContactItem();

    void setUserName(const QString& userName);      //设置联系人用户名
    void setSignName(const QString& signName);      //设置联系人个性签名
    void setHeadPixmap(const QPixmap& headPath);    //设置联系人头像
    QString getUserName()const;                     //获取联系人用户名
    QSize getHeadLabelSize()const;                  //获取联系人头像标签大小

private:
    void initControl();                             //初始化控件(联系人选项)

private:
    Ui::ContactItem *ui;
};

#endif // CONTACTITEM_H
