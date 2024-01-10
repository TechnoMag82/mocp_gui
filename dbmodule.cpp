#include "dbmodule.h"

DbModule::DbModule(QString connectionName)
{
    db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName( QDir::homePath() + PROGRAM_DIR + "/moc_gui.db");
    if (db.open()) {
        db.exec("PRAGMA locking_mode = NORMAL");
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
    if (genresQuery != nullptr)
        delete genresQuery;
    if (artistsQuery != nullptr)
        delete artistsQuery;
    if (playlistQuery != nullptr)
        delete playlistQuery;
    if (insertQuery != nullptr)
        delete insertQuery;
}

void DbModule::insertData(QString artist, QString album, QString title, QString genre, QString path, uint duration)
{
    if (insertQuery == nullptr)
        insertQuery = new QSqlQuery(db);

    insertQuery->prepare("INSERT INTO album_table (artist, album, title, genre, path, time)"
                  " VALUES (:artist, :album, :title, :genre, :path, :time);");

    insertQuery->bindValue(":artist", artist);
    insertQuery->bindValue(":album", album);
    insertQuery->bindValue(":title", title);
    insertQuery->bindValue(":genre", genre);
    insertQuery->bindValue(":path", path);
    insertQuery->bindValue(":time", duration);
    insertQuery->exec();

    if (insertQuery->lastError().text().size() > 1) {
        qDebug() << "album_table insert error: " << insertQuery->lastError().text();
    }

    insertQuery->prepare("INSERT INTO rating_table (path)"
                  " VALUES (:path);");
    insertQuery->bindValue(":path", path);
    insertQuery->exec();

    if (insertQuery->lastError().text().size() > 1) {
        qDebug() << "rating_table insert error: " << insertQuery->lastError().text();
    }

    insertQuery->finish();
}

void DbModule::clearTable()
{
    QSqlQuery deleteQuery(db);
    deleteQuery.exec("DELETE FROM album_table;");
    qDebug() << "Clear table error: " << deleteQuery.lastError().text();
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

void DbModule::updateRating(QString path, uint rating)
{
    QSqlQuery query(db);
    query.prepare("UPDATE rating_table SET rating=:rating WHERE path=:path;");
    query.bindValue(":rating", rating);
    query.bindValue(":path", path);
    query.exec();

    if (query.lastError().text().size() > 1) {
        qDebug() << "Update rating error: " << query.lastError().text();
    }
    query.finish();
}

void DbModule::updatePlayCount(QString path)
{
    QSqlQuery query(db);
    query.prepare("UPDATE rating_table"
                  " SET playcount=((SELECT playcount FROM rating_table WHERE path=:path) + 1)"
                  " WHERE path=:path;");
    query.bindValue(":path", path);
    query.exec();

    if (query.lastError().text().size() > 1) {
        qDebug() << "update playcount error: " << query.lastError().text();
    }
    query.finish();
}

PlaylistModel *DbModule::getPlaylist(QString genre, QString artist)
{
    if (modelPlaylist == nullptr);
        modelPlaylist = new PlaylistModel();
    if (playlistQuery == nullptr)
        playlistQuery = new QSqlQuery(db);
    if (genre.isEmpty() && artist.isEmpty()) {
        playlistQuery->prepare(basePlayListSelection);
    } else if (artist.isEmpty() && !genre.isEmpty()) {
        playlistQuery->prepare(basePlayListSelection + "WHERE genre=:strgenre");
        playlistQuery->bindValue(":strgenre", genre);
    } else if (!artist.isEmpty() && genre.isEmpty()) {
        playlistQuery->prepare(basePlayListSelection + "WHERE artist=:strartist");
        playlistQuery->bindValue(":strartist", artist);
    } else if (!artist.isEmpty() && !genre.isEmpty()) {
        playlistQuery->prepare(basePlayListSelection + "WHERE artist=:strartist AND genre=:strgenre");
        playlistQuery->bindValue(":strartist", artist);
        playlistQuery->bindValue(":strgenre", genre);
    }
    playlistQuery->exec();
    modelPlaylist->setQuery(*playlistQuery);
    if (modelPlaylist->lastError().isValid()) {
        qDebug() << modelPlaylist->lastError().text();
    }
    return modelPlaylist;
}

QSqlQueryModel * DbModule::getArtistsByGenre(QString genre)
{
    if (modelArtists == nullptr)
        modelArtists = new QSqlQueryModel();
    if (artistsQuery == nullptr)
        artistsQuery = new QSqlQuery(db);
    if (genre.isEmpty()) {
        artistsQuery->prepare("SELECT DISTINCT artist FROM album_table ORDER BY artist;");
    } else {
        artistsQuery->prepare("SELECT DISTINCT artist FROM album_table WHERE genre=:strgenre ORDER BY artist;");
        artistsQuery->bindValue(":strgenre", genre);
    }
    artistsQuery->exec();
    modelArtists->setQuery(*artistsQuery);
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
    if (genresQuery == nullptr);
        genresQuery = new QSqlQuery(db);
    genresQuery->exec("SELECT DISTINCT genre FROM album_table ORDER BY genre;");
    modelGenre->setQuery(*genresQuery);
    modelGenre->setHeaderData(0, Qt::Horizontal, "Genre");
    return modelGenre;
}

void DbModule::finshTableQueries()
{
    genresQuery->finish();
    artistsQuery->finish();
    playlistQuery->finish();
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
                            " time INTEGER NOT NULL DEFAULT 0"
                           ");";
    query.exec(createAlbum);
    if (query.lastError().text().size() > 1) {
        qDebug() << "Create album_table error: " << query.lastError().text();
    }

    QString createIndex = "CREATE UNIQUE INDEX IF NOT EXISTS idx_path"
                                " ON album_table (path);";
    query.exec(createIndex);
    if (query.lastError().text().size() > 1) {
        qDebug() << "Create idx_path error: " << query.lastError().text();
    }

    createIndex = "CREATE INDEX IF NOT EXISTS idx_genre"
                                " ON album_table (genre);";
    query.exec(createIndex);
    if (query.lastError().text().size() > 1) {
        qDebug() << "Create idx_genre error: " << query.lastError().text();
    }

    createIndex = "CREATE INDEX IF NOT EXISTS idx_artist"
                                " ON album_table (artist);";
    query.exec(createIndex);
    if (query.lastError().text().size() > 1) {
        qDebug() << "Create idx_artist error: " << query.lastError().text();
    }

    QString createRating = "CREATE TABLE IF NOT EXISTS rating_table ("
                            " id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                            " path TEXT NOT NULL,"
                            " rating INTEGER NOT NULL DEFAULT 0,"
                            " playcount INTEGER NOT NULL DEFAULT 0"
                           ");";
    query.exec(createRating);
    if (query.lastError().text().size() > 1) {
        qDebug() << "Create rating_table error: " << query.lastError().text();
    }

    createIndex = "CREATE UNIQUE INDEX IF NOT EXISTS idx_ratepath"
                                " ON rating_table (path);";
    query.exec(createIndex);
    if (query.lastError().text().size() > 1) {
        qDebug() << "Create idx_ratepath error: " << query.lastError().text();
    }

    query.finish();
}
