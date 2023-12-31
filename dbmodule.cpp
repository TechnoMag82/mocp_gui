#include "dbmodule.h"

DbModule::DbModule(QString connectionName)
{
    db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName("moc_gui.db");
    db.exec("PRAGMA locking_mode = NORMAL");
    if (db.open()) {
        qDebug() << "DB is Open: " << db.connectionName();
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
    if (modelPlaylist != nullptr)
        delete modelPlaylist;
}

void DbModule::insertData(QString artist, QString album, QString title, QString genre, QString path, uint duration)
{
    QSqlQuery insertQuery(db);

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
    insertQuery.finish();
}

void DbModule::clearTable()
{
    QSqlQuery deleteQuery(db);
    deleteQuery.exec("DELETE FROM album_table;");
    qDebug() << deleteQuery.lastError().text();
    deleteQuery.finish();
}

QString DbModule::getSelectedGenre(int row)
{
    QSqlRecord record = modelGenre->record(row);
    return record.value("genre").toString();
}

QString DbModule::getSelectedArtist(int row)
{
    QSqlRecord record = modelArtists->record(row);
    return record.value("artist").toString();
}

PlaylistItem DbModule::getPlayListItem(int row)
{
    PlaylistItem item;
    QSqlRecord record = modelPlaylist->record(row);
    item.artist = record.value("artist").toString();
    item.title = record.value("title").toString();
    item.path = record.value("path").toString();
    item.time = record.value("time").toUInt();
    item.id = record.value("id").toUInt();
    item.row = row;
    return item;
}

void DbModule::updateRating(uint id, uint rating)
{
    QSqlQuery query(db);
    query.prepare("UPDATE album_table SET rating=:rating WHERE id=:id;");
    query.bindValue(":rating", rating);
    query.bindValue(":id", id);
    query.exec();

    if (query.lastError().text().size() > 1) {
        qDebug() << "DB insert error: " << query.lastError().text();
    }
    query.finish();
}

PlaylistModel *DbModule::getPlaylist(QString genre, QString artist)
{
    if (modelPlaylist == nullptr);
        modelPlaylist = new PlaylistModel();
    QSqlQuery query(db);
    if (genre.isEmpty() && artist.isEmpty()) {
        query.prepare("SELECT * FROM album_table");
    } else if (artist.isEmpty() && !genre.isEmpty()) {
        query.prepare("SELECT * FROM album_table WHERE genre=:strgenre");
        query.bindValue(":strgenre", genre);
    } else if (!artist.isEmpty() && genre.isEmpty()) {
        query.prepare("SELECT * FROM album_table WHERE artist=:strartist");
        query.bindValue(":strartist", artist);
    } else if (!artist.isEmpty() && !genre.isEmpty()) {
        query.prepare("SELECT * FROM album_table WHERE artist=:strartist AND genre=:strgenre");
        query.bindValue(":strartist", artist);
        query.bindValue(":strgenre", genre);
    }
    query.exec();
    modelPlaylist->setQuery(query);
    if (modelPlaylist->lastError().isValid()) {
        qDebug() << modelPlaylist->lastError().text();
    }
    return modelPlaylist;
}

QSqlQueryModel * DbModule::getArtistsByGenre(QString genre)
{
    if (modelArtists == nullptr)
        modelArtists = new QSqlQueryModel();
    QSqlQuery query(db);
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
    QSqlQuery query(db);
    query.exec("SELECT DISTINCT genre FROM album_table ORDER BY genre;");
    modelGenre->setQuery(query);
    modelGenre->setHeaderData(0, Qt::Horizontal, "Genre");
    return modelGenre;
}

void DbModule::createDB()
{
    QSqlQuery query(db);
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

    QString createIndex = "CREATE UNIQUE INDEX IF NOT EXISTS idx_path"
                                " ON album_table (path);";
    query.exec(createIndex);
    if (query.lastError().text().size() > 1) {
        qDebug() << "Create DB error: " << query.lastError().text();
    }

    createIndex = "CREATE INDEX IF NOT EXISTS idx_genre"
                                " ON album_table (genre);";
    query.exec(createIndex);
    if (query.lastError().text().size() > 1) {
        qDebug() << "Create DB error: " << query.lastError().text();
    }

    createIndex = "CREATE INDEX IF NOT EXISTS idx_artist"
                                " ON album_table (artist);";
    query.exec(createIndex);
    if (query.lastError().text().size() > 1) {
        qDebug() << "Create DB error: " << query.lastError().text();
    }
    query.finish();
}
