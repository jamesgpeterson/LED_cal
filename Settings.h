#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QString>

class CSettings
{
public:
    CSettings();
    ~CSettings();

    void loadSettings();
    void saveSettings();

public:
    QString m_logFile;
    QString m_commPort;

private:
    QSettings  *m_qSettings;
};


#endif // SETTINGS_H
