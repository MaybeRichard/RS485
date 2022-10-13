#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();
    return a.exec();
}

//#include <QApplication>
//#include <QWidget>
//#include <QLabel>
//#include <QHBoxLayout>
//int main1()
//{
//    QApplication a();
//    //创建主窗口
//    QWidget widget;
//    widget.setWindowTitle("QHBoxLayout水平布局");
//    //创建水平布局控件
//    QHBoxLayout *layout=new QHBoxLayout;
//    //设置布局控件中所有控件的布局方向为从右往左依次排列
//    layout->setDirection(QBoxLayout::RightToLeft);
//    //连续创建 3 个文本框，并设置它们的背景和字体大小
//    QLabel lab1("Label1");
//    lab1.setStyleSheet("QLabel{background:#dddddd;font:20px;}");
//    lab1.setAlignment(Qt::AlignCenter);
//    QLabel lab2("Label2");
//    lab2.setStyleSheet("QLabel{background:#cccccc;font:20px;}");
//    lab2.setAlignment(Qt::AlignCenter);
//    QLabel lab3("Label3");
//    lab3.setStyleSheet("QLabel{background:#ffffff;font:20px;}");
//    lab3.setAlignment(Qt::AlignCenter);
//    //将 3 个文本框和 2 个空白列添加到管理器中，它们的拉伸系数比是 2:1:2:3:3
//    layout->addStretch(2);
//    layout->addWidget(&lab1,1);
//    layout->addWidget(&lab2,2);
//    layout->addWidget(&lab3,3);
//    layout->addStretch(3);
//    //将布局控件添加到 widget 窗口中
//    widget.setLayout(layout);
//    widget.show();
//    return a.exec();
//}

