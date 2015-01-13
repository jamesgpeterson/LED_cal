#include "SerialBuffer.h"

CSerialBuffer::CSerialBuffer()
{
    m_serialPort = new QSerialPort(this);

}

CSerialBuffer::~CSerialBuffer()
{
    if (m_serialPort)
    {
        m_serialPort->clear();
        m_serialPort->close();
    }
}

bool CSerialBuffer::openPort(QString portName)
{
    //
    // First close the port if it is already open
    //
    if (m_serialPort->isOpen())
    {
        m_serialPort->close();
    }

    //
    // Return if the name is not specified
    //
    if (portName.isEmpty() || portName.startsWith("<"))
    {
        return(false);
    }

    m_serialPort->setPortName(portName);
    m_serialPort->open(QSerialPort::ReadWrite);
    if (!m_serialPort->isOpen())
    {
        return(false);
    }

    m_serialPort->setBaudRate(QSerialPort::Baud115200);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
    m_serialPort->setDataTerminalReady(true);

    m_serialPort->clearError();
    m_serialPort->clear();
    m_serialPort->flush();

    return(true);
}
