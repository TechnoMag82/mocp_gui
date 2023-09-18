#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>

#include "dbmodule.h"
#include "playmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

    private:
        Ui::MainWindow *ui;
        DbModule *dbModule = nullptr;
        PlayManager *playManager = nullptr;

    private slots:
        void tooglePlayPause();
        void play();
        void stop();
        void serverOnLine(bool value);
};
#endif // MAINWINDOW_H
