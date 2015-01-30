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
    ui->lineEdit_logFile->setText(m_settings.m_reportFile);
    m_serialPortName = m_settings.m_serialPort;
    ui->lineEdit_serialPort->setText(m_serialPortName);

    //
    // initial DAC values
    //
    m_dac1 = 0;
    m_dac2 = 0;

    //
    // Start timer
    //
    m_timerID = startTimer(3000);
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
    killTimer(m_timerID);

    //
    // Save the settings
    //
    m_settings.m_reportFile = ui->lineEdit_logFile->text();
    m_settings.m_serialPort = ui->lineEdit_serialPort->text();

    delete ui;
}


void MainWindow::timerEvent(QTimerEvent *)
{
    //
    // Try to lock the mutex.
    // If we can't get it it means that a calibration is in progress.
    //
    if (!m_mutex.tryLock())
    {
        return;
    }

    //
    // Try to open the serial port
    //
    if (!m_serialBuffer.openPort(m_serialPortName))
    {
        m_mutex.unlock();
        clearInfoFields();
        ui->label_status->setText("idle: No serial connection");
        qApp->processEvents();
        return;
    }

    //
    // See if the controller is running
    //
    if (m_serialBuffer.checkForEcho() == false)
    {
        m_mutex.unlock();
        clearInfoFields();
        ui->label_status->setText("idle: serial connection opened, no communication with controller");
        qApp->processEvents();
        return;
    }

    //
    // Turn off the event echoing
    //
    if (    (!m_serialBuffer.writeLine("disable_events=1"))
         || (!m_serialBuffer.writeLine("em_style=0")) )
    {
        m_mutex.unlock();
        clearInfoFields();
        ui->label_status->setText("idle: communication with controller established, but no response to commands.");
        qApp->processEvents();
        return;
    }
    m_serialBuffer.readString();

    //
    // Get the firmware version
    //
    getFirmwareVersion();

    //
    // Get the current calibration values
    //
    getCurrentCalibrationValues();

    //
    // Read the Current and Voltage data
    //
    getCurrentAndVoltage();

    //
    // Read the exposure data
    //
    if (!getExposure())
    {
        ui->label_status->setText("idle: communication with controller established, no scope detected");
        clearExposureAndDacFields();
        qApp->processEvents();
    }
    else
    {
        ui->label_status->setText("idle: communication with controller established, scope detected");
        //
        // Read the LED DAC values
        //
        getDacValues();
        qApp->processEvents();
    }


    //
    // Unlock the mutex
    //
    m_mutex.unlock();
}

void MainWindow::errorMessage(QString msg)
{
    QString title = QFileInfo( QCoreApplication::applicationFilePath() ).fileName();
    QMessageBox::warning(this, title, msg, QMessageBox::Ok);
    return;
}

