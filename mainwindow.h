#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QThread>
#include <QLabel>
#include <QProgressBar>
#include <QItemSelectionModel>

#include "dbmodule.h"
#include "playmanager.h"
#include "libscanner.h"
#include "appsettingsmanager.h"

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
        LibScanner *libScanner = nullptr;
        AppSettingsManager *appSettings = nullptr;

        QLabel *statusBarLabel = nullptr;
        QProgressBar *statusProgressBar = nullptr;
        QThread thread;

        QItemSelectionModel *selectionModelTableViewGenre = nullptr;
        QItemSelectionModel *selectionModelTableViewArtists = nullptr;

        void selectGenres();
        void selectArtists(QString genre = "");
        void initGenreTableView();
        void initAeristsTableView(QString genre = "");
        void initPlaylist(QString genre = "", QString artist = "");

    private slots:
        void tooglePlayPause();
        void play();
        void stop();
        void serverOnLine(bool value);
        void displayData(PlayData value);
        void showScanProgress(QString value, int progress);
        void setupProgress(uint max);
        void scanFinished();

        void changeTableViewGenreCurrentSelection(const QModelIndex &current,
                    const QModelIndex &previous);

        void changeTableViewArtistCurrentSelection(const QModelIndex &current,
                    const QModelIndex &previous);
};
#endif // MAINWINDOW_H
