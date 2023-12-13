#ifndef TAGSREADER_H
#define TAGSREADER_H

#define _UNICODE

#include <QString>
#include <QDebug>

#include <MediaInfo/MediaInfo.h>

using namespace MediaInfoLib;

class TagsReader
{
    public:
        TagsReader();

        void readInfoFromFile(QString filePath);

        QString getArtist();
        QString getAlbum();
        QString getGenre();
        QString getTitle();
        uint getDuration();

    private:
        MediaInfo MI;

        QString artist;
        QString title;
        QString album;
        QString genre;
        uint duration;
};

#endif // TAGSREADER_H
