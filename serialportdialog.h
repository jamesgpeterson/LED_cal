/*!
 * @file SerialPortDialog.h
 * @brief declares a dialog box for selecting COM ports
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

#ifndef SERIALPORTDIALOG_H
#define SERIALPORTDIALOG_H

#include <QDialog>

namespace Ui {
class CSerialPortDialog;
}

class CSerialPortDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit CSerialPortDialog(QWidget *parent = 0);
    ~CSerialPortDialog();
    QString getSelection();

private:
    void refreshSerialPortList();
    
private:
    Ui::CSerialPortDialog *ui;
};

#endif // SERIALPORTDIALOG_H
