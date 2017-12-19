#include "ParseRunProfile.h"

ParseRunProfile::ParseRunProfile()
{
    treadmillDb = new TreadmillAutomationDbIFace;
    truncateTable("currentRunProfile");
}

ParseRunProfile::~ParseRunProfile()
{
}

void ParseRunProfile::setRunFile(QString mFileName)
{
    runFile = new QFile(mFileName);
    parseRunFile();
}

void ParseRunProfile::parseRunFile()
{
    QString table("runProfile");
    if(runFile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream currentRun(runFile);
        while (!currentRun.atEnd())
        {
            QString line = currentRun.readLine();
            treadmillDb->insertRunProfileRecord(line.split('_'));
        }
        runFile->close();
    }
    else
    {
        qDebug() << "Error opening file";
    }
}

double ParseRunProfile::getAccelLeft()
{
    double accelerationLeft(treadmillDb->getAccelLeft());

    return accelerationLeft;
}

double ParseRunProfile::getAccelRight()
{
    double accelerationRight(treadmillDb->getAccelRight());

    return accelerationRight;
}

double ParseRunProfile::getDecelRight()
{
    double decelerationRight(treadmillDb->getDecelRight());

    return decelerationRight;
}


double ParseRunProfile::getDecelLeft()
{
    double decelerationLeft(treadmillDb->getDecelLeft());

    return decelerationLeft;
}

double ParseRunProfile::getAccelTime()
{
    double accelTime(treadmillDb->getAccelTime());

    return accelTime;
}


void ParseRunProfile::getRuns()
{
    treadmillDb->retrieveRuns();
}

void ParseRunProfile::getRun(int runIndex)
{
    treadmillDb->retrieveRun(runIndex);
    connect(treadmillDb, SIGNAL(startStimulation()), SLOT(startStim()));
}

void ParseRunProfile::startStim()
{
    emit startStimulation();
}

void ParseRunProfile::truncateTable(QString table)
{
    treadmillDb->truncateTable(table);
}

#include "../include/moc_ParseRunProfile.cpp"
