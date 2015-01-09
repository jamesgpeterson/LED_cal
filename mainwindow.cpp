#include <QSerialPortInfo>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "SerialPortDialog.h"



#define NOT_SELECTED "not selected"


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


MainWindow::~MainWindow()
{
    //
    // Save the settings
    //
    m_settings.m_logFile = ui->lineEdit_logFile->text();
    delete ui;
}


void MainWindow::selectSerialPort()
{
    CSerialPortDialog *dlg = new CSerialPortDialog(this);
    //dlg->setModal(true);
    dlg->exec();
    ui->lineEdit_serialPort->setText(dlg->getSelection());


}

void MainWindow::startCalibration()
{
    //
    // Check serial connection
    //

    //
    // Check that the controller is running
    //

    //
    // Check that the scope is connected
    //

    //
    // Get the current calibration values
    //

    //
    // Find the
}


