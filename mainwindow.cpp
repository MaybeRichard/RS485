#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QMessageBox.h"
//#include "QHBoxLayout.h"
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QSignalMapper>
#include <QDebug>
#include <QVector>
#include <QTreeWidgetItem>
#include <QTimer>
#include <QTime>
#include <QModbusRtuSerialMaster>
#include <QThread>

//存储当前传输的协议
unsigned int startAdd;

//定义需要查询的11个参数
// 0x0064、0x0066、0x0068 线电压
float Uab=0.0;
float Ubc=0.0;
float Uca=0.0;
// 0x0074、0x0076、0x0078 线电流
float Ia=0.0;
float Ib=0.0;
float Ic=0.0;
// 0x00C0、0x00C2、0x00C4 温度
float FirstTemp=0.0;
float SecondTemp=0.0;
float ThirdTemp=0.0;
//剩余/漏电/零序电流
float remainAc=0.0;


//order存储所有需要发送的功能指令，一共11个，其中绝缘信号暂时不知道，使用零序电流代替
unsigned int order[11] = {0x00C0,0x00C2,0x00C4,0x0064,0x0066,0x0068,0x0074,0x0076,0x0078,0x007C,0x007C};
//控制当前执行到的命令索引
int currentAddre = 0;
//控制当前执行到的命令
int currentOrrder = 0;
//地址表长度和内容的定义，地址索引的定义，以及地址的赋值
unsigned int Address[6]={1,2,3,4,5,6};
unsigned int addrIndex = 0;
unsigned int addr=Address[addrIndex];

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Initserial();
    signalMapper = new QSignalMapper(this);
    InitUi();
