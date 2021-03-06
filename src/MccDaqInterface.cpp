#include "MccDaqInterface.h"


MccDaqInterface::MccDaqInterface()
{
    leftForceCoefficientXKey = 0;
    leftForceCoefficientX = 500.00;
    leftForceCoefficientYKey = 1;
    leftForceCoefficientY = 500.00;
    leftForceCoefficientZKey = 2;
    leftForceCoefficientZ = 1000.00;
    leftMomentCoefficientXKey = 3;
    leftMomentCoefficientX = 800.00;
    leftMomentCoefficientYKey = 4;
    leftMomentCoefficientY = 400.00;
    leftMomentCoefficientZKey = 5;
    leftMomentCoefficientZ = 400.00;

    rightForceCoefficientXKey = 6;
    rightForceCoefficientX = 500.00;
    rightForceCoefficientYKey = 7;
    rightForceCoefficientY = 500.00;
    rightForceCoefficientZKey = 8;
    rightForceCoefficientZ = 1000.00;
    rightMomentCoefficientXKey = 9;
    rightMomentCoefficientX = 800.00;
    rightMomentCoefficientYKey = 10;
    rightMomentCoefficientY = 400.00;
    rightMomentCoefficientZKey = 11;
    rightMomentCoefficientZ = 400.00;
 
    
    scale = 10;

    mccDaqDisc = new MccDaqDiscovery;
    qRegisterMetaType<uint16_t>("uint16_t");
    connect(mccDaqDisc, SIGNAL(setText(QString)), this, SLOT(setDaqButtonText(QString)));
    channelCoefficientMap.insert({ {leftForceCoefficientXKey, leftForceCoefficientX}, \
            {leftForceCoefficientYKey, leftForceCoefficientY}, \
            {leftForceCoefficientZKey, leftForceCoefficientZ}, \
            {leftMomentCoefficientXKey, leftMomentCoefficientX}, \
            {leftMomentCoefficientYKey, leftMomentCoefficientY}, \
            {leftMomentCoefficientZKey, leftMomentCoefficientZ}, \
            {rightForceCoefficientXKey, rightForceCoefficientX}, \
            {rightForceCoefficientYKey, rightForceCoefficientY}, \
            {rightForceCoefficientZKey, rightForceCoefficientZ}, \
            {rightMomentCoefficientXKey, rightMomentCoefficientX}, \
            {rightMomentCoefficientYKey, rightMomentCoefficientY}, \
            {rightMomentCoefficientZKey, rightMomentCoefficientZ} \
    });

}

MccDaqInterface::~MccDaqInterface()
{

}

void MccDaqInterface::beginDataCollectionOnChannel(int ch)
{
    std::vector<short> ChanVector;
    std::vector<short> ChanTypeVector;
    std::vector<short> GainVector;
    BoardNum = 0;
    ULStat = 0;
    rowCount = 0;
    chs = 0;
   
    qDebug() << "beginning data collection";


    forcePlateDataFile = new QFile(daqLogFileName);
   
    ULStat = cbDeclareRevision(&RevLevel);

    qDebug("cbDeclareRevision ULStat: %d", ULStat);

    ADData = static_cast<WORD*>(cbWinBufAlloc(NUMPOINTS * NUMCHANS));
    if (!ADData)    /* Make sure it is a valid pointer */
    {
        std::cout << "\nout of memory\n" << std::endl;
    }
    
    
    /* Initiate error handling
       Parameters:
       PRINTALL :all warnings and errors encountered will be printed
       DONTSTOP :program will continue even if error occurs.
       Note that STOPALL and STOPFATAL are only effective in 
       Windows applications, not Console applications. 
    */
    cbErrHandling (PRINTALL, DONTSTOP);

    std::cout << "Continuous data collection has begun in the BACKGROUND.\n" << std::endl;

    /* load the arrays with values */
    if (!forcePlateDataFile->open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append))
            return;
    QTextStream forcePlateDataStream(forcePlateDataFile);
    forcePlateDataStream << "Timestamp,";

}

