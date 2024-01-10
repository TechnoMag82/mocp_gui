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
    if (role == Qt::BackgroundRole) {
        if (index.row() % 2 ==0) {
            return QVariant(QBrush(QColor(230, 230, 230, 100)));
        } else {
            return QVariant(QBrush(QColor(255, 255, 255, 100)));
        }
    }
    if (index.column() == 8) {
        if (role == Qt::DisplayRole) {
            return timeToString(QSqlQueryModel::data(index, role).toUInt());
        }
    }
    if (index.column() == 6) {
        if (role == Qt::DecorationRole) {
            int rating = QSqlQueryModel::data(index, Qt::DisplayRole).toInt();
            QPixmap pixmap(92, 20);
            pixmap.fill(Qt::transparent);
            QPainter painter(&pixmap);
            switch (rating) {
                case 0:
                    painter.drawPixmap(0, 0, 92, 20, pixmapNoRating);
                    break;
                case 1:
                    painter.drawPixmap(0, 0, 92, 20, pixmap1r);
                    break;
                case 2:
                    painter.drawPixmap(0, 0, 92, 20, pixmap2r);
                    break;
                case 3:
                    painter.drawPixmap(0, 0, 92, 20, pixmap3r);
                    break;
                case 4:
                    painter.drawPixmap(0, 0, 92, 20, pixmap4r);
                    break;
                case 5:
                    painter.drawPixmap(0, 0, 92, 20, pixmap5r);
                    break;
                default:
                    painter.drawPixmap(0, 0, 92, 20, pixmapNoRating);
                    break;
            }
            return pixmap;
        } else if (role == Qt::DisplayRole) {
            return QVariant();
        }
    }
    return QSqlQueryModel::data(index, role);
}
