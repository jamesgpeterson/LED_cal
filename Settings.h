/*!
 * @file Settings.h
 * @brief Definition of the CSettings class
 *
 * @author    	J. Peterson
 * @date        01/23/2015
 * @copyright	(C) Copyright Enercon Technologies 2015, All rights reserved.
 *
 * Revision History
 * ----------------
 *  Version | Author       | Date        | Description
 *  :--:    | :-----       | :--:        | :----------
 *   1      | J. Peterson  | 01/23/2015  | initial version
 *
*/

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QString>

class CSettings
{
public:
    CSettings();
    ~CSettings();
    bool load(QString filename);

public:
    QString m_reportFile;     // file name of the report file
    QString m_serialPort;     // name of the desired serial port
    QString m_versionARM;     // version of ARM firmware
    QString m_versionDSP;     // version of DSP firmware
    QString m_versionFPGA;    // version of FPGA firmware

private:
    QSettings  *m_qSettings;  //! QT QSettings object that provides the interface to the ini file
};


#endif // SETTINGS_H