bool MainWindow::yesNoMessage(QString msg)
{
    QString title = QFileInfo( QCoreApplication::applicationFilePath() ).fileName();
    if (QMessageBox::warning(this, title, msg, QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
    {
        return(true);
    }
    return(false);
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

    m_mutex.lock();

    if (    !checkFields()
         || !establishConnectionToController()
         || !getCurrentCalibrationValues()
         || !checkScope() )
    {
        ui->pushButton->setEnabled(true);
        m_mutex.unlock();
        return;
    }

    if (findCalibration())
    {
        saveCalibration();
    }

    m_serialBuffer.writeLine("led=0");

    m_mutex.unlock();
    ui->pushButton->setEnabled(true);
}


/*!
 * @brief
 *
 * @param[in] none
 * @param[out] none
 * @return none
 *
 * @author J. Peterson
 * @date 01/23/2015
*/
bool MainWindow::checkFields()
{
    //
    // Check that the operator field is filled in.
    //
    m_operator = ui->lineEdit_operator->text();
    if (m_operator.isEmpty())
    {
        errorMessage("An operator name must be entered.");
        ui->lineEdit_operator->setFocus();
        return(false);
    }

    //
    // Check that the serial number field was filled in.
    //
    m_serialNumber = ui->lineEdit_serialNumber->text();
    if (m_serialNumber.isEmpty())
    {
        errorMessage("A serial number name must be entered.");
        ui->lineEdit_serialNumber->setFocus();
        return(false);
    }

    //
    // Check that the COM port field was filled in.
    //
    m_serialPortName = ui->lineEdit_serialPort->text();
    if (m_serialPortName.isEmpty())
    {
        errorMessage("A serial port must be specified/selected.");
        ui->lineEdit_serialPort->setFocus();
        return(false);
    }

    return(true);

}


/*!
 * @brief
 *
 * @param[in] none
 * @param[out] none
 * @return none
 *
 * @author J. Peterson
 * @date 01/23/2015
*/
bool MainWindow::establishConnectionToController()
{
    //
    // Open the COM port
    //
    ui->label_status->setText("Opening Serial Port...");
    if (!m_serialBuffer.openPort(m_serialPortName))
    {
        QString msg = "The specified serial port could not be opened.";
        errorMessage(msg);
        return(false);
    }

    //
    // Check that the controller is running
    //
    ui->label_status->setText("Checking communication with controller...");
    if (m_serialBuffer.checkForEcho() == false)
    {
        QString msg = "Communication with the controller could not be established.\n\nPort opend successfully.\nCommands are not echoed.";
        errorMessage(msg);
        return(false);
    }

    //
    // Disable excessive echoing of commands
    //
    ui->label_status->setText("Initializing controller settings...");
    m_serialBuffer.writeLine("disable_events=1");
    m_serialBuffer.readString();
#if 0
    m_serialBuffer.writeLine("gamma=1.0");
    m_serialBuffer.readString();
    m_serialBuffer.writeLine("cc_reset");
    m_serialBuffer.readString();
#endif

    //
    // Check version of the firmware
    //
    ui->label_status->setText("Checking firmware version...");
    int i;
    m_serialBuffer.writeLine("version");
    QString ver_FPGA = m_serialBuffer.readString();
    i = ver_FPGA.indexOf("FPGA: ");
    ver_FPGA.remove(0, i+6);
    i = ver_FPGA.indexOf("\r\n");
    ver_FPGA.truncate(i);

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

    // jgp - the versions should come from the ini file
    // jgp - should give the option to continue
    if (   (ver_ARM != m_settings.m_versionARM)
           || (ver_DSP != m_settings.m_versionDSP)
           || (ver_FPGA != m_settings.m_versionFPGA) )
    {
        if (!yesNoMessage("Controller version does not match expected.\nContinue?"))
        {
            return(false);
        }
    }

    return(true);
}

/*!
 * @brief
 *
 * @param[in] none
 * @param[out] none
 * @return none
 *
 * @author J. Peterson
 * @date 01/23/2015
*/
bool MainWindow::getCurrentCalibrationValues()
{
    m_serialBuffer.writeLine("led_cal");
    QString calString = m_serialBuffer.readString();
    QString tmpString;
    QString numberString;

    int i;
    i = calString.indexOf("low=");
    if (i >= 0)
    {
        calString.remove(0, i+4);
        tmpString = calString;
        i = tmpString.indexOf(" ");
        if (i >= 0)
        {
            tmpString.truncate(i);
            int lowCal1 = tmpString.toInt();
            numberString.setNum(lowCal1);
            ui->lineEdit_LED1_low->setText(numberString);
        }
    }

    i = calString.indexOf("high=");
    if (i >= 0)
    {
        calString.remove(0, i+5);
        tmpString = calString;
        i = tmpString.indexOf(")");
        if (i >= 0)
        {
            tmpString.truncate(i);
            int highCal1 = tmpString.toInt();
            numberString.setNum(highCal1);
            ui->lineEdit_LED1_high->setText(numberString);
        }
    }

    i = calString.indexOf("low=");
    if (i >= 0)
    {
        calString.remove(0, i+4);
        tmpString = calString;
        i = tmpString.indexOf(" ");
        if (i >= 0)
        {
            tmpString.truncate(i);
            int lowCal2 = tmpString.toInt();
            numberString.setNum(lowCal2);
            ui->lineEdit_LED2_low->setText(numberString);
        }
    }

    i = calString.indexOf("high=");
    if (i >= 0)
    {
        calString.remove(0, i+5);
        tmpString = calString;
        if (i >= 0)
        {
            i = tmpString.indexOf(")");
            tmpString.truncate(i);
            int highCal2 = tmpString.toInt();
            numberString.setNum(highCal2);
            ui->lineEdit_LED2_high->setText(numberString);
        }
    }

    qApp->processEvents();

    return(true);
}

/*!
 * @brief Check that the scope is connected
 *
 * @param[in] none
 * @param[out] none
 * @return none
 *
 * @author J. Peterson
 * @date 01/23/2015
*/
bool MainWindow::checkScope()
{
    ui->label_status->setText("Checking for scope...");
    m_serialBuffer.writeLine("em_style=1");
    m_serialBuffer.readString();
    m_serialBuffer.writeLine("em=-1");
    QString em_string = m_serialBuffer.readString();
    if (em_string.isEmpty())
    {
        QString msg = "Scope not detected.\n\nEnsure that the scope is connected and installed in the calibration fixture.";
        errorMessage(msg);
        return(false);
    }
    m_serialBuffer.writeLine("em_style=0");

    return(true);
}



/*!
 * @brief
 *
 * @param[in] none
 * @param[out] none
 * @return none
 *
 * @author J. Peterson
 * @date 01/23/2015
*/
bool MainWindow::getExposure()
{
    if (!m_serialBuffer.writeLine("em=-1"))
    {
        return(false);
    }

    int zones[25];
    for (int i=0; i<25; i++)
    {
        zones[i] = -1;
    }

    //
    // Dump the first line
    //
    m_serialBuffer.readString();

    //
    // Read the 25 fields (five rows of five values)
    //
    bool sawError = false;
    for (int i=0; i<5; i++)
    {
        QString response = m_serialBuffer.readString();
        QStringList list = response.split(QRegExp("\\W+"), QString::SkipEmptyParts);
        if (list.size() != 5)
        {
            //QString title = QFileInfo( QCoreApplication::applicationFilePath() ).fileName();
            //QString msg = "Unexpected response:\n";
            //msg.append(response);
            //QMessageBox::warning(this, title, msg, QMessageBox::Ok);
            return(false);
        }
        for (int j=0; j<list.size(); j++)
        {
            bool b;
            zones[i*5+j] = list[j].toInt(&b);
            if (!b)
            {
                sawError = true;
                break;
            }
        }
        if (sawError)
        {
            break;
        }
    }

    if (!sawError)
    {
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

        m_totalExposure = 0;
        for (int i=0; i<25; i++)
        {
            m_totalExposure += zones[i];
        }
    }
    else
    {
        ui->lineEdit_em_11->clear();
        ui->lineEdit_em_12->clear();
        ui->lineEdit_em_13->clear();
        ui->lineEdit_em_14->clear();
        ui->lineEdit_em_15->clear();

        ui->lineEdit_em_21->clear();
        ui->lineEdit_em_22->clear();
        ui->lineEdit_em_23->clear();
        ui->lineEdit_em_24->clear();
        ui->lineEdit_em_25->clear();

        ui->lineEdit_em_31->clear();
        ui->lineEdit_em_32->clear();
        ui->lineEdit_em_33->clear();
        ui->lineEdit_em_34->clear();
        ui->lineEdit_em_35->clear();

        ui->lineEdit_em_41->clear();
        ui->lineEdit_em_42->clear();
        ui->lineEdit_em_43->clear();
        ui->lineEdit_em_44->clear();
        ui->lineEdit_em_45->clear();

        ui->lineEdit_em_51->clear();
        ui->lineEdit_em_52->clear();
        ui->lineEdit_em_53->clear();
        ui->lineEdit_em_54->clear();
        ui->lineEdit_em_55->clear();

        m_totalExposure = 0;
    }

    qApp->processEvents();
    return(!sawError);
}

/*!
 * @brief
 *
 * @param[in] none
 * @param[out] none
 * @return none
 *
 * @author J. Peterson
 * @date 01/23/2015
*/
bool MainWindow::getCurrentAndVoltage()
{
    bool sawError = false;

    if (!m_serialBuffer.writeLine("ledvi"))
    {
        sawError = true;
    }
    QString response = m_serialBuffer.readString();
    double index;
    QString tmpStr;

    //
    // V1
    //
    tmpStr = response;
    index = tmpStr.indexOf("V1:");
    if (index < 0)
    {
        sawError = true;
    }
    else
    {
        tmpStr.remove(0, index+3);
        index = tmpStr.indexOf(",");
        if (index < 0)
        {
            sawError = true;
        }
        else
        {
            tmpStr.truncate(index);
            m_V1 = tmpStr.toDouble();
        }
    }

    //
    // I1
    //
    tmpStr = response;
    index = tmpStr.indexOf("I1:");
    if (index < 0)
    {
        sawError = true;
    }
    else
    {
        tmpStr.remove(0, index+3);
        index = tmpStr.indexOf(",");
        if (index < 0)
        {
            sawError = true;
        }
        else
        {
            tmpStr.truncate(index);
            m_I1 = tmpStr.toDouble();
        }
    }

    //
    // V2
    //
    tmpStr = response;
    index = tmpStr.indexOf("V2:");
    if (index < 0)
    {
        sawError = true;
    }
    else
    {
        tmpStr.remove(0, index+3);
        index = tmpStr.indexOf(",");
        if (index < 0)
        {
            sawError = true;
        }
        else
        {
            tmpStr.truncate(index);
            m_V2 = tmpStr.toDouble();
        }
    }

    //
    // I2
    //
    tmpStr = response;
    index = tmpStr.indexOf("I2:");
    if (index < 0)
    {
        sawError = true;
    }
    else
    {
        tmpStr.remove(0, index+3);
        m_I2 = tmpStr.toDouble();
    }

    if (sawError)
    {
        m_V1 = m_V2 = m_I1 = m_I2 = 0.0;
        ui->lineEdit_amps1->clear();
        ui->lineEdit_amps2->clear();
        ui->lineEdit_volts1->clear();
        ui->lineEdit_volts2->clear();
    }
    else
    {
        QString numStr;
        if ( (m_V1 > -0.005) && (m_V1 < 0.0) )
        {
            m_V1 = 0.0;
        }
        if ( (m_V2 > -0.005) && (m_V2 < 0.0) )
        {
            m_V2 = 0.0;
        }
        ui->lineEdit_volts1->setText(numStr.setNum(m_V1, 'f', 2));
        ui->lineEdit_volts2->setText(numStr.setNum(m_V2, 'f', 2));
        ui->lineEdit_amps1->setText(numStr.setNum(m_I1, 'f', 3));
        ui->lineEdit_amps2->setText(numStr.setNum(m_I2, 'f', 3));
    }
    qApp->processEvents();

    return(true);
}

/*!
 * @brief
 *
 * @param[in] none
 * @param[out] none
 * @return none
 *
 * @author J. Peterson
 * @date 01/23/2015
*/
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


/*!
 * @brief
 *
 * @param[in] none
 * @param[out] none
 * @return none
 *
 * @author J. Peterson
 * @date 01/23/2015
*/
void MainWindow::setDACValues(int dac1, int dac2)
{
    QString command = QString("led_dac=%1,%2").arg(dac1).arg(dac2);
    m_serialBuffer.writeLine(command.toLocal8Bit().data());
    m_serialBuffer.readString();

    snooze(100);
    updateDACValues();
    getCurrentAndVoltage();
    getExposure();

    m_dac1 = dac1;
    m_dac2 = dac2;
}


/*!
 * @brief
 *
 * @param[in] none
 * @param[out] none
 * @return none
 *
 * @author J. Peterson
 * @date 01/23/2015
*/
bool MainWindow::findCalibration()
{
    ui->lineEdit_LED1_low_final->setText("");
    ui->lineEdit_LED2_low_final->setText("");
    ui->lineEdit_LED1_high_final->setText("");
    ui->lineEdit_LED2_high_final->setText("");

    int X1, X2, M;
    QString numStr;

    X1 = 0;
    X2 = 16384;
    M = (X1+X2)/2;
    setDACValues(M, 0);
    snooze(100);
    while ( X1 < (X2-1) )
    {
        M = (X1+X2)/2;
        setDACValues(M, 0);
        if (m_totalExposure == 0)
        {
            X1 = M;
        }
        else
        {
            X2 = M;
        }
    }
    m_calibrationLow_1 = X1;
    ui->lineEdit_LED1_low_final->setText(numStr.setNum(m_calibrationLow_1));

    X1 = 0;
    X2 = 16384;
    M = (X1+X2)/2;
    setDACValues(0, M);
    snooze(100);
    while ( X1 < (X2-1) )
    {
        M = (X1+X2)/2;
        setDACValues(0, M);
        if (m_totalExposure == 0)
        {
            X1 = M;
        }
        else
        {
            X2 = M;
        }
    }
    m_calibrationLow_2 = X1;
    ui->lineEdit_LED2_low_final->setText(numStr.setNum(m_calibrationLow_2));

    X1 = 48152;
    X2 = 65535;
    M = (X1+X2)/2;
    setDACValues(X1, 0);
    snooze(100);
    while ( X1 < (X2-1) )
    {
        M = (X1+X2)/2;
        setDACValues(M, 0);
        snooze(200);
        if (m_I1 <= 5.25)
        {
            X1 = M;
        }
        else
        {
            X2 = M;
        }
    }
    m_calibrationHigh_1 = X1;
    ui->lineEdit_LED1_high_final->setText(numStr.setNum(m_calibrationHigh_1));

    X1 = 48152;
    X2 = 65535;
    M = (X1+X2)/2;
    setDACValues(0, X1);
    snooze(100);
    while ( X1 < (X2-1) )
    {
        M = (X1+X2)/2;
        setDACValues(0, M);
        snooze(200);
        if (m_I2 <= 5.25)
        {
            X1 = M;
        }
        else
        {
            X2 = M;
        }
    }
    m_calibrationHigh_2 = X1;
    ui->lineEdit_LED2_high_final->setText(numStr.setNum(m_calibrationHigh_2));


    setDACValues(m_calibrationLow_1, m_calibrationLow_2);

    qApp->processEvents();
    return(true);
}

/*!
 * @brief
 *
 * @param[in] none
 * @param[out] none
 * @return none
 *
 * @author J. Peterson
 * @date 01/23/2015
*/
void MainWindow::saveCalibration()
{
    QString response;
    QString command;

    command = QString("led_cal=0,%1,%2").arg(m_calibrationLow_1).arg(m_calibrationHigh_1);
    m_serialBuffer.writeLine(command.toLocal8Bit().data());
    response = m_serialBuffer.readString();

    command = QString("led_cal=1,%1,%2").arg(m_calibrationLow_2).arg(m_calibrationHigh_2);
    m_serialBuffer.writeLine(command.toLocal8Bit().data());
    response = m_serialBuffer.readString();
}



bool MainWindow::getFirmwareVersion()
{
    int i;

    //
    // Issue the command and check the return
    //
    if (!m_serialBuffer.writeLine("version"))
    {
        return(false);
    }

    QString ver_FPGA = m_serialBuffer.readString();
    i = ver_FPGA.indexOf("FPGA: ");
    if (i >= 0)
    {
        ver_FPGA.remove(0, i+6);
        i = ver_FPGA.indexOf("\r\n");
        if (i >= 0)
        {
            ver_FPGA.truncate(i);
        }
        ui->lineEdit_ver_FPGA->setText(ver_FPGA);
    }

    QString ver_ARM  = m_serialBuffer.readString();
    i = ver_ARM.indexOf("ARM: ");
    if (i >= 0)
    {
        ver_ARM.remove(0, i+5);
        i = ver_ARM.indexOf(" ");
        if (i >= 0)
        {
            ver_ARM.truncate(i);
        }
        ui->lineEdit_ver_ARM->setText(ver_ARM);
    }

    QString ver_DSP  = m_serialBuffer.readString();
    i = ver_DSP.indexOf("DSP: ");
    if (i >= 0)
    {
        ver_DSP.remove(0, i+5);
        i = ver_DSP.indexOf(" ");
        if (i >= 0)
        {
            ver_DSP.truncate(i);
        }
        ui->lineEdit_ver_DSP->setText(ver_DSP);
    }

    qApp->processEvents();
    return(true);
}

bool MainWindow::getDacValues()
{
    bool sawError = false;
    double X1, X2;

    if (m_serialBuffer.writeLine("led_dac"))
    {
        QString response = m_serialBuffer.readString();
        int index;
        QString tmpStr;

        bool conversionWasOk = false;
        tmpStr = response;
        index = tmpStr.indexOf("led1=");
        if (index >= 0)
        {
            tmpStr.remove(0, index+5);
            index = tmpStr.indexOf(",");
            if (index >= 0)
            {
                tmpStr.truncate(index);
                X1 = tmpStr.toDouble(&conversionWasOk);
            }
        }
        if (!conversionWasOk)
        {
            sawError = true;
        }

        conversionWasOk = false;
        tmpStr = response;
        index = tmpStr.indexOf("led2=");
        if (index >= 0)
        {
            tmpStr.remove(0, index+5);
            X2 = tmpStr.toDouble(&conversionWasOk);
        }
        if (!conversionWasOk)
        {
            sawError = true;
        }
    }

    if (sawError)
    {
        ui->lineEdit_dac1->setText("");
        ui->lineEdit_dac2->setText("");
    }
    else
    {
        QString numStr;
        ui->lineEdit_dac1->setText(numStr.setNum(X1));
        ui->lineEdit_dac2->setText(numStr.setNum(X2));
    }

    qApp->processEvents();
    return(!sawError);

}

void MainWindow::clearInfoFields()
{
    ui->lineEdit_ver_ARM->clear();
    ui->lineEdit_ver_DSP->clear();
    ui->lineEdit_ver_FPGA->clear();
    ui->lineEdit_LED1_high->clear();
    ui->lineEdit_LED1_low->clear();
    ui->lineEdit_LED2_high->clear();
    ui->lineEdit_LED2_low->clear();
    ui->lineEdit_amps1->clear();
    ui->lineEdit_amps2->clear();
    ui->lineEdit_volts1->clear();
    ui->lineEdit_volts2->clear();

    clearExposureAndDacFields();
}

void MainWindow::clearExposureAndDacFields()
{
    ui->lineEdit_em_11->clear();
    ui->lineEdit_em_12->clear();
    ui->lineEdit_em_13->clear();
    ui->lineEdit_em_14->clear();
    ui->lineEdit_em_15->clear();

    ui->lineEdit_em_21->clear();
    ui->lineEdit_em_22->clear();
    ui->lineEdit_em_23->clear();
    ui->lineEdit_em_24->clear();
    ui->lineEdit_em_25->clear();

    ui->lineEdit_em_31->clear();
    ui->lineEdit_em_32->clear();
    ui->lineEdit_em_33->clear();
    ui->lineEdit_em_34->clear();
    ui->lineEdit_em_35->clear();

    ui->lineEdit_em_41->clear();
    ui->lineEdit_em_42->clear();
    ui->lineEdit_em_43->clear();
    ui->lineEdit_em_44->clear();
    ui->lineEdit_em_45->clear();

    ui->lineEdit_em_51->clear();
    ui->lineEdit_em_52->clear();
    ui->lineEdit_em_53->clear();
    ui->lineEdit_em_54->clear();
    ui->lineEdit_em_55->clear();

    ui->lineEdit_dac1->clear();
    ui->lineEdit_dac2->clear();
}







