#ifndef PARSERUNPROFILE_H
#define PARSERUNPROFILE_H

#include <QWidget>
#include <QFile>
#include <QTextStream>

#include "TreadmillAutomationDbIFace.h"

class ParseRunProfile : public QWidget
{
    Q_OBJECT

    public:
        ParseRunProfile();
        ~ParseRunProfile();
        void parseRunFile();
        double getAccelLeft();
        double getAccelRight();
        double getAccelTime();
        double getDecelLeft();
        double getDecelRight();
        double getDecelTime();
        void getRuns();
        void getRun(int runIndex);
        void truncateTable(QString table);
        void setRunFile(QString mRunFile);
        QVector<QString> getRunsVector();
        QVector<QString> getRunProfileHead();
        void updateRunTable(QString tableName, QString type, QString level, 
        QString stimOrder, QString participantId, QString sessionNo, QString runNo, 
        QString trialNo);
        void clearRunResultsVector();
        double getAccelLeftDb(QString typeNo, QString levelNo);
        double getAccelRightDb(QString typeNo, QString levelNo);
        double getDecelLeftDb(QString typeNo, QString levelNo);
        double getDecelRightDb(QString typeNo, QString levelNo);
        double getAccelTimeDb(QString typeNo);
        int getDirectionFromDb(QString type, QString level);

    signals:
        startStimulation();

    public slots:
        void startStim();
    
    private:
        QFile* runFile;
        TreadmillAutomationDbIFace* treadmillDb;
};
#endif
