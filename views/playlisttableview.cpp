#include "playlisttableview.h"

PlayListTableView::PlayListTableView(QWidget *parent) : QTableView(parent)
{

}

void PlayListTableView::setImage(QString image)
{
    background_image_ = QImage(image);
}

void PlayListTableView::paintEvent(QPaintEvent *event)
{
    QPainter background_painter(viewport());
    background_painter.setOpacity(0.5);
    background_painter.drawImage(QPoint(0, 0), background_image_);
    QTableView::paintEvent(event);
}

void PlayListTableView::scrollContentsBy(int dx, int dy)
{
    viewport()->update();
    QTableView::scrollContentsBy(dx, dy);
}
