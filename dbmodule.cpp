#include "dbmodule.h"

DbModule::DbModule()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("moc_db.db");
    if (db.open()) {
        createDB();
    }
}

DbModule::~DbModule()
{
    if (db.isOpen()) {
        db.close();
    }
}

void DbModule::createDB()
{
    QSqlQuery query;
    QString createAlbum =  "CREATE TABLE IF NOT EXISTS album_table ("
                            "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                            " artist TEXT, "
                            " album TEXT, "
                            " genre TEXT, "
                            " rating INTEGER,"
                            " time INTEGER NOT NULL DEFAULT 0"
                           ");";
    query.exec(createAlbum);
    qDebug() << "error: " << query.lastError().text();
}
