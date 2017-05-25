/**
 * @file PortListener.h
 * @brief Port Listener.
 * @see PortListener
 */

#ifndef PORTLISTENER_H_
#define PORTLISTENER_H_

#include <QObject>

#include <QtSerialPort/qserialport.h>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

/**
 * Port Listener.
 */
class PortListener : public QObject
{
    Q_OBJECT

    QSerialPort * port;


    public:
        /**
         * Constructor.
         *  @param parent parent object.
         */
        PortListener(QSerialPort * port, QObject * parent = 0);

    public slots:
        /**
         * Receive data from serial port.
         */
        void receive();
        /**
         * Report port closing.
         */
};


#endif /*PORTLISTENER_H_*/
