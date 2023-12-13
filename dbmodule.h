#ifndef DBMODULE_H
#define DBMODULE_H

#include <QtSql>
#include <QDebug>
#include <QString>
#include <QSqlTableModel>

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

        QSqlQueryModel * getArtistsByGenre(QString genre);
        QSqlQueryModel * getGenres();

    private:
        QSqlDatabase db;
        QSqlQueryModel *modelGenre = nullptr;
        QSqlQueryModel *modelArtists = nullptr;

        void createDB();
};

#endif // DBMODULE_H
