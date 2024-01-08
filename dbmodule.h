#ifndef DBMODULE_H
#define DBMODULE_H

#include <QtSql>
#include <QDebug>
#include <QString>
#include <QSqlTableModel>

#include "consts.h"

#include "models/playlistmodel.h"
#include "models/playlistitem.h"


class DbModule
{
    public:
        DbModule(QString connectionName);
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
        void updateRating(QString path, uint rating);
        void updatePlayCount(QString path);

        PlaylistModel * getPlaylist(QString genre = "", QString artist = "");
        QSqlQueryModel * getArtistsByGenre(QString genre = "");
        QSqlQueryModel * getGenres();

    private:
        QSqlDatabase db;
        QSqlQueryModel *modelGenre = nullptr;
        QSqlQueryModel *modelArtists = nullptr;
        PlaylistModel *modelPlaylist = nullptr;

        void createDB();

        const QString basePlayListSelection = "SELECT "
                                                  " alb.id,"
                                                  " alb.artist,"
                                                  " alb.album,"
                                                  " alb.title,"
                                                  " alb.genre,"
                                                  " alb.path,"
                                                  " rate.rating,"
                                                  " rate.playcount,"
                                                  " alb.time"
                                              " FROM album_table alb"
                                              " JOIN rating_table rate ON alb.path=rate.path ";
};

#endif // DBMODULE_H
