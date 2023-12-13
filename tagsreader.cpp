#include "tagsreader.h"

TagsReader::TagsReader()
{

}

void TagsReader::readInfoFromFile(QString filePath)
{
    MI.Open(filePath.toStdWString());
    artist = QString::fromWCharArray(MI.Get(Stream_General, 0, __T("Artist"), Info_Text).c_str());
//    qDebug() << artist;
    title = QString::fromWCharArray(MI.Get(Stream_General, 0, __T("Title"), Info_Text).c_str());
//    qDebug() << title;
    album = QString::fromWCharArray(MI.Get(Stream_General, 0, __T("Album"), Info_Text).c_str());
//    qDebug() << album;
    genre = QString::fromWCharArray(MI.Get(Stream_General, 0, __T("Genre"), Info_Text).c_str());
//    qDebug() << genre;
    QString _duration = QString::fromWCharArray(MI.Get(Stream_General, 0, __T("Duration"), Info_Text).c_str());
    duration = _duration.toUInt();
    MI.Close();
}

QString TagsReader::getArtist()
{
    return artist;
}

QString TagsReader::getAlbum()
{
    return album;
}

QString TagsReader::getGenre()
{
    return genre;
}

QString TagsReader::getTitle()
{
    return title;
}

uint TagsReader::getDuration()
{
    return duration;
}
