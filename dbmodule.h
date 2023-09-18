#ifndef DBMODULE_H
#define DBMODULE_H

#include <QtSql>
#include <QDebug>

class DbModule
{
    public:
        DbModule();
        ~DbModule();

        void createDB();

    private:
        QSqlDatabase db;
};

#endif // DBMODULE_H
