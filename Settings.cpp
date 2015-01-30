/*!
 * @file Settings.cpp
 * @brief Implements access to the ini file
 *
 * @author    	J. Peterson
 * @date        01/12/2015
 * @copyright	(C) Copyright Enercon Technologies 2015, All rights reserved.
 *
 * Revision History
 * ----------------
 *  Version | Author       | Date        | Description
 *  :--:    | :-----       | :--:        | :----------
 *   1      | J. Peterson  | 01/23/2015  | initial version
 *
*/

#include "Settings.h"

//
// INI file name
//
const char *c_iniFile = "LED_Cal.ini";

//
// Default settings value-key pairs
//
const char *c_ReportFile_key      = "LedCalReport.txt";
const char *c_ReportFile_default  = "Spyglass_LED_Calibration.log";

const char *c_SerialPort_key      = "SerialPort";
const char *c_SerialPort_default  = "";

const char *c_VersionARM_key      = "version/ARM";
const char *c_VersionARM_default  = "2.1.2250";

const char *c_VersionDSP_key      = "version/DSP";
const char *c_VersionDSP_default  = "2.1.2250";

const char *c_VersionFPGA_key     = "version/FPGA";
const char *c_VersionFPGA_default = "2.02";



/*!
 * @brief constructor for the CSettings class
 *
 * @param[in] none
 * @param[out] none
 * @return none
 *
 * The ini file is opened and all parameters are read.
 * Any parameter that is not listed in the ini file will be set
 * to the default value.  If the ini file can not be opened then
 * all parameters will be set to their defaults.
 *
 * Note: There should only be one instance of this class created.
 *
 * @author J. Peterson
 * @date 01/12/2015
*/
CSettings::CSettings()
{
    m_qSettings = new QSettings(c_iniFile, QSettings::IniFormat);
    m_qSettings->sync();

    m_reportFile  = m_qSettings->value(c_ReportFile_key, c_ReportFile_default).toString();
    m_serialPort  = m_qSettings->value(c_SerialPort_key, c_SerialPort_default).toString();
    m_versionARM  = m_qSettings->value(c_VersionARM_key, c_VersionARM_default).toString();
    m_versionDSP  = m_qSettings->value(c_VersionDSP_key, c_VersionDSP_default).toString();
    m_versionFPGA = m_qSettings->value(c_VersionFPGA_key, c_VersionFPGA_default).toString();
}


/*!
 * @brief destructor for the CSettings class
 *
 * @param[in] none
 * @param[out] none
 * @return none
 *
 * The current values of all settings are written to the ini file
 * before destroying the object.
 *
 * @author J. Peterson
 * @date 01/12/2015
*/
CSettings::~CSettings()
{
    m_qSettings->setValue(c_ReportFile_key, m_reportFile);
    m_qSettings->setValue(c_SerialPort_key, m_serialPort);
    m_qSettings->setValue(c_VersionARM_key, m_versionARM);
    m_qSettings->setValue(c_VersionDSP_key, m_versionDSP);
    m_qSettings->setValue(c_VersionFPGA_key, m_versionFPGA);

    m_qSettings->sync();
}



