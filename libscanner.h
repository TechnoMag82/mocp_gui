#ifndef LIBSCANNER_H
#define LIBSCANNER_H

#include <QThread>
#include <QDir>
#include <QStringList>

#include "tagsreader.h"
#include "dbmodule.h"

class LibScanner : public QThread
{
    Q_OBJECT
    signals:
        void scanOnProgress(QString value, int progress);
        void progressIsInit(uint max);
        void scanCompleted();

    public:
        LibScanner(QString pathToLib, bool rescan = false);
        ~LibScanner();

        void run();

    private:
        void scanRecursive(const QDir &dir);
        void writeToDB(QString filePath);

        bool rescan;
        QString pathToLib;
        TagsReader *tagsReader = nullptr;
        DbModule *dbModule = nullptr;
        QStringList findResult;

        qint64 totalSize = 0;
};

#endif // LIBSCANNER_H
