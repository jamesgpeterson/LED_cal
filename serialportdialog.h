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
