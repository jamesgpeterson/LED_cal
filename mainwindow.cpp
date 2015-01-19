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
#include "Snooze.h"


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
    ui->pushButton->setEnabled(false);

    //
    // Check that the operator field is filled in.
    //
    if (ui->lineEdit_operator->text().isEmpty())
    {
        QString title = QFileInfo( QCoreApplication::applicationFilePath() ).fileName();
        QString msg = "An operator name must be entered.";
        QMessageBox::warning(this, title, msg, QMessageBox::Ok);
        ui->lineEdit_operator->setFocus();
        ui->pushButton->setEnabled(true);
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
        ui->pushButton->setEnabled(true);
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
        ui->pushButton->setEnabled(true);
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
        ui->pushButton->setEnabled(true);
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
        ui->pushButton->setEnabled(true);
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
    i = ver_ARM.indexOf(" ");
    ver_ARM.truncate(i);

    QString ver_DSP  = m_serialBuffer.readString();
    i = ver_DSP.indexOf("DSP: ");
    ver_DSP.remove(0, i+5);
    i = ver_DSP.indexOf(" ");
    ver_DSP.truncate(i);

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
    m_serialBuffer.writeLine("em_style=0");

    //
    // Update the current exposure
    //
    updateExposure();

    //
    // Update the current and voltage
    //
    updateCurrentAndVoltage();
    updateDACValues();

    //
    // Find the low-end calibration for LED 1
    //
    findHighCalibration();
    findLowCalibration();


    ui->pushButton->setEnabled(true);
}


void MainWindow::updateExposure()
{
    m_serialBuffer.writeLine("em=-1");
    int zones[25];
    for (int i=0; i<25; i++)
    {
        zones[i] = -1;
    }

    // Dump the first line
    m_serialBuffer.readString();
    for (int i=0; i<5; i++)
    {
        QString response = m_serialBuffer.readString();
        QStringList list = response.split(QRegExp("\\W+"), QString::SkipEmptyParts);
        if (list.size() != 5)
        {
            QString title = QFileInfo( QCoreApplication::applicationFilePath() ).fileName();
            QString msg = "Unexpected response:\n";
            msg.append(response);
            QMessageBox::warning(this, title, msg, QMessageBox::Ok);
            ui->lineEdit_serialNumber->setFocus();
            return;
        }
        for (int j=0; j<list.size(); j++)
        {
            zones[i*5+j] = list[j].toInt();
        }
    }

    int z = 0;
    QString numStr;
    ui->lineEdit_em_11->setText(numStr.setNum(zones[z++]));
    ui->lineEdit_em_12->setText(numStr.setNum(zones[z++]));
    ui->lineEdit_em_13->setText(numStr.setNum(zones[z++]));
    ui->lineEdit_em_14->setText(numStr.setNum(zones[z++]));
    ui->lineEdit_em_15->setText(numStr.setNum(zones[z++]));

    ui->lineEdit_em_21->setText(numStr.setNum(zones[z++]));
    ui->lineEdit_em_22->setText(numStr.setNum(zones[z++]));
    ui->lineEdit_em_23->setText(numStr.setNum(zones[z++]));
    ui->lineEdit_em_24->setText(numStr.setNum(zones[z++]));
    ui->lineEdit_em_25->setText(numStr.setNum(zones[z++]));

    ui->lineEdit_em_31->setText(numStr.setNum(zones[z++]));
    ui->lineEdit_em_32->setText(numStr.setNum(zones[z++]));
    ui->lineEdit_em_33->setText(numStr.setNum(zones[z++]));
    ui->lineEdit_em_34->setText(numStr.setNum(zones[z++]));
    ui->lineEdit_em_35->setText(numStr.setNum(zones[z++]));

    ui->lineEdit_em_41->setText(numStr.setNum(zones[z++]));
    ui->lineEdit_em_42->setText(numStr.setNum(zones[z++]));
    ui->lineEdit_em_43->setText(numStr.setNum(zones[z++]));
    ui->lineEdit_em_44->setText(numStr.setNum(zones[z++]));
    ui->lineEdit_em_45->setText(numStr.setNum(zones[z++]));

    ui->lineEdit_em_51->setText(numStr.setNum(zones[z++]));
    ui->lineEdit_em_52->setText(numStr.setNum(zones[z++]));
    ui->lineEdit_em_53->setText(numStr.setNum(zones[z++]));
    ui->lineEdit_em_54->setText(numStr.setNum(zones[z++]));
    ui->lineEdit_em_55->setText(numStr.setNum(zones[z++]));
    qApp->processEvents();

    m_totalExposure = 0;
    for (int i=0; i<25; i++)
    {
        m_totalExposure += zones[i];
    }
}

