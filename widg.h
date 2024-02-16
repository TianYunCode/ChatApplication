#ifndef WIDG_H
#define WIDG_H

#include <QWidget>

class widg : public QWidget
{
    Q_OBJECT
public:
    explicit widg(QWidget *parent = nullptr);

signals:
};

#endif // WIDG_H
