#include "sendfile.h"
#include "ui_sendfile.h"
#include "talkwindowshell.h"
#include "windowmanager.h"

#include <QFileDialog>
#include <QMessageBox>

SendFile::SendFile(QWidget *parent) : BasicWindow(parent), ui(new Ui::SendFile), m_filePath("")
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    initTitleBar();
    setTitleBarTitle("", ":/Resources/MainWindow/qqlogoclassic.png");
    loadStyleSheet("SendFile");
    this->move(100, 400);

    //获取主聊天窗口
    TalkWindowShell* talkWindowShell = WindowManager::getInstance()->getTalkWindowShell();
    //当发送文件按钮被点击 执行主聊天窗口类下的 updateSendTcpMsg()函数发送数据
    connect(this, &SendFile::sendFileClicked, talkWindowShell, &TalkWindowShell::updateSendTcpMsg);
}

SendFile::~SendFile()
{
    delete ui;
}

//打开(选择)文件按钮点击
void SendFile::on_openBtn_clicked()
{
    m_filePath = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择文件"), "/", QString::fromLocal8Bit("发送的文件(*.txt *.doc);;所有文件(*.*);;"));
    ui->lineEdit->setText(m_filePath);
}

//发送按钮按钮点击
void SendFile::on_sendBtn_clicked()
{
    if (!m_filePath.isEmpty())  //文件路径不为空
    {
        QFile file(m_filePath); //选择文件
        if (file.open(QIODevice::ReadOnly)) //只读方式打开文件
        {
            int msgType = 2;
            QString str = file.readAll();   //读取文件全部内容

            //文件名称
            QFileInfo fileInfo(m_filePath);
            QString fileName = fileInfo.fileName();

            emit sendFileClicked(str, msgType, fileName);
            file.close();
        }
        else    //打开文件失败
        {
            QMessageBox::information(this, QStringLiteral("提示"), QString::fromLocal8Bit("发送文件:%1失败！").arg(m_filePath));
            this->close();
            return;
        }

        m_filePath = "";    //重置文件路径
        this->close();      //关闭窗口
    }
}
