#ifndef RECEIVEFILE_H
#define RECEIVEFILE_H

#include "basicwindow.h"

namespace Ui {
class ReceiveFile;
}

//读取(接收)文件类
class ReceiveFile : public BasicWindow
{
    Q_OBJECT

public:
    ReceiveFile(QWidget *parent = Q_NULLPTR);
    ~ReceiveFile();

    void setMsg(QString &msgLabel);     //设置消息

signals:
    void refuseFile();                  //拒绝文件信号

private slots:
    void on_cancelBtn_clicked();        //点击取消按钮 槽函数
    void on_okBtn_clicked();            //点击确定按钮 槽函数

private:
    Ui::ReceiveFile *ui;
};

#endif // RECEIVEFILE_H