//    readData();
    MainWindow::updata_content();
    QTimer *timer = new QTimer(this);
    timer->start(50000);
    connect(timer, SIGNAL(timeout()), this, SLOT(readData()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::show_detial(QString name){
    int id =name.toInt();
////    QMessageBox::question(this,
//           tr("弹窗标题"),
//           tr("弹窗内容:\n温度:%d \n温度:%d \n电压:%d ",(1,2,3)),
//           QMessageBox::Ok | QMessageBox::Cancel,
//           QMessageBox::Ok);
    QMessageBox::information(NULL,  "信息",
tr("位置:%1\n温度(C):%2.2f,%3f,%4\n电压(V):%5,%6,%7\n电流(A):%8,%9,%10\n漏电电流(V)%11\n绝缘信号:%12").arg(cont[id][0])\
   .arg(cont[id][1]).arg(cont[id][2]).arg(cont[id][3]).arg(cont[id][4]).arg(cont[id][5]).arg(cont[id][6]).arg(cont[id][7])\
   .arg(cont[id][8]).arg(cont[id][9]).arg(cont[id][10]).arg(cont[id][11]),
                             QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
}

//void MainWindow::Initserial(){
////    QPushButton *btn=new QPushButton(this);
//    mylineedit *btn=new mylineedit(this);
//    btn->setText("on");
//    btn->move(0,0);
//}

void MainWindow::InitUi()
{
    for(int i=0;i<maxnum;i++){
        names<<QString::number(i);;
    }
    QGridLayout *gridLayout = new QGridLayout;
    //Buttom******************************
    for (int i = 0; i < names.size(); ++i)
    {
        mytextedit *button = new mytextedit(this);
        button->setObjectName(names[i]);
        button->setText(names[i]);
        button->resize(20,20);
        connect(button, SIGNAL(clicked()), signalMapper, SLOT(map()));
        signalMapper->setMapping(button, names[i]);
        gridLayout->addWidget(button, i / colnum, i % colnum);
    }
//    qDebug()<<gridLayout->columnCount();
//    connect(signalMapper, SIGNAL(mapped(QString)),
//            this, SLOT(ShowName(QString)));
    connect(signalMapper, &QSignalMapper::mappedString,this, &MainWindow::ShowName);
//    connect(signalMapper, &QSignalMapper::mappedString,[=]{
//        &MainWindow::ShowName;
////        &MainWindow::show_detial;
////        QString name="1";
////        MainWindow::ShowName(names[i]);
//    });
//    setLayout(gridLayout);

    QWidget* widget = new QWidget(this);
    widget->setLayout(gridLayout);
    this->setCentralWidget(widget);
//    qDebug()<<"end";
}
void MainWindow::ShowName(QString name)
{
//    QMessageBox::information(this, "Name", name);
    MainWindow::show_detial(name);

//    button->setText("123");
//    qDebug() << QObject::sender()->objectName();
//    QObject::sender()->
//    QTreeWidgetItem *pCurrentItem = ui->treeWidget->currentItem();
//    QString strText = pCurrentItem->text(3333);
    int mouse_x = QCursor::pos().x();//鼠标点击处横坐标
    int mouse_y = QCursor::pos().y();//鼠标点击处纵坐标
    QWidget *action = QApplication::widgetAt(mouse_x, mouse_y);
//    action->
//    action->setObjectName();
//    qDebug()<<action->baseSize();
//    qDebug()<<action->focusWidget()->pos();
//     qDebug()<<action->pos();
}

void MainWindow::Initserial()
{
    modbustoolDevice = new QModbusRtuSerialMaster(this);
    //判断modbus是否已经定义，没有则直接返回
    if (!modbustoolDevice)
        return;

    //判断modbus是否已经连接
    if (modbustoolDevice->state() != QModbusDevice::ConnectedState) {
        //如果没有连接，则先设置串口的名称，串口奇偶校验位，串口波特率，串口数据位，串口停止位
        modbustoolDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter,
                                                 "COM4");
        modbustoolDevice->setConnectionParameter(QModbusDevice::SerialParityParameter,
                                                 QSerialPort::EvenParity);
        modbustoolDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,
                                                 QSerialPort::Baud9600);
        modbustoolDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,
                                                 QSerialPort::Data8);
        modbustoolDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,
                                                 QSerialPort::OneStop);

        //再设置从机无响应时的动作*******************************
        qDebug()<<"Check Device Responsd Time";
        modbustoolDevice->setTimeout(5000);//从设备回复信息的超时时间
        modbustoolDevice->setNumberOfRetries(0);//重复发送次数

        //开始连接串口，并判断连接状态
        if (!modbustoolDevice->connectDevice()) {
            //如果连接失败，提示
            qDebug()<<"连接失败";
//            statusBar()->showMessage(tr("连接失败: ") + modbustoolDevice->errorString(), 5000);
            //FlagGetStatus=0;//连接失败，停止定时刷新
            //ui->Status_SerialConnect->setStyleSheet("font-size:100px;border-radius:30px;\nbackground-color: red");
        }else{
            //如果连接成功，设置下面按钮的状态，同时给出提示
//                ui->modbus_connect->setEnabled(false);
//                ui->modbus_disconnect->setEnabled(true);
//                ui->send_commander->setEnabled(true);

            statusBar()->showMessage(tr("连接成功!"), 5000);
            //FlagGetStatus=1;//连接成功，定时刷新
            //ui->Status_SerialConnect->setStyleSheet("font-size:100px;border-radius:30px;\nbackground-color: green");
        }
    } else {
        //如果已经连接，则关闭串口
        modbustoolDevice->disconnectDevice();
        //ui->Status_SerialConnect->setStyleSheet("font-size:100px;border-radius:30px;\nbackground-color: rgb(205, 205, 205)");
    }
}

void MainWindow::readData()
{
    qDebug()<<"1";
    addr=Address[addrIndex];
//    qDebug()<<"当前地址"<<addr;
    //每个地址的第一个协议都是通过readData来调用，之后的协议在decode中调用，每次调用后都把当前的命令索引(currentOrrder)+1
    readfromclient(0x00C0,0x02);
    currentOrrder++;
    MainWindow::updata_content();
}

