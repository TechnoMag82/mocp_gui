#include "libscanner.h"

LibScanner::LibScanner(QString pathToLib, bool rescan)
{
    this->rescan = rescan;
    this->pathToLib = pathToLib;
    tagsReader = new TagsReader();
}

LibScanner::~LibScanner()
{
    delete tagsReader;
}

void LibScanner::run()
{
    int progress = 0;
    if (dbModule == nullptr)
        dbModule = new DbModule();
    QDir dir(pathToLib);
    scanRecursive(dir);
    emit progressIsInit(findResult.count());
    if (rescan)
        dbModule->clearTable();
    foreach (QString filePath, findResult) {
        emit scanOnProgress(filePath, progress++);
        writeToDB(dir.absoluteFilePath(filePath));
    }
    if (dbModule != nullptr)
        delete dbModule;
    emit scanCompleted();
}

void LibScanner::scanRecursive(const QDir &dir)
{
    QStringList listFiles = dir.entryList(QStringList("*.mp3"), QDir::Files);

    foreach (QString file , listFiles) {
        findResult.append(dir.absoluteFilePath(file));
    }

    QStringList dirList = dir.entryList(QDir::Dirs);

    foreach (QString subDir, dirList) {
        if (subDir == "." || subDir == "..")
            continue;
        scanRecursive(QDir(dir.absoluteFilePath(subDir)));
    }
}

void LibScanner::writeToDB(QString filePath)
{
    tagsReader->readInfoFromFile(filePath);
    dbModule->insertData(tagsReader->getArtist(),
                        tagsReader->getAlbum(),
                        tagsReader->getTitle(),
                        tagsReader->getGenre(),
                        filePath,
                        tagsReader->getDuration());
}
