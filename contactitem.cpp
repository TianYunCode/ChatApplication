#include "contactitem.h"
#include "ui_contactitem.h"

ContactItem::ContactItem(QWidget *parent) : QWidget(parent), ui(new Ui::ContactItem)
{
    ui->setupUi(this);
    initControl();      //初始化控件(联系人选项)
}

ContactItem::~ContactItem()
{
    delete ui;
}

//设置联系人用户名
void ContactItem::setUserName(const QString & userName)
{
    ui->userName->setText(userName);
}

//设置联系人个性签名
void ContactItem::setSignName(const QString & signName)
{
    ui->signName->setText(signName);
}

//设置联系人头像
void ContactItem::setHeadPixmap(const QPixmap & headPath)
{
    ui->label->setPixmap(headPath);
}

//获取联系人用户名
QString ContactItem::getUserName() const
{
    return ui->userName->text();
}

//获取联系人头像标签大小
QSize ContactItem::getHeadLabelSize() const
{
    return ui->label->size();
}

//初始化控件(联系人选项)
void ContactItem::initControl()
{
}
