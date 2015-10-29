#include "serialport.h"

SerialPort::SerialPort(QObject *parent) : QObject(parent)
{
    connect(&serialport, SIGNAL(readyRead()), this, SLOT(emit_ready_read()));
}

SerialPort::~SerialPort()
{
    serialport.close();
}

bool SerialPort::setPort(QString port)
{
    serialport.setPortName(port);
    if (serialport.open(QIODevice::ReadWrite))
    {
    serialport.setBaudRate(QSerialPort::Baud38400);
    serialport.setDataBits(QSerialPort::Data8);
    serialport.setParity(QSerialPort::OddParity);
    serialport.setStopBits(QSerialPort::TwoStop);
    serialport.setFlowControl(QSerialPort::NoFlowControl);
    return true;
    }

    return false;
}

int SerialPort::write(QByteArray responce)
{
    return serialport.write(responce);
}

QByteArray SerialPort::read()
{
    return serialport.readAll();
}

QStringList SerialPort::ports()
{
    QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();
    QStringList stringList;
    for(auto p = list.begin(); p != list.end(); p++)
        stringList.append(p->portName());
    return stringList;
}

void SerialPort::close()
{
    serialport.close();
}

void SerialPort::emit_ready_read()
{
    emit ready_read_signal();
}

