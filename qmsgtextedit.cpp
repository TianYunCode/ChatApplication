#include "qmsgtextedit.h"

#include <QMovie>
#include <QUrl>

QMsgTextEdit::QMsgTextEdit(QWidget *parent) : QTextEdit(parent)
{
}

QMsgTextEdit::~QMsgTextEdit()
{
    deletAllEmotionImage();
}

//动画改变时响应的槽函数
void QMsgTextEdit::onEmotionImageFrameChange(int frame)
{
    //sender()返回信号发送者
    QMovie* movie = qobject_cast<QMovie*>(sender());
    document()->addResource(QTextDocument::ImageResource, QUrl(m_emotionMap.value(movie)), movie->currentPixmap());
}

//添加表情网址
void QMsgTextEdit::addEmotionUrl(int emotionNum)
{
    const QString& imageName = QString("qrc:/Resources/MainWindow/emotion/%1.png").arg(emotionNum);
    const QString& flagName = QString("%1").arg(imageName);
    //拼接网页
    insertHtml(QString("<img src='%1'/>").arg(flagName));
    //如果列表已存在刚拼接的网页 直接返回
    if (m_listEmotionUrl.contains(imageName))
    {
        return;
    }
    else    //如果不存在 加入列表
    {
        m_listEmotionUrl.append(imageName);
    }

    //创建一个动图
    QMovie* apngMovie = new QMovie(imageName, "apng", this);
    m_emotionMap.insert(apngMovie, flagName);   //插入动图进map

    //数据帧改变时发射的信号
    connect(apngMovie, SIGNAL(frameChanged(int)), this, SLOT(onEmotionImageFrameChange(int)));
    apngMovie->start();
    updateGeometry();   //更新窗口几何形状
}

//删除全部表情图片(在文本编辑框里删除)
void QMsgTextEdit::deletAllEmotionImage()
{
    for (auto itor = m_emotionMap.constBegin(); itor != m_emotionMap.constEnd(); ++itor)
    {
        delete itor.key();
    }

    m_emotionMap.clear();
}
