#ifndef PLAYLISTITEM_H
#define PLAYLISTITEM_H

#include <QString>

struct PlaylistItem {
    uint id;
    QString artist;
    QString title;
    QString path;
    uint time;
    uint row;
};

#endif // PLAYLISTITEM_H
