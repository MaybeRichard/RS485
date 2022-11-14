#ifndef MYLINEEDIT_H
#define MYLINEEDIT_H
#include <QTextEdit>
#include <QMouseEvent>

class mytextedit: public QTextEdit
{
    Q_OBJECT
public:
    mytextedit();
//    explicit mylineedit(const QString &text, QWidget *parent = 0);
    explicit mytextedit(QWidget *parent = 0);
protected:
    //重写mousePressEvent事件
    virtual void mousePressEvent(QMouseEvent *event);

signals:
    //自定义clicked()信号,在mousePressEvent事件发生时触发
    void clicked();

public slots:
};

#endif // MYLINEEDIT_H
