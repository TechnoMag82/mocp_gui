#ifndef DBMODULE_H
#define DBMODULE_H

#include <QtSql>
#include <QDebug>
#include <QString>
#include <QSqlTableModel>

#include "models/playlistmodel.h"
#include "models/playlistitem.h"


class DbModule
{
    public:
        DbModule();
        ~DbModule();

        void insertData(
                QString artist,
                QString album,
                QString title,
                QString genre,
                QString path,
                uint duration
                );
        void clearTable();

        QString getSelectedGenre(int row);
        QString getSelectedArtist(int row);
        PlaylistItem getPlayListItem(int row);

        PlaylistModel * getPlaylist(QString genre = "", QString artist = "");
        QSqlQueryModel * getArtistsByGenre(QString genre = "");
        QSqlQueryModel * getGenres();

    private:
        QSqlDatabase db;
        QSqlQueryModel *modelGenre = nullptr;
        QSqlQueryModel *modelArtists = nullptr;
        PlaylistModel *modelPlaylist = nullptr;

        void createDB();
};

#endif // DBMODULE_H