// startAddress：协议中的地址表
void MainWindow::readfromclient(unsigned int startAddress,int number)
{
    qDebug()<<2;
//    qDebug()<<"readfromclient协议"<<binaryTransfer(startAddress);
//    qDebug()<<"readfromclient地址"<<addr;
    startAdd=startAddress;
    //如果没有定义modbus串口，直接返回
    if (!modbustoolDevice)
        return;
    QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters,startAddress,number);
    //发送Modbus格式的数据到从机地址为serverAddress的从机，读取相应的数据
      unsigned int serverAddress=addr;
    if (auto *reply = modbustoolDevice->sendReadRequest(readUnit, serverAddress)) {
        //再设置从机无响应时的动作*******************************
        if (!reply->isFinished()){
            qDebug()<<3;
//            qDebug()<<"readfromclient中有Reply，即将进入readReady";

            connect(reply, &QModbusReply::finished, this, &MainWindow::readReady);
        }
        else
        {//如果多次发送后没有响应，结束
            delete reply; // broadcast replies return immediately
            qDebug()<<"readfromclient NO Response";
        }
    } else {//如果发送失败，更新状态栏信息
        qDebug()<<"readfromclient read Failed";
//        statusBar()->showMessage(tr("读取失败: ") + modbustoolDevice->errorString(), 5000);
    }
}
/*
 * @FuncName   函数名:读下位机寄存器数据回调函数
 *
 * @Function   功能：解析下位机上传的数据
 */

void MainWindow::readReady()
{
    qDebug()<<4;
//    qDebug () <<"进入ReadReady";
    //reply: 回应状态 result: 存储解码数据
    auto reply = qobject_cast<QModbusReply *>(sender());
    QString result;
     //如果没有响应数据，直接返回
    if (!reply)
    {
//        qDebug()<<"readReady没有响应";
        return;
    }
    if (reply->error() == QModbusDevice::NoError) {
        //如果响应数据校验后，没有错误。解析数据
        qDebug()<<5;
//        qDebug()<<"ReadReady校验正常";
        const QModbusDataUnit unit = reply->result();
        QString showdata=tr("总数:%1 值为:").arg(unit.valueCount());
        for (uint i = 0; i < unit.valueCount(); i++) {
            const QString entry=tr("%1").arg(unit.value(i));
            showdata=showdata+entry;
            result+=binaryTransfer(unit.value(i));
        }
        int address = reply->serverAddress();
//        qDebug()<<"命令"<<binaryTransfer(startAdd)<<"进入decode";
        decode(result,address);

    } else if (reply->error() == QModbusDevice::ProtocolError) {
        //如果响应数据校验后，有协议错误，更新状态栏显示错误码
        qDebug()<<"readReady部分协议出错";
        statusBar()->showMessage(tr("读取回应错误: %1 (Mobus exception: 0x%2)").
                                 arg(reply->errorString()).
                                 arg(reply->rawResult().exceptionCode(), -1, 16), 5000);
    } else {
        //如果响应数据校验后，有错误，更新状态栏显示错误码
        qDebug()<<"readReady部分校验出错"<<reply->errorString();
    }
    //结束响应过程
    reply->deleteLater();
}

