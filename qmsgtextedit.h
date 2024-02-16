#ifndef QMSGTEXTEDIT_H
#define QMSGTEXTEDIT_H

#include <QTextEdit>

//自定义消息编辑框接口类
class QMsgTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    QMsgTextEdit(QWidget *parent = nullptr);
    ~QMsgTextEdit();

private slots:
    void onEmotionImageFrameChange(int frame);  //动画改变时响应的槽函数

public:
    void addEmotionUrl(int emotionNum);         //添加表情网址
    void deletAllEmotionImage();                //删除全部表情图片(在文本编辑框里删除)

private:
    QList<QString> m_listEmotionUrl;            //表情网址列表
    QMap<QMovie*, QString> m_emotionMap;        //表情动画映射
};

#endif // QMSGTEXTEDIT_H
