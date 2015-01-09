
#include "Settings.h"

CSettings::CSettings()
{
    m_logFile  = "Spyglass_LED_Calibration.log";
    m_commPort = "";

    m_settings = new QSettings("LED_Cal.ini", QSettings::IniFormat);
    m_settings->sync();

    m_logFile  = m_settings->value("logFile", m_logFile).toString();
    m_commPort = m_settings->value("commPort", m_commPort).toString();
}

CSettings::~CSettings()
{
    m_settings->setValue("logFile", m_logFile);
    m_settings->setValue("commPort", m_commPort);

    m_settings->sync();
}

