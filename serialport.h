#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QSerialPort>
#include <QByteArray>
#include <QSerialPortInfo>
#include <QStringList>
#include <QDebug>

class SerialPort : public QObject
{
    Q_OBJECT
public:
    explicit SerialPort(QObject *parent = 0);
    ~SerialPort();
    bool setPort(QString);
    int write(QByteArray);
    QByteArray read();
    QStringList ports();
    void close();

signals:
    void ready_read_signal();

public slots:
    void emit_ready_read();
private:
    QSerialPort serialport;
};

#endif // SERIALPORT_H
