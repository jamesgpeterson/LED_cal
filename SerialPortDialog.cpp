#include <QSerialPortInfo>
#include "serialportdialog.h"
#include "ui_SerialPortDialog.h"

CSerialPortDialog::CSerialPortDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CSerialPortDialog)
{
    ui->setupUi(this);
    refreshSerialPortList();
}

CSerialPortDialog::~CSerialPortDialog()
{
    delete ui;
}

void CSerialPortDialog::refreshSerialPortList()
{
    ui->comboBox_serialPorts->clear();
    QList<QSerialPortInfo> commPortList = QSerialPortInfo::availablePorts();
    ui->comboBox_serialPorts->setEnabled(false);
    ui->comboBox_serialPorts->addItem("<select>");
    for (int i=commPortList.size()-1; i>=0; i--)
    {
        QSerialPortInfo portInfo = commPortList[i];
        ui->comboBox_serialPorts->addItem(portInfo.portName());
    }
    ui->comboBox_serialPorts->setEnabled(true);
}

QString CSerialPortDialog::getSelection()
{
    QString str = ui->comboBox_serialPorts->currentText();
    return(ui->comboBox_serialPorts->currentText());
}

