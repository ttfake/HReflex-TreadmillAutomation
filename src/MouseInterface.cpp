#include "include/MouseInterface.h"

MouseInterface::MouseInterface()
{
    QTextStream out(stdout);
    const auto serialPortInfos = QSerialPortInfo::availablePorts();

    out << QObject::tr("Total number of ports available: ") << serialPortInfos.count() << endl;
    
    const QString blankString = QObject::tr("N/A");
    QString description;
    QString manufacturer;
    QString serialNumber;

    movementBool = false;

/*    for (const QSerialPortInfo &serialPortInfo : serialPortInfos) {
        description = serialPortInfo.description();
        manufacturer = serialPortInfo.manufacturer();
        serialNumber = serialPortInfo.serialNumber();
        out << endl
            << QObject::tr("Port: ") << serialPortInfo.portName() << endl
            << QObject::tr("Location: ") << serialPortInfo.systemLocation() << endl
            << QObject::tr("Description: ") << (!description.isEmpty() ? description : blankString) << endl
            << QObject::tr("Manufacturer: ") << (!manufacturer.isEmpty() ? //
                    manufacturer : blankString) << endl
            << QObject::tr("Serial number: ") << (!serialNumber.isEmpty() ? //
                    serialNumber : blankString) << endl
            << QObject::tr("Vendor Identifier: ") << (serialPortInfo.hasVendorIdentifier() ? //
                    QByteArray::number(serialPortInfo.vendorIdentifier(), 16) : blankString) << endl
            << QObject::tr("Product Identifier: ") << (serialPortInfo.hasProductIdentifier() ? //
                    QByteArray::number(serialPortInfo.productIdentifier(), 16) : blankString) << endl
            << QObject::tr("Busy: ") << (serialPortInfo.isBusy() ? QObject::tr("Yes") : //
                    QObject::tr("No")) << endl;
        
            availableComPorts.push_back(serialPortInfo.portName());
            comPortInfo.push_back(serialPortInfo);

    } 
*/
}
 
MouseInterface::~MouseInterface()
{

}

int MouseInterface::openPort()
{

    QTextStream standardOutput(stdout);

    serialPort = new QSerialPort;
    QString serialPortName = "com7"; //argumentList.at(1);
    qDebug() << serialPortName;
    serialPort->setPortName(serialPortName);

    int numRead = 0, numReadTotal = 0;
    char buffer[50];

    int serialPortBaudRate = QSerialPort::Baud9600;
    serialPort->setBaudRate(serialPortBaudRate);

    if (!serialPort->open(QIODevice::ReadOnly)) {
        standardOutput << QObject::tr("Failed to open port %1, error: %2").
            arg(serialPortName).arg(serialPort->errorString()) << endl;
        return 1;
    }

    connect(serialPort, SIGNAL(readyRead()), this, SLOT(getRawInput()));
    return 0;

}

void MouseInterface::getRawInput()
{

    QFile mouseLog("mouseLog.log");
    QFile movementDetectedLog(logPath + "/" + "movementDetectedLog.log");
    movementDetectedLog.open(QIODevice::Append);
    double startingCoord(0.00);
    bool ok;
    QByteArray data;
    QString dataString;
        
    qDebug() << " Data Detected\n";
   
    while(serialPort->canReadLine())
    {
        data = serialPort->readLine();
        dataString = QString::fromLocal8Bit(data);

        qDebug() << "Reading Data";
        //-----------------Current Unix Timestamp--------------------------------
        qint64 currentMsecsSinceEpoch = QDateTime::currentMSecsSinceEpoch();
        movementDetectedLog.write(QString::number(currentMsecsSinceEpoch).toUtf8());
        //-----------------------------------------------------------------------
        //qDebug() << startingCoord;
        movementDetectedLog.write(",");
        movementDetectedLog.write(data);
        dataString.chop(2);
        startingCoord = fabs(dataString.toDouble());
        if(startingCoord > 0)
        {
            qDebug() << startingCoord;
            emit movement();
        }
    }
    
    movementDetectedLog.write("\n");
    
    movementDetectedLog.close();


    //    QList<QByteArray> list = dataRead.split('\t');

/*    for(const auto item : list)
    {
        if(item.contains("x="))
        {
            QList<QByteArray> xCoord = item.split('=');
            if(qFabs(xCoord[1].toDouble()) > startingCoord)
            {
                //qDebug() << "Data Received: " << xCoord[1].toInt();
                qDebug() << "Movement Detected: " << xCoord[1].toInt();
                if( !movementBool && perturbationActiveBool)
                {
                    QFile movementDetectedLog(logPath + "/" + "movementDetectedLog.log");
                    movementDetectedLog.open(QIODevice::Append);
                   //--------------------------------------------------------------------- 

                     auto uptime = std::chrono::milliseconds(GetTickCount());

                   //---------------------------------------------------------------------
                    std::ostringstream out;
                    out << std::chrono::milliseconds(uptime).count() << "ms";
                    std::string uptimeStdString = out.str();
                    QString uptimeString = QString::fromStdString(uptimeStdString);
                    movementDetectedLog.write(uptimeString.toUtf8() + "\n");

 
                    //-----------------Current Unix Timestamp--------------------------------
                    qint64 currentMsecsSinceEpoch = QDateTime::currentMSecsSinceEpoch();
                    movementDetectedLog.write("Unix System Timestamp:" + QString::number(currentMsecsSinceEpoch).toUtf8() + "\n");
                    //-----------------------------------------------------------------------


                    movementDetectedLog.close();
                    emit movement();
                }
            }
        }
    }
*/

    mouseLog.open(QIODevice::Append);
    
    mouseLog.write(dataRead);
    mouseLog.close();


    dataRead.clear();

} 

void MouseInterface::setMovementBool(bool m_movementDetected)
{
    movementBool = m_movementDetected;
}

void MouseInterface::setPerturbationActiveBoolTrue()
{
    perturbationActiveBool = true;
}

void MouseInterface::setPerturbationActiveBoolFalse()
{
    perturbationActiveBool = false;
}

void MouseInterface::setLogPath(QString m_logPath)
{
    logPath = m_logPath;
}

void MouseInterface::SetupDigitalOutput()
{
    //DAQmxCreateTask("",&taskHandle);
    //DAQmxCreateCOPulseChanTime(taskHandle,"Dev1/ctr0","",DAQmx_Val_Seconds,DAQmx_Val_High,0.0,0.01,0.001);
    //DAQmxCreateDOChan(taskHandle,OutputLine.c_str(),"",DAQmx_Val_ChanPerLine);
    //DAQmxCfgSampClkTiming(taskHandle,"",100.0,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,13);
    //DAQmxStartTask(taskHandle);
    //DAQmxRegisterDoneEvent(taskHandle,0,DoneCallback,NULL);

}

void MouseInterface::StopTask()
{
   // DAQmxStopTask(taskHandle);
   // DAQmxClearTask(taskHandle);
}

void MouseInterface::WriteLine()
{
   // DAQmxStartTask(taskHandle);
   // DAQmxWaitUntilTaskDone(taskHandle,0.05);
   // DAQmxStopTask(taskHandle);
}

#include "../include/moc_MouseInterface.cpp"
