#ifndef MYLINEEDIT_H
#define MYLINEEDIT_H
#include <QTextEdit>
#include <QMouseEvent>
#include <QTimer>

class mytextedit: public QTextEdit
{
    Q_OBJECT
public:
    mytextedit();
//    explicit mylineedit(const QString &text, QWidget *parent = 0);
    explicit mytextedit(QWidget *parent = 0);
    void showContent();
    void changeContentColor(QString str);
    void anomaly(int control);
    QTimer *timer = new QTimer(this);
    void responseTimeOut();
protected:
    //重写mousePressEvent事件
    virtual void mousePressEvent(QMouseEvent *event);
    int pos=0;
    float tem[3]={0,0,0};//温度
    float ele[3]={0,0,0};//电流
    float vol[3]={0,0,0};//电压
    float louele=0; //漏电电流
    float ins=0;//绝缘信号
    float temRange[2]={0,10};
    float eleRange[2]={0,10};
    float volRange[2]={0,10};
    float loueleRange[2]={0,10};
    float insRange[2]={2,10};
    QString temp;
    bool response=true;



signals:
    //自定义clicked()信号,在mousePressEvent事件发生时触发
    void clicked();
//    void responseTimeOut();

public slots:
};

#endif // MYLINEEDIT_H
