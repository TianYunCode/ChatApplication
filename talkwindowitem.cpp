#include "talkwindowitem.h"
#include "ui_talkwindowitem.h"
#include "commonutils.h"

TalkWindowItem::TalkWindowItem(QWidget *parent) : QWidget(parent), ui(new Ui::TalkWindowItem)
{
    ui->setupUi(this);

    initControl();      //初始化控件(聊天选项卡)
}

TalkWindowItem::~TalkWindowItem()
{
    delete ui;
}

//设置聊天选项卡头像
void TalkWindowItem::setHeadPixmap(const QPixmap& pixmap)
{
    //空白圆头像
    QPixmap mask = QPixmap(":/Resources/MainWindow/head_mask.png");

    //获取(制作)圆头像
    const QPixmap& headpixmap = CommonUtils::getRoundImage(pixmap, mask, ui->headlabel->size());
    //设置联系人选项卡头像
    ui->headlabel->setPixmap(headpixmap);
}

//设置聊天选项卡内容(聊天人名称)
void TalkWindowItem::setMsgLabelContent(const QString & msg)
{
    ui->msgLabel->setText(msg);
}

//获取聊天选项卡内容(聊天人名称)
QString TalkWindowItem::getMsgLabelText()
{
    return ui->msgLabel->text();
}

//初始化控件(聊天选项卡)
void TalkWindowItem::initControl()
{
    //设置聊天选项卡关闭按钮不可见
    ui->tclosebtn->setVisible(false);
    //当聊天选项卡关闭按钮按下 执行发射关闭点击信号
    connect(ui->tclosebtn, SIGNAL(clicked(bool)), this, SIGNAL(signalCloseClicked()));
}

//重写进入事件
void TalkWindowItem::enterEvent(QEvent * event)
{
    //设置聊天选项卡关闭按钮可见
    ui->tclosebtn->setVisible(true);

    __super::enterEvent(event);
}

//重写离开事件
void TalkWindowItem::leaveEvent(QEvent * event)
{
    //设置聊天选项卡关闭按钮不可见
    ui->tclosebtn->setVisible(false);

    __super::enterEvent(event);
}

//重写调整大小事件
void TalkWindowItem::resizeEvent(QResizeEvent * event)
{
    __super::resizeEvent(event);
}