QString MainWindow::binaryTransfer(int data)
{
    //转化为16进制
    bool ok;
    QString hex = QString("%1").arg(data,4,16, QLatin1Char('0'));
    return hex;

//读寄存器
}
// 解码函数
void MainWindow::decode(QString result,int address)
{
    qDebug()<<6;
//    qDebug()<<"进入decode";
    QString temp= binaryTransfer(startAdd);
//    qDebug()<<"协议"<<temp;
    if(temp=="00c0"){
        //1线温度
        FirstTemp = fromFourToFloat(result);
        qDebug()<<"1通道温度:"<<fromFourToFloat(result);
    }
    if(temp=="00c2"){
        //2线温度
        SecondTemp = fromFourToFloat(result);
        qDebug()<<"2通道温度:"<<fromFourToFloat(result);
    }
    if(temp=="00c4"){
        //3线温度
        ThirdTemp = fromFourToFloat(result);
        qDebug()<<"3通道温度:"<<fromFourToFloat(result);
    }

    if(temp=="0064"){
        //线电压Uab
        Uab = fromFourToFloat(result);
        qDebug()<<"线电压Uab:"<<fromFourToFloat(result);
    }
    if(temp=="0066"){
        //线电压Ubc
        Ubc = fromFourToFloat(result);
        qDebug()<<"线电压Uab:"<<fromFourToFloat(result);
    }
    if(temp=="0068"){
        //线电压Uca
        Uca = fromFourToFloat(result);
        qDebug()<<"线电压Uab:"<<fromFourToFloat(result);
    }
    if(temp=="0074"){
        //线电流Ia
        Ia = fromFourToFloat(result);
        qDebug()<<"线电流Ia:"<<fromFourToFloat(result);
    }
    if(temp=="0076"){
        //线电流Ib
        Ib = fromFourToFloat(result);
        qDebug()<<"线电流Ib:"<<fromFourToFloat(result);
    }
    if(temp=="0078"){
        //线电流Ic
        Ic = fromFourToFloat(result);
        qDebug()<<"线电流Ic:"<<fromFourToFloat(result);
    }
    if(temp=="007C"){
        //零序剩余电流remainAc
        remainAc = fromFourToFloat(result);
        qDebug()<<"零序剩余电流remainAc:"<<fromFourToFloat(result);
    }
    //存储信息到全局变量cont中，按照顺序分别赋值
    //位置:%1\n温度(C):%2,%3,%4\n电压(v):%5,%6,%7\n电流(A):%8,%9,%10\n漏电电流(A)%11,%12,%13\n绝缘信号
    cont[address-1][0]=address; //地址赋值给1号，电机第一号为0，所以赋值需要减1
    cont[address-1][1]=FirstTemp;//温度信息暂无
    cont[address-1][2]=SecondTemp;
    cont[address-1][3]=ThirdTemp;
    cont[address-1][4]=Uab;//电压信息
    cont[address-1][5]=Ubc;
    cont[address-1][6]=Uca;
    cont[address-1][7]=Ia;//电流信息
    cont[address-1][8]=Ib;
    cont[address-1][9]=Ic;
    cont[address-1][10]=remainAc;//漏电电流信息暂无
    cont[address-1][11]=0;//绝缘信号暂无
    //当前指令执行完成，返回并执行下一个
    qDebug()<<"currentOrrder:"<<currentOrrder;
    if(currentOrrder>=1&&currentOrrder<=10){
//        qDebug()<<"sending Order"<<binaryTransfer(order[currentOrrder]);
        readfromclient(order[currentOrrder],0x02);
        currentOrrder++;
    }else if(currentOrrder==11){
        readfromclient(order[currentOrrder],0x02);
        //判断地址是否循环到表的最后，如果到了最后则回到初始位置重新开始
    }else{
        currentOrrder=0;
        readData();
    }
}

/*
 *四字节转浮点数
 *result:待转换的4字节
 */
float MainWindow::fromFourToFloat(QString result){
    uint hex_uint = result.toUInt(nullptr, 16);

    float hex_res = *(float*)&hex_uint;

    return hex_res;
}


void MainWindow::updata_content(){
//    auto TextEdit = findChild<mytextedit *>(name);
//    TextEdit->setText(QString::number(300));
    for(int id=0;id<maxnum;++id){
        auto TextEdit = findChild<mytextedit *>(QString::number(id));
        TextEdit->setText(QString("位置:%1").arg((int) cont[id][0]));
        auto cur_text_color = TextEdit->textColor();
        if(0==0){
            // 设置当前行要使用的颜色，假设为红色
            TextEdit->setTextColor(Qt::red);
            TextEdit->append(QString("温度(C):%2,%3,%4").arg((int) cont[id][1]).arg((int) cont[id][2]).arg((int) cont[id][3]));
        }
        // 最后恢复原来的颜色
        TextEdit->setTextColor(cur_text_color);

        TextEdit->append(QString("电压(V):%5,%6,%7").arg((int) cont[id][4]).arg((int) cont[id][5]).arg((int) cont[id][6]));
        TextEdit->append(QString("电流(A):%8,%9,%10").arg((int) cont[id][7]).arg((int) cont[id][8]).arg((int) cont[id][9]));
        TextEdit->append(QString("漏电电流(A):%11").arg((int) cont[id][10]));
        TextEdit->append(QString("绝缘信号:%12").arg((int) cont[id][11]));
//        TextEdit->setText(QString("位置:%1\n温度(C):%2,%3,%4\n电压(V):%5,%6,%7\n电流(A):%8,%9,%10\n漏电电流(A)%11\n绝缘信号:%12").arg((int) cont[id][0])\
//           .arg(cont[id][1]).arg(cont[id][2]).arg(cont[id][3]).arg(cont[id][4]).arg(cont[id][5]).arg(cont[id][6]).arg(cont[id][7])\
//           .arg(cont[id][8]).arg(cont[id][9]).arg(cont[id][10]).arg(cont[id][11]));
//        auto cur_text_color = TextEdit->textColor();
//        TextEdit->setTextColor(cur_text_color);
        QPalette pal = TextEdit->palette();

        if(cont[id][11]==0 && cont[id][1]==0 && cont[id][2]==0 && cont[id][3]==0&& cont[id][4]==0){
            pal.setBrush(QPalette::Base, Qt::red);
        }else{
            pal.setBrush(QPalette::Base, Qt::white);
        }
        TextEdit->setPalette(pal);
    }
}

