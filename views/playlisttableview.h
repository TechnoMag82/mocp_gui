#ifndef PLAYLISTTABLEVIEW_H
#define PLAYLISTTABLEVIEW_H

#include <QTableView>
#include <QWidget>
#include <QPainter>
#include <QImage>
#include <QPixmap>

class PlayListTableView : public QTableView
{
    Q_OBJECT
    public:
        PlayListTableView(QWidget *parent = 0);
        void setImage(QString image);
    protected:
        void paintEvent(QPaintEvent* event);
        void scrollContentsBy(int dx, int dy);
    private:
        QImage background_image_;
        QPixmap image;

};

#endif // PLAYLISTTABLEVIEW_H
