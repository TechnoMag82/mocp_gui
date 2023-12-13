#ifndef APPSETTINGSMANAGER_H
#define APPSETTINGSMANAGER_H

#include <QSettings>
#include <QString>

class AppSettingsManager
{
    public:
        AppSettingsManager();
        ~AppSettingsManager();

        // Result true if is path chamged for rescan library
        bool setLibraryPath(QString path);
        QString getLibraryPath();

        void setScanCompleted(bool value);
        bool isScanHasBeenFinished();

        // Qeuery example: genre;artist
        void setFilterQuery(QString query);
        QString getFilterQuery();

        void setLastSoundPath(QString path);
        QString getLastSoundPath();

    private:
        QSettings *settings = nullptr;

        const QString KEY_LIBPATH = "libpath";
        const QString KEY_SCAN_HAS_BEEN_COMPLETED = "scancompleted";
};

#endif // APPSETTINGSMANAGER_H
