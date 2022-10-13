# include "myweight.h"
#include "mylineedit.h"
#include <QSignalMapper>
#include <QPushButton>
#include <QGridLayout>
#include <QLineEdit>
#include <QDebug>
#include "QMessageBox.h"

MyWidget::MyWidget(const QStringList &texts, QWidget *parent)
    : QWidget(parent)
{
    signalMapper = new QSignalMapper(this);

    QGridLayout *gridLayout = new QGridLayout;
    for (int i = 0; i < texts.size(); ++i) {
        QPushButton *button = new QPushButton(texts[i]);
//        connect(button, &QPushButton::clicked, signalMapper, &QSignalMapper::map);
//        connect(button, &QPushButton::clicked, signalMapper, &QSignalMapper::map);
        signalMapper->setMapping(button, texts[i]);
        gridLayout->addWidget(button, i / 3, i % 3);
    }

//    connect(signalMapper, &QSignalMapper::mappedString,
//            this, &MyWidget::doClicked);
    connect(signalMapper, &QSignalMapper::mappedString,
                this, &MyWidget::clicked);

    setLayout(gridLayout);
}
//MyWidget::MyWidget(QWidget *parent) :
//    QWidget(parent)
//{
//    QString buttonText = "btn1,btn2,btn3,btn4,btn5,btn6,btn7,btn8,btn9,btn10";
//    QStringList textList = buttonText.split(",");
//    signalMapper = new QSignalMapper(this);
//    QGridLayout *gridLayout = new QGridLayout;
//    qDebug()<<"init mywidget";

//    for(int i=0; i<textList.size(); ++i)
//    {
//        //动态创建按钮
//        QPushButton *button = new QPushButton(textList[i]);

//        //原始信号传递给signalMapper
////        connect(button,SIGNAL(clicked()),signalMapper,SLOT(map()));
//        connect(button, &QPushButton::clicked, signalMapper, &QSignalMapper::map);
//        //设置signalMapper的转发规则，转发为参数为QString类型的信号，并把textList[i]的内容作为实参传递
//        signalMapper->setMapping(button, textList[i]);

//        gridLayout->addWidget(button, i/3, i%3);
//    }

//    //将转发的信号连接到最终的槽函数上
////    connect(signalMapper,SIGNAL(mapped(QString)),this,SLOT(doClicked(QString)));
//    connect(signalMapper, &QSignalMapper::mappedString,this, &MyWidget::doClicked(QString));

//    setLayout(gridLayout);
//}

void MyWidget::doClicked(const QString &btnName)
{
    //显示被按下的button名称
    QMessageBox::information(this,"Clicked",btnName+" is clicked !");
}