void MccDaqInterface::beginDataCollection()
{
    dateIndex = 0;
    captureTime = 1;                    // Capture time in seconds. This will become configurable in 
                                        // future iterations.
    int globalIdxCurIdxDiff = 0;
    
    std::vector<short> ChanVector;
    std::vector<short> ChanTypeVector;
    std::vector<short> GainVector;
    
    BoardNum = 0;
    ULStat = 0;
    rowCount = 0;
    chs = 0;
   
    qDebug() << "beginning data collection";


    forcePlateDataFile = new QFile(daqLogFileName);
   
    ULStat = cbDeclareRevision(&RevLevel);

    qDebug("cbDeclareRevision ULStat: %d", ULStat);

    ADData = static_cast<WORD*>(cbWinBufAlloc(NUMPOINTS * NUMCHANS));
    if (!ADData)    /* Make sure it is a valid pointer */
    {
        std::cout << "\nout of memory\n" << std::endl;
    }
    
    
    /* Initiate error handling
       Parameters:
       PRINTALL :all warnings and errors encountered will be printed
       DONTSTOP :program will continue even if error occurs.
       Note that STOPALL and STOPFATAL are only effective in 
       Windows applications, not Console applications. 
    */
    cbErrHandling (PRINTALL, DONTSTOP);

    std::cout << "Continuous data collection has begun in the BACKGROUND.\n" << std::endl;

    /* load the arrays with values */
    if (!forcePlateDataFile->open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append))
            return;
    QTextStream forcePlateDataStream(forcePlateDataFile);
    forcePlateDataStream << "Timestamp,";

    for(channel = 0; channel < NUMCHANS; channel++)
    {
        emit getActiveState(channel);

        QThread::msleep(30);
        
        if(activeState)
        {
            channelVector.push_back(channel);
            ChanVector.push_back(channel);
            ChanTypeVector.push_back(ANALOG);
            GainVector.push_back(BIP10VOLTS);
            
            QString channelHeaderString("Channel ");
            channelHeaderString += QString::number(channel + 1);
            channelHeaderString += ",";

            forcePlateDataStream << channelHeaderString;

/*            double voltage = ((20.0 / pow(2.0,16)) * 38993.92) - 10.0;
            double force = channelCoefficientMap[channel] * (voltage / scale);

            QString dataPoint1String(QString::number(force,'f',4));

            qDebug("Force: %f", force);
*/
            chs++;

            activeState = false;
        }
    }

    forcePlateDataStream << "Perturbation\n";
    forcePlateDataFile->close();

    //setNumberOfChannels(chs);
    //chs = 0;
    
    short ChanArray[ChanVector.size()];
    short ChanTypeArray[ChanTypeVector.size()];
    short GainArray[GainVector.size()];


    std::copy(ChanVector.begin(), ChanVector.end(), ChanArray);
    std::copy(ChanTypeVector.begin(), ChanTypeVector.end(), ChanTypeArray);
    std::copy(GainVector.begin(), GainVector.end(), GainArray);
    

	/* configure FIRSTPORTA and FIRSTPORTB for digital input */
	
	PortNum = FIRSTPORTA;
    Direction = DIGITALIN;
    qDebug("BoardNumber %d", BoardNum);
    ULStat = cbDConfigPort (BoardNum, PortNum, Direction);
    qDebug("cbDConfigPort UL Stat: %d", ULStat);

	PortNum = FIRSTPORTB;
	ULStat = cbDConfigPort (BoardNum, PortNum, Direction);

    qDebug("UL Status %i", ULStat);


    /* Collect the values with cbDaqInScan() in BACKGROUND mode, CONTINUOUSLY
        Parameters:
            BoardNum    :the number used by CB.CFG to describe this board
            ChanArray[] :array of channel values
			ChanTypeArray[] : array of channel types
            GainArray[] :array of gain values
            ChanCount    :the number of channels
            Rate        :sample rate in samples per second
			PretrigCount:number of pre-trigger A/D samples to collect
            TotalCount  :the total number of A/D samples to collect
            ADData[]    :the array for the collected data values
            Options     :data collection options  */

    ChanCount = NUMCHANS;
	PreTrigCount =0;
	TotalCount = NUMPOINTS * NUMCHANS;
	Rate = 1000;								             /* sampling rate (samples per second) */
	Options = CONVERTDATA + BACKGROUND + CONTINUOUS;         /* data collection options */

    channel = 0;
    qDebug("Starting scan");
	ULStat = cbDaqInScan(BoardNum, ChanArray, \
            ChanTypeArray, GainArray, \
            ChanCount, &Rate, &PreTrigCount, &TotalCount, ADData, Options);
    qDebug("Error code is %d", ULStat);

	if(ULStat == NOERRORS)
		Status = RUNNING;
    globalDaqDataIndex = 0;
    dataPoints = 0;

    
    
    while (!_kbhit() && Status==RUNNING)
    {
        /* Check the status of the current background operation
           Parameters:
           BoardNum  :the number used by CB.CFG to describe this board
           Status    :current status of the operation (IDLE or RUNNING)
           CurCount  :current number of samples collected
           CurIndex  :index to the last data value transferred 
           FunctionType: A/D operation (DAQIFUNCTION)*/

        ULStat = cbGetStatus (BoardNum, &Status, &CurCount, &CurIndex, DAQIFUNCTION);

        /* check the current status of the background operation */
        if (Status == RUNNING)
        {

            unixTime = QDateTime::currentSecsSinceEpoch();
            DataIndex = CurIndex -  CurIndex % NUMCHANS - NUMCHANS;
            //if(DataIndex>0)
            
            qDebug() << "CurIndex: " << CurIndex;
            
            if((globalDaqDataIndex != CurIndex) && (CurIndex > 0))
            {
//                emit updateRowCount(rowCount);
                if (!forcePlateDataFile->open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append))
                    return;

                QTextStream forcePlateDataStream(forcePlateDataFile);

//                if(recordBool)
//                {
//                    forcePlateDataStream << QTime::currentTime().toString() << ",";
//                }

                globalIdxCurIdxDiff = CurIndex - globalDaqDataIndex;



                for(channel = 0; channel < NUMCHANS; channel++)
                {
                    qDebug() << "Channel: " << channel;
                    //emit updateCol(channelVector[channel]);
                    emit setCurrentChannel(channel);
                    double voltage = ((20.0 / pow(2.0,16)) * static_cast<double>(ADData[DataIndex])) - 10.0;
                    qDebug() << "Voltage: " << voltage;
                    double force = channelCoefficientMap[channel] * (voltage / scale);
                    emit updateDaqDataBoxSignal(voltage);
                    daqVoltageMap[channel].append(voltage);
                    daqForceMap[channel].append(force);
                    dataPoints++;

                    dateIndex++;
                    //qDebug("Force: %f", force);
                    dataPoint1String = QString::number(force,'f',4);
                    //qDebug("dataPoint1String: %f", dataPoint1String);
                    if(recordBool)
                    {
                        forcePlateDataStream << dataPoint1String << ",";
                        if(pertTrigger)
                        {
                            forcePlateDataStream << QString::number(1);
                            setPerturbationTrigger(false);
                        }
                        else
                        {
                            forcePlateDataStream << QString::number(0);
                        }
                    }

                    DataIndex++;
                }

                qDebug() << "dataPoints: " << dataPoints;
                qDebug() << "Rate * NUMCHANS: " << (Rate * NUMCHANS);
                    
                if( dataPoints >= floor(Rate * NUMCHANS) )//unixTime - prevUnixTime) >= 1 )
                {
                    qDebug() << "Number of time points is: " << dataPoints;
                    dataPoints = 0;
                    tmpVoltageMap = daqVoltageMap;
                    daqVoltageMap.clear();
                    emit updateEmg();
                }

                prevUnixTime = unixTime;

                if(recordBool)
                {
                    forcePlateDataStream << "\n";
                }

                forcePlateDataFile->close(); 

                rowCount++;

                globalDaqDataIndex = CurIndex;
            }
        }
    }

    /* The BACKGROUND operation must be explicitly stopped
       Parameters:
       BoardNum    :the number used by CB.CFG to describe this board 
       FunctionType: A/D operation (DAQIFUNCTION)*/  
    qDebug("Exited");
    
    ULStat = cbStopBackground (BoardNum,DAQIFUNCTION);

    cbWinBufFree(ADData);
    
}

