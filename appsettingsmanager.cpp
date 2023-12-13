#include "appsettingsmanager.h"

AppSettingsManager::AppSettingsManager()
{
    settings = new QSettings("technomag", "mocgui");
}

AppSettingsManager::~AppSettingsManager()
{
    delete settings;
}

bool AppSettingsManager::setLibraryPath(QString path)
{
    QString oldPath = settings->value(KEY_LIBPATH, "").toString();
    settings->setValue(KEY_LIBPATH, path);
    return oldPath == path;
}

QString AppSettingsManager::getLibraryPath()
{
    return settings->value(KEY_LIBPATH, "").toString();
}

void AppSettingsManager::setScanCompleted(bool value)
{
    settings->setValue(KEY_SCAN_HAS_BEEN_COMPLETED, value);
}

bool AppSettingsManager::isScanHasBeenFinished()
{
    return settings->value(KEY_SCAN_HAS_BEEN_COMPLETED, false).toBool();
}
