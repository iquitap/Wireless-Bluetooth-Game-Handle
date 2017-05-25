
/**
 * @file PortListener.cpp
 * @brief PortListener Implementation.
 * @see PortListener.h
 */


/*
==============
<INIT>
==============
*/
#include <QTextStream>
#include <QtSerialPort/qserialport.h>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "portlistener.h"
#include "stdio.h"

/*
==============
<CONSTRUCTORS>
==============
*/

PortListener::PortListener(QSerialPort *port, QObject * parent):
    QObject(parent)
{
    this->port = port;
}


/*
==============
<DESTRUCTOR>
==============
*/


/*
==============
<STATIC>
==============
*/


/*
==============
<SLOTS>
==============
*/

void PortListener::receive()
{
    char temp = port->readAll().toInt();
    printf("temp is %d\n", temp);
}


/*
==============
<VIRTUAL>
==============
*/


/*
==============
<NON-VIRTUAL>
==============
*/

