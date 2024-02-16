#include "commonutils.h"

#include <QPainter>
#include <QFile>
#include <QWidget>
#include <QApplication>
#include <QSettings>

CommonUtils::CommonUtils()
{
}

//获取(制作)圆头像     basicwindow.cpp中存在相同函数
QPixmap CommonUtils::getRoundImage(const QPixmap &src, QPixmap &mask, QSize masksize)
{
    if (masksize == QSize(0, 0))
    {
        masksize = mask.size();
    }
    else
    {
        mask = mask.scaled(masksize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    QImage resultImage(masksize, QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&resultImage);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(resultImage.rect(), Qt::transparent);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawPixmap(0, 0, mask);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.drawPixmap(0, 0, src.scaled(masksize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    painter.end();

    return QPixmap::fromImage(resultImage);
}

//加载样式表
void CommonUtils::loadStyleSheet(QWidget* widget, const QString &sheetName)
{
    QFile file(":/Resources/QSS/" + sheetName + ".css");
    file.open(QFile::ReadOnly);
    if (file.isOpen())
    {
        widget->setStyleSheet("");
        QString qsstyleSheet = QLatin1String(file.readAll());
        widget->setStyleSheet(qsstyleSheet);
    }

    file.close();
}

//设置默认皮肤颜色
void CommonUtils::setDefaultSkinColor(const QColor &color)
{
    //配置文件路径
    const QString && path = QApplication::applicationDirPath() + "/" + QString("tradeprintinfo.ini");
    //操作配置文件 QSettings::IniFormat表示对文件进行读写操作
    QSettings settings(path, QSettings::IniFormat);
    //设置配置文件中的rgb值
    settings.setValue("DefaultSkin/red", color.red());
    settings.setValue("DefaultSkin/green", color.green());
    settings.setValue("DefaultSkin/blue", color.blue());
}

//获取默认皮肤颜色
QColor CommonUtils::getDefaultSkinColor()
{
    QColor color;   //保存颜色的变量
    //配置文件路径
    const QString && path = QApplication::applicationDirPath() + "/" + QString("tradeprintinfo.ini");
    //如果文件不存在
    if (!QFile::exists(path))
    {
        setDefaultSkinColor(QColor(22, 154, 218));  //文件不存在 设置一个默认值
    }

    //操作配置文件 QSettings::IniFormat表示对文件进行读写操作
    QSettings settings(path, QSettings::IniFormat);

    //读取配置文件rgb值 并设置到 color 变量中
    color.setRed(settings.value("DefaultSkin/red").toInt());
    color.setGreen(settings.value("DefaultSkin/green").toInt());
    color.setBlue(settings.value("DefaultSkin/blue").toInt());

    return color;   //返回从配置文件读取到的值
}
