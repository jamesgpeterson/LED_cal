/*!
 * @file SerialPortDialog.cpp
 * @brief Implements a dialog box for selecting COM ports
 *
 * When created this dialog populates a ComboBox with the available COM ports.
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
#include "serialportdialog.h"
#include "ui_SerialPortDialog.h"

/*!
 * @brief constructor
 *
 * @param[in] parent QWidget for this dialog
 * @param[out] none
 * @return none
 *
 * @author J. Peterson
 * @date 01/12/2015
*/
CSerialPortDialog::CSerialPortDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CSerialPortDialog)
{
    ui->setupUi(this);
    refreshSerialPortList();
}

/*!
 * @brief destructor
 *
 * @param[in] none
 * @param[out] none
 * @return none
 *
 * @author J. Peterson
 * @date 01/12/2015
*/
CSerialPortDialog::~CSerialPortDialog()
{
    delete ui;
}

/*!
 * @brief initializes the Combo-Box with the available comm ports
 *
 * @param[in] none
 * @param[out] none
 * @return none
 *
 * @author J. Peterson
 * @date 01/12/2015
*/
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

/*!
 * @brief returns the serial port last selected by the user
 *
 * @param[in] none
 * @param[out] none
 * @return Name of the selected COMM port in the form of a QString
 *
 * @author J. Peterson
 * @date 01/12/2015
*/
QString CSerialPortDialog::getSelection()
{
    QString name = ui->comboBox_serialPorts->currentText();
    if (name == "<select>")
    {
        name = "";
    }
    return(name);
}

