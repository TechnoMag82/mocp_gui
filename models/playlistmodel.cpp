#include "playlistmodel.h"


PlaylistModel::PlaylistModel(QObject *parent) :
     QSqlQueryModel(parent)
{
    pixmapNoRating.load(":/rating/images/no_rating.png");
    pixmap1r.load(":/rating/images/r1.png");
    pixmap2r.load(":/rating/images/r2.png");
    pixmap3r.load(":/rating/images/r3.png");
    pixmap4r.load(":/rating/images/r4.png");
    pixmap5r.load(":/rating/images/r5.png");
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    if (index.column() == 8) {
        if (role == Qt::DisplayRole) {
            return timeToString(QSqlQueryModel::data(index, role).toUInt());
        }
    }
    if (index.column() == 6) {
        if (role == Qt::DecorationRole) {
            int rating = QSqlQueryModel::data(index, role).toInt();
            QPixmap pixmap(116, 24);
            QPainter painter(&pixmap);
            switch (rating) {
                case 1:
                    painter.drawPixmap(0, 0, 116, 24, pixmap1r);
                    break;
                case 2:
                    painter.drawPixmap(0, 0, 116, 24, pixmap2r);
                    break;
                case 3:
                    painter.drawPixmap(0, 0, 116, 24, pixmap3r);
                    break;
                case 4:
                    painter.drawPixmap(0, 0, 116, 24, pixmap4r);
                    break;
                case 5:
                    painter.drawPixmap(0, 0, 116, 24, pixmap5r);
                    break;
                default:
                    painter.drawPixmap(0, 0, 116, 24, pixmapNoRating);
                    break;
            }
            return pixmap;
        } else if (role == Qt::DisplayRole) {
            return QVariant();
        }
    }
    return QSqlQueryModel::data(index, role);
}
