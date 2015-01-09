
#include "Settings.h"

CSettings::CSettings()
{
    m_logFile  = "Spyglass_LED_Calibration.log";
    m_commPort = "";

    m_qSettings = new QSettings("LED_Cal.ini", QSettings::IniFormat);
    m_qSettings->sync();

    m_logFile  = m_qSettings->value("logFile", m_logFile).toString();
    m_commPort = m_qSettings->value("commPort", m_commPort).toString();
}

CSettings::~CSettings()
{
    m_qSettings->setValue("logFile", m_logFile);
    m_qSettings->setValue("commPort", m_commPort);

    m_qSettings->sync();
}

