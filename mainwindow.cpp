/*!
 * @file mainwindow.cpp
 * @brief Implements the main window class
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

#include <QSerialPortInfo>
#include <QMessageBox>
#include <QFileInfo>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "SerialPortDialog.h"



#define NOT_SELECTED "not selected"


/*!
 * @brief constructor for the main window
 *
 * @param[in] parent QWidget for this dialog
 * @param[out] none
 * @return none
 *
 * @author J. Peterson
 * @date 01/12/2015
*/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //
    // Initialize the comm port list
    //
    //refreshSerialConnections();

    //
    // initialize the windows with the last setting
    //
    ui->lineEdit_logFile->setText(m_settings.m_logFile);
}


/*!
 * @brief destructor for the main window
 *
 * @param[in] none
 * @param[out] none
 * @return none
 *
 * @author J. Peterson
 * @date 01/12/2015
*/
MainWindow::~MainWindow()
{
    //
    // Save the settings
    //
    m_settings.m_logFile = ui->lineEdit_logFile->text();
    m_settings.m_commPort = ui->lineEdit_serialPort->text();

    delete ui;
}


/*!
 * @brief called when the port selection button is pressed
 *
 * @param[in] none
 * @param[out] none
 * @return none
 *
 * @author J. Peterson
 * @date 01/12/2015
*/
void MainWindow::selectSerialPort()
{
    CSerialPortDialog *dlg = new CSerialPortDialog(this);
    if (dlg->exec())
    {
        ui->lineEdit_serialPort->setText(dlg->getSelection());
    }


}

/*!
 * @brief called when the "Start Calibration" button is pressed
 *
 * @param[in] none
 * @param[out] none
 * @return none
 *
 * @author J. Peterson
 * @date 01/12/2015
*/
void MainWindow::startCalibration()
{
    //
    // Check that the operator field is filled in.
    //

    //
    // Check that the serial number field was filled in.

    //
    // Open the Serial Port.
    //
    QString portName = ui->lineEdit_serialPort->text();
    if (portName.isEmpty())
    {
        QString title = QFileInfo( QCoreApplication::applicationFilePath() ).fileName();
        QString msg = "A serial port must be specified/selected.";
        QMessageBox::warning(this, title, msg, QMessageBox::Ok);
        return;
    }

    if (!m_serialBuffer.openPort(portName))
    {
        QString title = QFileInfo( QCoreApplication::applicationFilePath() ).fileName();
        QString msg = "The specified serial port could not be opened.";
        QMessageBox::warning(this, title, msg, QMessageBox::Ok);
        return;
    }

    //
    // Check that the controller is running
    //

    //
    // Check that the scope is connected
    //

    //
    // Get the current calibration values
    //

}



