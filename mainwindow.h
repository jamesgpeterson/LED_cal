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
#include <QString>
#include "Settings.h"
#include "SerialBuffer.h"


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
    void loadSettings();
    void saveSettings();

public slots:
    void selectSerialPort();
    void startCalibration();
    
private:
    Ui::MainWindow *ui;

    CSettings     m_settings;
    CSerialBuffer m_serialBuffer;
};

#endif // MAINWINDOW_H
