#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include "Settings.h"



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void loadSettings();
    void saveSettings();

public slots:
    void refreshSerialConnections();
    void startCalibration();
    
private:
    Ui::MainWindow *ui;

    // settings
    CSettings   m_settings;
};

#endif // MAINWINDOW_H