void MainWindow::updateCurrentAndVoltage()
{
    m_serialBuffer.writeLine("ledvi");
    QString response = m_serialBuffer.readString();
    double V1, V2, I1, I2, index;
    QString tmpStr;

    tmpStr = response;
    index = tmpStr.indexOf("V1:");
    tmpStr.remove(0, index+3);
    tmpStr.truncate(tmpStr.indexOf(","));
    V1 = tmpStr.toDouble();

    tmpStr = response;
    index = tmpStr.indexOf("I1:");
    tmpStr.remove(0, index+3);
    tmpStr.truncate(tmpStr.indexOf(","));
    I1 = tmpStr.toDouble();

    tmpStr = response;
    index = tmpStr.indexOf("V2:");
    tmpStr.remove(0, index+3);
    tmpStr.truncate(tmpStr.indexOf(","));
    V2 = tmpStr.toDouble();

    tmpStr = response;
    index = tmpStr.indexOf("I2:");
    tmpStr.remove(0, index+3);
    I2 = tmpStr.toDouble();

    QString numStr;
    ui->lineEdit_volts1->setText(numStr.setNum(V1));
    ui->lineEdit_volts2->setText(numStr.setNum(V2));
    ui->lineEdit_amps1->setText(numStr.setNum(I1));
    ui->lineEdit_amps2->setText(numStr.setNum(I2));
    qApp->processEvents();
}

void MainWindow::updateDACValues()
{
    m_serialBuffer.writeLine("led_dac");
    QString response = m_serialBuffer.readString();
    double X1, X2, index;
    QString tmpStr;

    tmpStr = response;
    index = tmpStr.indexOf("led1=");
    tmpStr.remove(0, index+5);
    tmpStr.truncate(tmpStr.indexOf(","));
    X1 = tmpStr.toDouble();

    tmpStr = response;
    index = tmpStr.indexOf("led2=");
    tmpStr.remove(0, index+5);
    X2 = tmpStr.toDouble();

    QString numStr;
    ui->lineEdit_dac1->setText(numStr.setNum(X1));
    ui->lineEdit_dac2->setText(numStr.setNum(X2));
    qApp->processEvents();
}

void MainWindow::setDACValues(int dac1, int dac2)
{
    QString command = QString("led_dac=%1,%2").arg(dac1).arg(dac2);
    m_serialBuffer.writeLine(command.toLocal8Bit().data());
    updateDACValues();
#if 0
    m_serialBuffer.writeLine("led_dac");
    QString response = m_serialBuffer.readString();
    double X1, X2, index;
    QString tmpStr;

    tmpStr = response;
    index = tmpStr.indexOf("led1=");
    tmpStr.remove(0, index+5);
    tmpStr.truncate(tmpStr.indexOf(","));
    X1 = tmpStr.toDouble();

    tmpStr = response;
    index = tmpStr.indexOf("led2=");
    tmpStr.remove(0, index+5);
    X2 = tmpStr.toDouble();

    QString numStr;
    ui->lineEdit_dac1->setText(numStr.setNum(X1));
    ui->lineEdit_dac2->setText(numStr.setNum(X2));
    qApp->processEvents();
#endif
}


void MainWindow::findLowCalibration()
{
    int X1 = 0;
    int X2 = 5000;

    while ( X1 < (X2-1) )
    {
        int M = (X1+X2)/2;
        setDACValues(M, 0);
        snooze(60);
        updateExposure();
        if (m_totalExposure == 0)
        {
            X1 = M;
        }
        else
        {
            X2 = M;
        }
    }

}

void MainWindow::findHighCalibration()
{
}



