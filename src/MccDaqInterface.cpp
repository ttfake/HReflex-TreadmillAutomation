#include "MccDaqInterface.h"

MccDaqInterface* MccDaqInterface::_mccDaqInterfaceInstance = 0;

MccDaqInterface::MccDaqInterface()
{
    mccDaqDisc = new MccDaqDiscovery;
    connect(mccDaqDisc, SIGNAL(setText(QString)), this, SLOT(setDaqButtonText(QString)));

}

MccDaqInterface::~MccDaqInterface()
{

}

MccDaqInterface* MccDaqInterface::getInstance()
{
    if(_mccDaqInterfaceInstance == NULL)
    {
        _mccDaqInterfaceInstance = new MccDaqInterface();
    }

    return _mccDaqInterfaceInstance;
}

void MccDaqInterface::beginDataCollection()
{
    std::vector<short> ChanArray;
    std::vector<short> ChanTypeArray;
    std::vector<short> GainArray;

    qDebug("beginning data collection");

    forcePlateDataString = "forcePlateData.csv";

    forcePlateDataFile = new QFile(forcePlateDataString);
   
    ULStat = cbDeclareRevision(&RevLevel);

    qDebug("%d", ULStat);

    ADData = static_cast<WORD*>(cbWinBufAlloc(numPoints * numChans));
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

    std::cout << "Data are being collected in the BACKGROUND, CONTINUOUSLY\n" << std::endl;


	/* load the arrays with values */
    ChanArray.push_back(3);
	ChanTypeArray.push_back(ANALOG);
    GainArray.push_back(BIP10VOLTS);

    ChanArray.push_back(FIRSTPORTA);
	ChanTypeArray.push_back(DIGITAL16);
    GainArray.push_back(NOTUSED);

	ChanArray.push_back(0);
	ChanTypeArray.push_back(CTR32LOW);
    GainArray.push_back(NOTUSED);

	ChanArray.push_back(0);
	ChanTypeArray.push_back(CTR32HIGH);
    GainArray.push_back(NOTUSED);

	/* configure FIRSTPORTA and FIRSTPORTB for digital input */
	
	PortNum = FIRSTPORTA;
    Direction = DIGITALIN;
    ULStat = cbDConfigPort (BoardNum, PortNum, Direction);

	PortNum = FIRSTPORTB;
	ULStat = cbDConfigPort (BoardNum, PortNum, Direction);

	// configure counter 0
	CounterNum = ChanArray[2];
    ULStat = cbCConfigScan(BoardNum, CounterNum, ROLLOVER, CTR_DEBOUNCE_NONE, 0, CTR_RISING_EDGE, \
            0, CounterNum);

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

    ChanCount = numChans;
	PreTrigCount =0;
	TotalCount = numPoints * numChans;
	Rate = 1000;								             /* sampling rate (samples per second) */
	Options = CONVERTDATA + BACKGROUND + CONTINUOUS;         /* data collection options */

	ULStat = cbDaqInScan(BoardNum, static_cast<short*>(&ChanArray[0]), \
            static_cast<short*>(&ChanTypeArray[0]), static_cast<short*>(&GainArray[0]), \
            ChanCount, &Rate, &PreTrigCount, &TotalCount, ADData, Options);

	if(ULStat == NOERRORS)
		Status = RUNNING;
    
    while (!_kbhit() && Status==RUNNING)
    {
        /* Check the status of the current background operation
           Parameters:
           BoardNum  :the number used by CB.CFG to describe this board
           Status    :current status of the operation (IDLE or RUNNING)
           CurCount  :current number of samples collected
           CurIndex  :index to the last data value transferred 
           FunctionType: A/D operation (DAQIFUNCTION)*/

        ULStat = cbGetStatus (BoardNum, &Status, &CurCount, &CurIndex,DAQIFUNCTION);

        /* check the current status of the background operation */
        if (Status == RUNNING)
        {
            DataIndex = CurIndex -  CurIndex % numChans - numChans;
            if(DataIndex>0)
            {
                if (!forcePlateDataFile->open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append))
                    return;

                QTextStream forcePlateDataStream(forcePlateDataFile);


                printf ("Channel 0   Data point: %3ld   ", DataIndex);
                printf ("  Value: %d  \n",ADData[DataIndex]);
                updateDaqDataBox(ADData[DataIndex]);
                QString dataPoint1String(QString::number(ADData[DataIndex],'e',12));
                forcePlateDataStream << QTime::currentTime().toString() << ",";
                forcePlateDataStream << dataPoint1String << ",";
                DataIndex++;
                printf ("FIRSTPORTA  Data point: %3ld   ", DataIndex);
                printf ("  Value: %d  \n",ADData[DataIndex]);
                updateDaqDataBox(ADData[DataIndex]);
                QString dataPoint2String(QString::number(ADData[DataIndex],'e',12));
                forcePlateDataStream << dataPoint2String << ",";
                DataIndex++;
                printf ("Counter 0   Data point: %3ld   ", DataIndex);
                printf ("  Value: %u  ",ADData[DataIndex] + (ADData[DataIndex+1]<<16));   // 32-bit counter
                updateDaqDataBox(ADData[DataIndex] + (ADData[DataIndex+1]<<16));
                QString dataPoint3String(QString::number((ADData[DataIndex] + \
                                ADData[DataIndex+1]<<16),'e',12));
                forcePlateDataStream << dataPoint3String << '\n';
                forcePlateDataFile->close(); 
            }
        }
    }

    /* The BACKGROUND operation must be explicitly stopped
       Parameters:
       BoardNum    :the number used by CB.CFG to describe this board 
       FunctionType: A/D operation (DAQIFUNCTION)*/  
    ULStat = cbStopBackground (BoardNum,DAQIFUNCTION);

    cbWinBufFree(ADData);
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

void MccDaqInterface::setNumberOfChannels(int mchs)
{
    numChans = mchs;
}

#include "../include/moc_MccDaqInterface.cpp"