#include "mytextedit.h"
#include<QDebug>

mytextedit::mytextedit(QWidget *parent) :
    QTextEdit(parent)
{
//    qDebug()<<"mylineedit.cpp";
//    parent->setObjectName(text);
//    this->setObjectName(text);
//    qDebug()<<objectName();
    //    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(mytextedit::responseTimeOut()));
    timer->start(2000);
}
void mytextedit::showContent(){
    QString str0=QString("位置:%1").arg(pos);
    setText(str0);
    for (int i=0;i<5;i++){
        anomaly(i);
    }
//    QPalette pal = palette();
//    pal.setBrush(QPalette::Base, Qt::blue);
//    setPalette(pal);
}
void mytextedit::responseTimeOut(){
    if(!response){
        QPalette pal = palette();
        pal.setBrush(QPalette::Base, Qt::red);
        setPalette(pal);
    }
    else{
        QPalette pal = palette();
        pal.setBrush(QPalette::Base, Qt::white);
        setPalette(pal);
        response=false;
    }
}
void mytextedit::changeContentColor(QString str){
    auto cur_text_color = textColor();
    // 设置当前行要使用的颜色，假设为红色
    setTextColor(Qt::red);
    // 写入一行内容
    append(str);
    // 最后恢复原来的颜色
    setTextColor(cur_text_color);
}
void mytextedit::anomaly(int control){
    qDebug()<<"time";
    response=true;
    switch (control) {
    case 0:
    {
        QString str1=QString("温度:%1,%2,%3").arg(tem[0]).arg(tem[1]).arg(tem[2]);
        for(int i=0;i<3;i++){
            if(tem[i]<temRange[0]||tem[i]>temRange[1])
                changeContentColor(str1);
            else
                append(str1);
        }
        break;
    }
    case 1:
    {
        QString str2=QString("电压:%1,%2,%3").arg(vol[0]).arg(vol[1]).arg(vol[2]);
        for(int i=0;i<3;i++){
            if(vol[i]<volRange[0]||vol[i]>volRange[1])
                changeContentColor(str2);
            else
                append(str2);
        }
        break;
    }
    case 2:
    {
        QString str3=QString("电流:%1,%2,%3").arg(ele[0]).arg(ele[1]).arg(ele[2]);
        for(int i=0;i<3;i++){
            if(ele[i]<eleRange[0] ||ele[i]>eleRange[1])
                changeContentColor(str3);
            else
                append(str3);
        }
        break;
    }
    case 3:
    {
        QString str4=QString("漏电电流:%1").arg(louele);
        if(louele<loueleRange[0] ||louele>loueleRange[1])
            changeContentColor(str4);
        else
            append(str4);
        break;
    }
    case 4:
    {
        QString str5=QString("绝缘信号:%1").arg(ins);
        if(ins<insRange[0] ||ins>insRange[1])
            changeContentColor(str5);
        else
            append(str5);
        break;
    }
    default:
        break;
    }
}

//重写mousePressEvent事件,检测事件类型是不是点击了鼠标左键
void mytextedit::mousePressEvent(QMouseEvent *event)
{
    //如果单击了就触发clicked信号
    if (event->button() == Qt::LeftButton)
    {
        //触发clicked信号
        emit clicked();
    }
    //将该事件传给父类处理
    QTextEdit::mousePressEvent(event);
}
