#ifndef MYWEIGHT_H
#define MYWEIGHT_H
#include <QWidget>
#include <QSignalMapper>

//class MyWidget : public QWidget
//{
//    Q_OBJECT
//public:
//    explicit MyWidget(QWidget *parent = 0);

//signals:

//public slots:
//    //处理最终信号的槽
//    void doClicked(const QString &btnName);

//private:
//    QSignalMapper *signalMapper;

//};

class MyWidget : public QWidget
{
    Q_OBJECT

public:
    MyWidget(const QStringList &texts, QWidget *parent = nullptr);
public slots:
    void doClicked(const QString &btnName);
signals:
    void clicked(const QString &text);
//    void doClicked(const QString &btnName);

private:
    QSignalMapper *signalMapper;
};

#endif // MYWEIGHT_H
