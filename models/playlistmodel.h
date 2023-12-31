#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QSqlQueryModel>
#include <QPixmap>
#include <QPainter>
#include <QTime>
#include <QDebug>

#include "utils.cpp"

class PlaylistModel : public QSqlQueryModel
{
    Q_OBJECT

    public:
        explicit PlaylistModel(QObject *parent = nullptr);
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    private:
        QPixmap pixmapNoRating;
        QPixmap pixmap1r;
        QPixmap pixmap2r;
        QPixmap pixmap3r;
        QPixmap pixmap4r;
        QPixmap pixmap5r;
};

#endif // PLAYLISTMODEL_H
