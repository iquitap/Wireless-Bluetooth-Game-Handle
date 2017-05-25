#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "portlistener.h"
#include "windows.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    lastaddr = 0;
    flag = 0;
    pause = 1;
    myCom = NULL;
    addrH = 0;
    addrL = 0;
    getH = false;
    getL = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::readMyCom() //读取串口数据并显示出来
{
    if(pause) return;
    QByteArray bytes = myCom->readAll();
    QString str;
    int addr = bytes[0] & 0x000000FF;
    addr |= ((bytes[1] << 8) & 0x0000FF00);
    addr |= ((bytes[2] << 16) & 0x00FF0000);
    addr |= ((bytes[3] << 24) & 0xFF000000);


    //check it is high or low
    if(addr & 0x4)//high 4 bit
    {
        if(getL)//low bits should follow the high bits
            getL = false;
        addrH = addr &0xff;
        getH = true;
    }
    else
    {
        addrL = addr &0xff;
        getL = true;
    }
    if(!(getL && getH)) //if not the full data
    {
        return;
    }
    getL = false;
    getH = false;
    addr = (addrH & 0xf0) | ((addrL & 0xf0) >> 4);
    if(addr & 0x00000080){
        addr |= 0xffffff00;
    }

    //ui->textBrowser->insertPlainText(temp);
    char * direct = "stop";
    switch(addrL & 0x3)
    {
    case 0 :
        direct = "button";
        if(addr&0x1){
            keybd_event('A',0,0,0);//按下a键
        }
        if(addr&0x2){
            keybd_event('S',0,0,0);//按下s键
        }
        return;
    case 2 : flag = 1;break;
    case 3 : flag = 0;break;
    default: return;
    }
    if(flag)
    {
        keybd_event('A',0,KEYEVENTF_KEYUP,0);//松开a键
        keybd_event('S',0,KEYEVENTF_KEYUP,0);//松开s键
        if(lastaddr * addr <= 0)
        {
            keybd_event(VK_DOWN, 0, KEYEVENTF_KEYUP, 0);
            keybd_event(VK_UP, 0, KEYEVENTF_KEYUP, 0);
        }
        if(-20 <= addr && addr <= 20)
        {
            keybd_event(VK_DOWN, 0, KEYEVENTF_KEYUP, 0);
            keybd_event(VK_UP, 0, KEYEVENTF_KEYUP, 0);
        }
        else if(addr > 20)
        {
            keybd_event(VK_DOWN, 0, 0, 0);
            direct = "down";
            //keybd_event(VK_DOWN, 0, KEYEVENTF_KEYUP, 0);
        }
        else if(addr < -20)
        {
            addr = -addr;
            keybd_event(VK_UP, 0, 0, 0);
            direct = "up";
            //keybd_event(VK_UP, 0, KEYEVENTF_KEYUP, 0);
        }
    }
    else
    {
        if(lastaddr * addr <= 0)
        {
            keybd_event(VK_LEFT, 0, KEYEVENTF_KEYUP, 0);
            keybd_event(VK_RIGHT, 0, KEYEVENTF_KEYUP, 0);
        }
        if(-20 <= addr && addr <= 20)
        {
            keybd_event(VK_LEFT, 0, KEYEVENTF_KEYUP, 0);
            keybd_event(VK_RIGHT, 0, KEYEVENTF_KEYUP, 0);
        }
        else if(addr > 20)
        {
            keybd_event(VK_LEFT, 0, 0, 0);
            direct = "left";
            //keybd_event(VK_DOWN, 0, KEYEVENTF_KEYUP, 0);
        }
        else if(addr < -20)
        {
            keybd_event(VK_RIGHT, 0, 0, 0);
            direct = "right";
            //keybd_event(VK_UP, 0, KEYEVENTF_KEYUP, 0);
        }
    }
    lastaddr = addr;
    ui->textBrowser->clear();
    //将串口的数据显示在窗口的文本浏览器中
    str.sprintf("the %s is %d\n", direct, addr);
    //读取串口缓冲区的所有数据给临时变量temp
    ui->textBrowser->insertPlainText(str);

}

void MainWindow::on_pushButton_clicked()
{
    if(myCom)
    {
        pause = 0;
        return;
    }
    QSerialPort *my_serialport = new QSerialPort("COM10");
    my_serialport->setPortName("COM10");
    //my_serialport->open(QIODevice::ReadWrite);
    my_serialport->setBaudRate(115200);
    my_serialport->setParity(QSerialPort::NoParity);
    my_serialport->setDataBits(QSerialPort::Data8);
    my_serialport->setStopBits(QSerialPort::OneStop);
    my_serialport->setFlowControl(QSerialPort::NoFlowControl);
    PortListener * listener = new PortListener(my_serialport);
    listener->connect(my_serialport, SIGNAL(readyRead()), this, SLOT(readMyCom()));
    my_serialport->open(QIODevice::ReadWrite);
    myCom = my_serialport;
}

void MainWindow::on_pushButton_2_clicked()
{
    pause = 1;
}
