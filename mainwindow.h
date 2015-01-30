/*!
 * @file mainwindow.h
 * @brief Declares the main window class
 *
 * @author    	J. Peterson
 * @date        01/12/2015
 * @copyright	(C) Copyright Enercon Technologies 2015, All rights reserved.
 *
 * Revision History
 * ----------------
 *  Version | Author       | Date        | Description
 *  :--:    | :-----       | :--:        | :----------
 *   1      | J. Peterson  | 01/12/2015  | initial version
 *
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMutex>
#include "Settings.h"
#include "SerialBuffer.h"

#define VERSION_STRING "0.6"


namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void timerEvent(QTimerEvent *);
    void errorMessage(QString msg);
    bool yesNoMessage(QString msg);

    bool checkFields();
    void clearInfoFields();
    void clearExposureAndDacFields();

    bool getFirmwareVersion();
    bool getCurrentCalibrationValues();
    bool getDacValues();
    bool getCurrentAndVoltage();
    bool getExposure();

    bool establishConnectionToController();
    bool checkScope();
    bool findCalibration();

    void updateDACValues();
    void setDACValues(int dac1, int dac2);
    void saveCalibration();

public slots:
    void selectSerialPort();
    void startCalibration();
    
private:
    Ui::MainWindow *ui;

    QString       m_operator;
    QString       m_serialNumber;
    QString       m_serialPortName;

    CSettings     m_settings;
    CSerialBuffer m_serialBuffer;
    int           m_timerID;
    QMutex        m_mutex;

    int           m_totalExposure;
    double        m_I1;
    double        m_I2;
    double        m_V1;
    double        m_V2;
    int           m_calibrationLow_1;
    int           m_calibrationHigh_1;
    int           m_calibrationLow_2;
    int           m_calibrationHigh_2;
    int           m_dac1;
    int           m_dac2;
};

#endif // MAINWINDOW_H
