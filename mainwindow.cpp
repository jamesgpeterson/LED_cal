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

#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
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
    // Set the window title.
    //
    QString title = QFileInfo( QCoreApplication::applicationFilePath() ).baseName();
    title.append(" (");
    title.append(VERSION_STRING);
    title.append(")");
    this->setWindowTitle(title);


    //
    // initialize the windows with the last setting
    //
    ui->lineEdit_logFile->setText(m_settings.m_logFile);
    ui->lineEdit_serialPort->setText(m_settings.m_commPort);
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
    if (ui->lineEdit_operator->text().isEmpty())
    {
        QString title = QFileInfo( QCoreApplication::applicationFilePath() ).fileName();
        QString msg = "An operator name must be entered.";
        QMessageBox::warning(this, title, msg, QMessageBox::Ok);
        ui->lineEdit_operator->setFocus();
        return;
    }

    //
    // Check that the serial number field was filled in.
    //
    if (ui->lineEdit_serialNumber->text().isEmpty())
    {
        QString title = QFileInfo( QCoreApplication::applicationFilePath() ).fileName();
        QString msg = "A serial number name must be entered.";
        QMessageBox::warning(this, title, msg, QMessageBox::Ok);
        ui->lineEdit_serialNumber->setFocus();
        return;
    }

    //
    // Check that the COM port field was filled in.
    //
    QString portName = ui->lineEdit_serialPort->text();
    if (portName.isEmpty())
    {
        QString title = QFileInfo( QCoreApplication::applicationFilePath() ).fileName();
        QString msg = "A serial port must be specified/selected.";
        QMessageBox::warning(this, title, msg, QMessageBox::Ok);
        ui->lineEdit_serialPort->setFocus();
        return;
    }

    //
    // Open the COM port
    //
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
    if (m_serialBuffer.checkForEcho() == false)
    {
        QString title = QFileInfo( QCoreApplication::applicationFilePath() ).fileName();
        QString msg = "Communication with the controller could not be established.\n\nPort opend successfully.\nCommands are not echoed.";
        QMessageBox::warning(this, title, msg, QMessageBox::Ok);
        return;
    }

    //
    // Disable excessive echoing of commands
    //
    m_serialBuffer.writeLine("disable_events=1");

    //
    // Check version of the firmware
    //
    int i;
    m_serialBuffer.writeLine("version");
    QString ver_FPGA = m_serialBuffer.readString();
    i = ver_FPGA.indexOf("FPGA: ");
    ver_FPGA.remove(0, i+6);
    QString ver_ARM  = m_serialBuffer.readString();
    i = ver_ARM.indexOf("ARM: ");
    ver_ARM.remove(0, i+5);
    QString ver_DSP  = m_serialBuffer.readString();
    i = ver_DSP.indexOf("DSP: ");
    ver_DSP.remove(0, i+5);
    ui->lineEdit_ver_ARM->setText(ver_ARM);
    ui->lineEdit_ver_DSP->setText(ver_DSP);
    ui->lineEdit_ver_FPGA->setText(ver_FPGA);
    qApp->processEvents();

    //
    // Get the current calibration values
    //
    m_serialBuffer.writeLine("led_cal");
    QString calString = m_serialBuffer.readString();
    QString tmpString;

    i = calString.indexOf("low=");
    calString.remove(0, i+4);
    tmpString = calString;
    i = tmpString.indexOf(" ");
    tmpString.truncate(i);
    int lowCal1 = tmpString.toInt();

    i = calString.indexOf("high=");
    calString.remove(0, i+5);
    tmpString = calString;
    i = tmpString.indexOf(")");
    tmpString.truncate(i);
    int highCal1 = tmpString.toInt();

    i = calString.indexOf("low=");
    calString.remove(0, i+4);
    tmpString = calString;
    i = tmpString.indexOf(" ");
    tmpString.truncate(i);
    int lowCal2 = tmpString.toInt();

    i = calString.indexOf("high=");
    calString.remove(0, i+5);
    tmpString = calString;
    i = tmpString.indexOf(")");
    tmpString.truncate(i);
    int highCal2 = tmpString.toInt();

    QString numberString;
    numberString.setNum(lowCal1);
    ui->lineEdit_LED1_low->setText(numberString);
    numberString.setNum(lowCal2);
    ui->lineEdit_LED2_low->setText(numberString);
    numberString.setNum(highCal1);
    ui->lineEdit_LED1_high->setText(numberString);
    numberString.setNum(highCal2);
    ui->lineEdit_LED2_high->setText(numberString);
    qApp->processEvents();


    //
    // Check that the scope is connected
    //
    m_serialBuffer.writeLine("disable_events=1");
    m_serialBuffer.writeLine("em_style=1");
    m_serialBuffer.writeLine("em=-1");
    QString em_string = m_serialBuffer.readString();
    if (em_string.isEmpty())
    {
        QString title = QFileInfo( QCoreApplication::applicationFilePath() ).fileName();
        QString msg = "Scope not detected.\n\nEnsure that the scope is connected and installed in the calibration fixture.";
        QMessageBox::warning(this, title, msg, QMessageBox::Ok);
        ui->lineEdit_serialNumber->setFocus();
        return;
    }

}



