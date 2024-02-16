#include "receivefile.h"
#include "ui_receivefile.h"

#include <QFileDialog>
#include <QMessageBox>

extern QString gfileName;
extern QString gfileData;

ReceiveFile::ReceiveFile(QWidget *parent) :BasicWindow(parent), ui(new Ui::ReceiveFile)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    initTitleBar();
    setTitleBarTitle("", ":/Resources/MainWindow/qqlogoclassic.png");
    loadStyleSheet("ReceiveFile");
    this->move(100, 400);

    //如果用户点击标题栏的关闭按钮 发射拒绝文件信号
    connect(_titleBar, &TitleBar::signalButtonCloseClicked, this, &ReceiveFile::refuseFile);
}

ReceiveFile::~ReceiveFile()
{
    delete ui;
}

//设置消息
void ReceiveFile::setMsg(QString & msgLabel)
{
    ui->label->setText(msgLabel);
}

//点击取消按钮
void ReceiveFile::on_cancelBtn_clicked()
{
    emit refuseFile();  //发送拒绝文件信号
    this->close();      //关闭窗口
}

//点击确定按钮
void ReceiveFile::on_okBtn_clicked()
{
    this->close();  //关闭窗口

    //获取想要保存的文件路径
    QString fileDirPath = QFileDialog::getExistingDirectory(nullptr, QStringLiteral("文件保存路径"), "/");

    QString filePath = fileDirPath + "/" + gfileName;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::information(nullptr, QStringLiteral("提示"), QStringLiteral("文件接收失败！"));
    }
    else
    {
        file.write(gfileData.toUtf8());
        file.close();
        QMessageBox::information(nullptr, QStringLiteral("提示"), QStringLiteral("文件接收成功！"));
    }
}
