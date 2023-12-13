#include "dbmodule.h"

DbModule::DbModule()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("moc_gui.db");
    if (db.open()) {
        qDebug() << "DB is Open";
        createDB();
    }
}

DbModule::~DbModule()
{
    if (db.isOpen()) {
        db.close();
    }
    if (modelArtists != nullptr)
        delete modelArtists;
    if (modelGenre != nullptr)
        delete modelGenre;
}

void DbModule::insertData(QString artist, QString album, QString title, QString genre, QString path, uint duration)
{
    QSqlQuery insertQuery;

    insertQuery.prepare("INSERT INTO album_table (artist, album, title, genre, path, time)"
                  " VALUES (:artist, :album, :title, :genre, :path, :time);");

    insertQuery.bindValue(":artist", artist);
    insertQuery.bindValue(":album", album);
    insertQuery.bindValue(":title", title);
    insertQuery.bindValue(":genre", genre);
    insertQuery.bindValue(":path", path);
    insertQuery.bindValue(":time", duration);

    insertQuery.exec();

    if (insertQuery.lastError().text().size() > 1) {
        qDebug() << "DB insert error: " << insertQuery.lastError().text();
    }
}

void DbModule::clearTable()
{
    QSqlQuery deleteQuery;
    deleteQuery.exec("DELETE * FROM album_table;");
}

QString DbModule::getSelectedGenre(int row)
{
    QSqlRecord record = modelGenre->record(row);
    return record.value("genre").toString();
}

QSqlQueryModel * DbModule::getArtistsByGenre(QString genre)
{
    if (modelArtists == nullptr)
        modelArtists = new QSqlQueryModel();
    QSqlQuery query;
    if (genre.isEmpty()) {
        query.prepare("SELECT DISTINCT artist FROM album_table ORDER BY artist;");
    } else {
        query.prepare("SELECT DISTINCT artist FROM album_table WHERE genre=:strgenre ORDER BY artist;");
        query.bindValue(":strgenre", genre);
    }
    query.exec();
    modelArtists->setQuery(query);
    if (modelArtists->lastError().isValid()) {
        qDebug() << modelArtists->lastError().text();
    }
    modelArtists->setHeaderData(0, Qt::Horizontal, "Artist");
    return modelArtists;
}

QSqlQueryModel * DbModule::getGenres()
{
    if (modelGenre == nullptr)
        modelGenre = new QSqlQueryModel();
    modelGenre->setQuery("SELECT DISTINCT genre FROM album_table ORDER BY genre;");
    modelGenre->setHeaderData(0, Qt::Horizontal, "Genre");
    return modelGenre;
}

void DbModule::createDB()
{
    QSqlQuery query;
    QString createAlbum =  "CREATE TABLE IF NOT EXISTS album_table ("
                            "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                            " artist TEXT,"
                            " album TEXT,"
                            " title TEXT,"
                            " genre TEXT,"
                            " path TEXT NOT NULL,"
                            " rating INTEGER NOT NULL DEFAULT 0,"
                            " playcount INTEGER NOT NULL DEFAULT 0,"
                            " time INTEGER NOT NULL DEFAULT 0"
                           ");";
    query.exec(createAlbum);
    if (query.lastError().text().size() > 1) {
        qDebug() << "Create DB error: " << query.lastError().text();
    }

    QString createUniqueIndex = "CREATE UNIQUE INDEX IF NOT EXISTS idx_path"
                                " ON album_table (path);";
    query.exec(createUniqueIndex);
    if (query.lastError().text().size() > 1) {
        qDebug() << "Create DB error: " << query.lastError().text();
    }
}
