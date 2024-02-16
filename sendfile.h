#ifndef SENDFILE_H
#define SENDFILE_H

#include "basicwindow.h"

namespace Ui {
class SendFile;
}

//发送文件类
class SendFile : public BasicWindow
{
    Q_OBJECT

public:
    SendFile(QWidget *parent = Q_NULLPTR);
    ~SendFile();

signals:
    void sendFileClicked(QString &strData, int &msgType, QString fileName); //发送文件按钮被点击的信号

private slots:
    void on_openBtn_clicked();	//打开(选择)文件按钮点击
    void on_sendBtn_clicked();	//发送按钮按钮点击

private:
    Ui::SendFile *ui;
    QString m_filePath;			//选择的文件路径
};

#endif // SENDFILE_H
