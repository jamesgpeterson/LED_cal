#include <QTimer>
#include <QEventLoop>
#include <QCoreApplication>
#include "SerialBuffer.h"

CSerialBuffer::CSerialBuffer()
{
    m_serialPort = new QSerialPort(this);
    m_timeoutMS = 3000;
}

CSerialBuffer::~CSerialBuffer()
{
    if (m_serialPort)
    {
        m_serialPort->clear();
        m_serialPort->close();
    }
}


/*!
 * @brief Open the specified serial port.
 *
 * @param[in] portName - the name of the serial port connected to the controller.
 * @return true if open was successful, false otherwise
 *
 * @author J. Peterson
 * @date 01/13/2015
*/
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

/*!
 * @brief sleeps for specified milliseconds
 *
 * @author J. Peterson
 * @date 06/11/2014
*/
void CSerialBuffer::snooze(int ms)
{
    QEventLoop loop;
    QTimer::singleShot(ms, &loop, SLOT(quit()));
    loop.exec();
}


void CSerialBuffer::flush()
{
    m_serialPort->clear();
    const int bufferSize = 1024;
    char buffer[bufferSize];
    snooze(1);
    while (readLine(buffer, bufferSize, 1))
    {
        snooze(1);
    }
}



bool CSerialBuffer::checkForEcho()
{
    //
    // Check that the port is open
    //
    if (!m_serialPort->isOpen())
    {
        return(false);
    }

    //
    // Flush the incoming data.
    //
    m_serialPort->clear();

    //
    // Write a new-line
    //
    if (m_serialPort->write("\n") == 0)
    {
        return(false);
    }

    //
    // Check that we got an echo back
    //
    char buffer[3];
    buffer[0] = '\0';
    if ( (readLine(buffer, 3, m_timeoutMS) == false) || (buffer[0] != '\r') )
    {
        return(false);
    }

    return(true);
}


/*!
 * @brief Write a command to the device.
 *
 * @param[in] command - string to be sent to device
 * @return true if write is successful, false otherwise
 *
 * @author J. Peterson
 * @date 01/13/2015
*/
bool CSerialBuffer::writeLine(const char *command)
{
    int bytesWritten = 0;
    int commandLength = strlen(command);

    //
    // Check that the port is open
    //
    if (!m_serialPort->isOpen())
    {
        return(false);
    }

    //
    // Flush the incoming data.
    //
    flush();
    //snooze(1);
   // m_serialPort->clear(QSerialPort::Input);

    //
    // Write the command.
    //
    bytesWritten += m_serialPort->write(command);
    m_serialPort->write("\n");


    //
    // If the bytes written differs fromm the command length plus the terminator,
    // then there must have been an error.
    //
    if (bytesWritten < commandLength)
    {
        return(false);
    }

    //
    // Check that the echoed data is the same as the command sent.
    //
    char *buffer = new char[commandLength+100];
    if (buffer)
    {
        readLine(buffer, commandLength+100, m_timeoutMS);
        if (strncmp(command, buffer, commandLength) != 0)
        {
            delete buffer;
            return(false);
        }
        delete buffer;
    }

    return(true);
}


/*!
 * @brief reads incoming data from the instrument
 *
 * @param[in] buffer - place to put the responce
 * @param[in] bufferSize - size of the buffer
 * @param[in] msTimeout - timeout period between successful reads
 * @return true if read is successful, false otherwise
 *
 * @author J. Peterson
 * @date 06/01/2014
*/
bool CSerialBuffer::readLine(char *buffer, int bufferSize, int timeoutMS)
{

    //
    // Initialize the out-going buffer
    //
    int index = 0;
    buffer[index] = '\0';
    int tickCount = 0;

    //
    // Check that the port is open
    //
    if (!m_serialPort->isOpen())
    {
        return(false);
    }

    while ((index < bufferSize-1) && (tickCount < timeoutMS) )
    {
        if (m_serialPort->read(&buffer[index], 1) > 0)
        {
            index++;
            buffer[index] = '\0';
            if (buffer[index-1] == '\n')
            {
                break;
            }
        }
        else
        {
            QCoreApplication::instance()->processEvents();
            snooze(1);
            tickCount++;
        }
    }

    return (tickCount < timeoutMS);
}


QString CSerialBuffer::readString()
{
    const int bufferSize = 1024;
    char buffer[bufferSize];
    QString str;

    //
    // Initialize the out-going buffer
    //
    int index = 0;
    buffer[index] = '\0';
    int tickCount = 0;

    //
    // Check that the port is open
    //
    if (!m_serialPort->isOpen())
    {
        return(str);
    }

    while ((index < bufferSize-1) && (tickCount < m_timeoutMS) )
    {
        if (m_serialPort->read(&buffer[index], 1) > 0)
        {
            index++;
            buffer[index] = '\0';
            if (buffer[index-1] == '\n')
            {
                break;
            }
        }
        else
        {
            QCoreApplication::instance()->processEvents();
            snooze(1);
            tickCount++;
        }
    }

    str.append(buffer);

    return (str);
}

