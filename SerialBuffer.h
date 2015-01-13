#include <QSerialPort>

class CSerialBuffer : QObject
{
    Q_OBJECT

public:
    CSerialBuffer();
    ~CSerialBuffer();

public:
    bool openPort(QString serialPort);

private:
    QSerialPort   *m_serialPort;
};

