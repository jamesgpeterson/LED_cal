#include <QSerialPortInfo>
#include "mainwindow.h"
#include "ui_mainwindow.h"



#define NOT_SELECTED "not selected"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //
    // Initialize the comm port list
    //
    refreshSerialConnections();

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


void MainWindow::refreshSerialConnections()
{
    ui->comboBox_commPort->clear();
    QList<QSerialPortInfo> commPortList = QSerialPortInfo::availablePorts();
    ui->comboBox_commPort->setEnabled(false);
    ui->comboBox_commPort->addItem(NOT_SELECTED);
    for (int i=commPortList.size()-1; i>=0; i--)
    {
        QSerialPortInfo portInfo = commPortList[i];
        ui->comboBox_commPort->addItem(portInfo.portName());
    }
    ui->comboBox_commPort->setEnabled(true);
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