//*--------------------------------------------------------------------
//void MainWindow::init(){
//    #define COL 4           //每行显示个数
//    QVector<QPushButton*>   mVecButton;
//    QVector<mylineedit*>   mVecline;
////    int32_t                 mMaxBuildings=16;          // 总数
//    const int               mMaxBuildings=32;
////    int32_t                 mSelectedBtn=0;           // 当前选择的
//   // ===============================================
//    if(mMaxBuildings == 0){
//        return;
//    }

//    QString commStyle = QString("QPushButton{%1;} QPushButton:checked{%2} \n")
//            .arg("a")
//            .arg("b");

//    mVecline.clear();
//    int maxBulidingCount = mMaxBuildings;
////    const int maxcount = ;

//    QSignalMapper * clickMapper = new QSignalMapper(this);
////    QPushButton * btn[maxBulidingCount];
////    QPushButton * btn=new QPushButton[maxBulidingCount];
////    mylineedit * btn=new mylineedit[maxBulidingCount];
//    mylineedit * btn[mMaxBuildings];
//    for(int i = 0 ; i < maxBulidingCount; ++i){
//        btn[i] = new mylineedit(this);
////        btn[i] = new QPushButton(ui->builidingWidget);
////        btn[i]->setText(QString::number(i + 1) + ntr("").c_str());
////        btn[i].setText(QString::number(i + 1) + ntr("a").c_str());
//        btn[i]->setText(QString::number(i + 1));
////        btn[i].setText(QString::number(i + 1));
//        btn[i]->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
//        btn[i]->setStyleSheet(commStyle);
//        btn[i]->setBaseSize(10,10);
////        btn[i].setCheckable(true);
////        mVecline->push_back(&btn[i]);
//        mVecline.push_back(btn[i]);
////        mVecButton.emplace_back(btn[i]);
////        connect(mVecButton[i], SIGNAL(clicked()), clickMapper, SLOT(map()));
////        connect(mVecButton[i], &mylineedit::clicked, clickMapper, &QSignalMapper::map);
////        connect(mVecline[i], &mylineedit::clicked, [=]{
////            show_detial();
////        });
////        connect(mVecButton[i], &mylineedit::clicked,[=]
//        clickMapper->setMapping(mVecButton[i], i);

//        mVecline[i]->installEventFilter(this);
//    }
////    connect(clickMapper, &mylineedit::clicked, [=]{
////        show_detial();
////    });
//    connect(clickMapper, &QSignalMapper::mappedString,[=]{
//            show_detial();
//        });
////    mVecline[0]->setChecked(true);
////    connect(clickMapper, SIGNAL(mapped(int)), this, SLOT(widgetClick(int)));

//    int colCount = COL;

//    int iitem = 0;  //余数
//    int row = 0;
//    qDebug()<<"this";

//    //只显示一行的时候
//    if(mMaxBuildings <= colCount){
//        row = 1;
//        colCount = mMaxBuildings;
//        iitem = colCount;

