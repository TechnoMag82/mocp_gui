#ifndef PLAYLISTITEM_H
#define PLAYLISTITEM_H

#include <QString>

struct PlaylistItem {
    QString artist;
    QString title;
    QString path;
    uint time;
    int row;
};

#endif // PLAYLISTITEM_H
