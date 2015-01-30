#include "mainwindow.h"
#include "ui_mainwindow.h"



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