//        for(int i = 0 ; i < COL; ++i){
//            ui->gridLayout->addWidget(mVecline.at(i),0,i,1,1);
//            ui->gridLayout->setSpacing(2);
//            //显示一行总数小于COL时也要显示四个，大于总数的地方用透明按钮做填充，否则会显示异常
//            if(i >= mMaxBuildings){
//                btn[i]->setText("");
////                btn[i].setStyleSheet(transbg_skin);
//                btn[i]->setEnabled(false);
//            }
//        }
//        return;
//    }else{
//        iitem = mMaxBuildings % COL;
//        row = mMaxBuildings / COL;
//        //如果有余数则向上取整  说明最后一行数小于规定的数目；余数则是最后一行要显示的数个数
//        if(iitem){
//            row+=1;
//        }
//    }

//    int itemIndex = 0;

//    //行数
//    for(int i = 0;i < row; ++i){
//        int btnIndx=0;
//        //如果是最后一次循环且最后行元素不是初始col个，则重新赋值col
//        if(i + 1 == row && iitem){
//            colCount = iitem;
//        }
//        //列数
//        for(int j = 0;j < colCount; j++){
//            ui->gridLayout->addWidget(mVecline.at(itemIndex),i,j + btnIndx);
//            ++itemIndex;
//            ++btnIndx;
//        }
//    }
//    ui->gridLayout->setSpacing(16);

//}

//void MainWindow::Initserial(){
//    QCheckBox *checkbox[8];
//    QLabel *name[8];
//    QLabel *ID[8];
//    QPushButton *userDelBtn[8];
//    QPushButton *userEdtBtn[8];

//    for(int i = 0; i < 8; i++)
//        {
//            checkbox[i] = new QCheckBox(this);
////            checkbox[i] -> move(52,20+100*i);

//            name[i] = new QLabel(this);
////            name[i] -> move(160,20+100*i);

//            ID[i] = new QLabel(this);
////            ID[i] -> move(327,320+100*i);

//            userEdtBtn[i] = new QPushButton(this);
////            userEdtBtn[i] -> move(563,320+100*i);
////            userEdtBtn[i] -> setIcon(QIcon(":/images/edit"));
//            userEdtBtn[i] -> setIconSize(QSize(28,28));
//            userEdtBtn[i] -> setStyleSheet("background-color:white; border:none");

//            userDelBtn[i] = new QPushButton(this);
////            userDelBtn[i] -> move(635,320+100*i);
////            userDelBtn[i] -> setIcon(QIcon(":/images/delete"));
//            userDelBtn[i] -> setIconSize(QSize(28,28));
//            userDelBtn[i] -> setStyleSheet("background-color:white; border:none");
//        }
////    for(int i=0;i<8;i++)
////            {
////                delete checkbox[i];         //复选框
////                delete name[i];             //用户姓名标签
////                delete ID[i];               //用户工号标签
////                delete userDelBtn[i];       //"删除"按钮
////                delete userEdtBtn[i];       //"编辑"按钮
////            }
//}

//void MainWindow::Initserial()
//{
//    //首先使用布局，将每一个要显示的控件添加进来
////     QHBoxLayout *qhbl = new QHBoxLayout();
////      for (size_t i = 0; i < num; i++)
////      {
//////          CoilInforn *w = new CoilInforn();   //动态生成控件
//////          mylineedit::QLineEdit *w=new mylineedit::QLineEdit();
////          QLineEdit *w=new QLineEdit();
////          w-> move(35,320+100*i);
////          qhbl->addWidget(w);//将控件添加到布局中
////      }
//      //容器显示布局
////      ui->groupbox_main->setLayout(qhbl);


////    QCheckBox *cb[12];
////    QGridLayout *layout = new QGridLayout();
////    int i = 0, j = 0;
////    int k = 0;
////    for (i = 0; i<6; i++)
////    {
////        for (j = 0; j<2; j++)
////        {
////            if (j == 0) k = i;
////            if (j == 1) k = 6 + i;
////            //cb是已经声明好了的QCheckBox类型变量。
////            cb[k] = new QCheckBox(QString::asprintf("%d", k + 1));
////            layout->addWidget(cb[k], i, j);
////        }
////    }

////    layout->setHorizontalSpacing(10);
////    layout->setVerticalSpacing(10);
////    layout->setContentsMargins(10, 10, 10, 10);
////    ui->groupBox->setLayout(layout);//ui->groupBox又是另外一个窗体

//}
