#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QThread>
#include <QLabel>
#include <QProgressBar>
#include <QItemSelectionModel>
#include <QToolTip>
#include <QAction>

#include "dbmodule.h"
#include "playmanager.h"
#include "libscanner.h"
#include "appsettingsmanager.h"
#include "consts.h"

#include "aboutdialog.h"
#include "settingsdialog.h"

#include "models/playlistitem.h"

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
        QItemSelectionModel *selectionModelTableViewPlaylist = nullptr;

        QString selectedGenre = "";
        QString selectedArtist = "";
        bool selectionPlaylistChanged = true;
        PlaylistItem currentPlayListItem;

        void selectGenres();
        void selectArtists(QString genre = "");
        void selectPlaylist(QString genre = "", QString artist = "");

        void initGenreTableView();
        void initArtistsTableView(QString genre = "");
        void initPlaylistTableView(QString genre = "", QString artist = "");
        void initMainMenu();

        void setArtistAndTitleLabels(QString artist, QString title);
        void lockPlayerControls(bool lock);

    private slots:
        void tooglePlayPause();
        void play();
        void stop();
        void playNext();
        void serverOnLine(bool value);
        void displayData(PlayData value);
        void showScanProgress(QString value, int progress);
        void setupProgress(uint max);
        void scanFinished();

        void sliderProgressReleasedByUser();
        void sliderVolumeReleasedByUser();

        void changeTableViewGenreCurrentSelection(const QModelIndex &current,
                    const QModelIndex &previous);

        void changeTableViewArtistCurrentSelection(const QModelIndex &current,
                    const QModelIndex &previous);

        void playlistDoubleClicked(const QModelIndex &current);

        void showSliderTime(int value);
        void startRescanLibrary();

        void menuSettingsDialog();
        void menuAboutDialog();
        void menuExit();
        void menuExitAndStopServer();
        void menuSetRating();
};
#endif // MAINWINDOW_H