void MccDaqInterface::testDataCollection()
{

}

void MccDaqInterface::setDaqButtonText(QString daqTitleText)
{
    emit setDaqTitleText(daqTitleText);
}

void MccDaqInterface::setDaqDataBox(QPlainTextEdit* mdaqBox)
{
    daqBox = new QPlainTextEdit;
    daqBox = mdaqBox;
}

void MccDaqInterface::startChannelScan(QMenu* mmenu)
{
    mccDaqDisc->discoverDaqDevices(mmenu);
}

void MccDaqInterface::dataCollectionSetup()
{

}

void MccDaqInterface::setNumberOfChannels(long mchs)
{
    NUMCHANS = mchs;
    qDebug("Number of Channels: %d", NUMCHANS);
}

void MccDaqInterface::setCurrentChannelActiveState(bool mactiveState)
{
    activeState = mactiveState;
}

void MccDaqInterface::setRunningState(int runningState)
{
    Status = IDLE;
}

void MccDaqInterface::setPerturbationTrigger(bool mpertTrigger)
{
    pertTrigger = mpertTrigger;
}

void MccDaqInterface::setRecordBool(bool mrecordBool)
{
    recordBool = mrecordBool;
}

void MccDaqInterface::setDaqLogFileName(QString mfileName)
{
    daqLogFileName = mfileName;
}

QMap<int,QVector<double>> MccDaqInterface::getVoltageMap()
{
    return tmpVoltageMap;
}


void MccDaqInterface::clearTmpVoltageMap()
{
    tmpDaqVoltageMap.clear();
}


#include "../include/moc_MccDaqInterface.cpp"
