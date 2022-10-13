#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <myweight.h>
#include <QLineEdit>
#include <mytextedit.h>
#include <mylineedit.h>
#include <QVector>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include <QModbusDataUnit>
#include <QModbusRtuSerialMaster>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void show_detial(QString name);
//    void Initserial();
    void init();
    void updata_content();

    const int maxnum=15;
    int colnum=5;
    //Vector: maxnum:地址个数->读取数据的个数
    QVector<QVector<float> >cont=QVector<QVector<float>> (maxnum,QVector<float> (12,0));
    void Initserial();

    void readfromclient(unsigned int startAddress,int number);
    void readReady();
    QString binaryTransfer(int data);
    void decode(QString result,int address);
    float fromFourToFloat(QString result);

public slots:
    void ShowName(QString name);
    void readData();
private:
    void InitUi();


private:
    QSignalMapper* signalMapper;
    QStringList names;



private:
    Ui::MainWindow *ui;
    QModbusClient *modbustoolDevice;//Modbus对象
};
#endif // MAINWINDOW_H
