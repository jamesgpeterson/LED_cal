#include <QSerialPort>

#define INPUT_BUFFER_SIZE

class CSerialBuffer : QObject
{
    Q_OBJECT

public:
    CSerialBuffer();
    ~CSerialBuffer();

public:
    bool openPort(QString serialPort);
    bool checkForEcho();
    void flush();
    bool writeLine(const char *command);
    bool readLine(char *buffer, int bufferSize, int timeoutMS);
    QString readString();


private:
#if 0
    void snooze(int ms);
#endif

private:
    QSerialPort   *m_serialPort;
    int            m_timeoutMS;
};

