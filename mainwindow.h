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
#include "Settings.h"
#include "SerialBuffer.h"

#define VERSION_STRING "0.3"


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
    void loadSettings();
    void saveSettings();
    void updateExposure();
    void updateCurrentAndVoltage();
    void updateDACValues();
    void setDACValues(int dac1, int dac2);
    void findHighCalibration();
    void findLowCalibration();

public slots:
    void selectSerialPort();
    void startCalibration();
    
private:
    Ui::MainWindow *ui;

    CSettings     m_settings;
    CSerialBuffer m_serialBuffer;

    int           m_totalExposure;
};

#endif // MAINWINDOW_H
