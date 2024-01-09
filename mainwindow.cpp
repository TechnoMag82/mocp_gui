#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "libscanner.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/common/images/main_icon.png"));
    setWindowState(Qt::WindowMaximized);
    initMainMenu();
    ui->sliderPlayProgress->setMinimum(0);
    ui->sliderVolume->setMinimum(0);
    ui->sliderVolume->setMaximum(100);
    ui->tableViewGenre->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableViewGenre->verticalHeader()->hide();
    ui->tableViewArtist->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableViewArtist->verticalHeader()->hide();
    ui->tableViewPlaylist->verticalHeader()->hide();

    // Init app settings

    appSettings = new AppSettingsManager();

    // Init statusBar

    statusBarLabel = new QLabel(this);
    ui->statusbar->addWidget(statusBarLabel);

    statusProgressBar = new QProgressBar(this);
    statusProgressBar->setVisible(false);
    ui->statusbar->addWidget(statusProgressBar);
    statusProgressBar->setFixedWidth(200);

    statusBarLabel->setMaximumWidth(this->width() - 300);

    createDbPath();
    dbModule = new DbModule("sql_main");

    // Scan music library

    if (libScanner == nullptr && !appSettings->isScanHasBeenFinished()) {
        lockPlayerControls(true);
        libScanner = new LibScanner(appSettings->getLibraryPath());
        connect(libScanner, SIGNAL(scanOnProgress(QString, int)), this, SLOT(showScanProgress(QString, int)));
        connect(libScanner, SIGNAL(scanCompleted()), this, SLOT(scanFinished()));
        connect(libScanner, SIGNAL(progressIsInit(uint)), this, SLOT(setupProgress(uint)));
        libScanner->start();
    }

    // Init playing

    playManager = new PlayManager(this);
    playManager->startServer();
    playManager->connectToServer();
    connect(playManager, SIGNAL(serverIsRunning(bool)), this, SLOT(serverOnLine(bool)));
    if (playManager->pingServer() == true) {
        playManager->moveToThread(&thread);
        playManager->getCurrentFileInformation();
        connect(playManager, SIGNAL(playDataChanged(PlayData)), this, SLOT(displayData(PlayData)));
        connect(playManager, SIGNAL(songIsEnd()), this, SLOT(playNext()));
        connect(&thread, SIGNAL(started()), playManager, SLOT(doWork()));
        thread.start();
    }
    connect(ui->btnPlayPause, SIGNAL(clicked()), this, SLOT(tooglePlayPause()));
    connect(ui->btnPlay, SIGNAL(clicked()), this, SLOT(play()));
    connect(ui->btnStop, SIGNAL(clicked()), this, SLOT(stop()));
    connect(ui->sliderVolume, SIGNAL(sliderReleased()), this, SLOT(sliderVolumeReleasedByUser()));

    connect(ui->sliderPlayProgress, SIGNAL(sliderReleased()), this, SLOT(sliderProgressReleasedByUser()));
    connect(ui->sliderPlayProgress, SIGNAL(sliderMoved(int)), this, SLOT(showSliderTime(int)));

    initGenreTableView();
    initArtistsTableView();
    initPlaylistTableView();
}

MainWindow::~MainWindow()
{
    if (libScanner != nullptr && libScanner->isRunning()) {
        libScanner->terminate();
    }
    playManager->stopWatch();
    thread.quit();
    thread.wait();
    playManager->disconnectFromServer();
    if (libScanner != nullptr)
        delete libScanner;
    delete playManager;
    delete dbModule;
    delete appSettings;
    delete ui;
}

void MainWindow::selectGenres()
{
    ui->tableViewGenre->setModel(dbModule->getGenres());
    ui->tableViewGenre->show();
}

void MainWindow::selectArtists(QString genre)
{
    ui->tableViewArtist->setModel(dbModule->getArtistsByGenre(genre));
    ui->tableViewArtist->show();
}

void MainWindow::selectPlaylist(QString genre, QString artist)
{
    selectionPlaylistChanged = true;
    ui->tableViewPlaylist->setModel(dbModule->getPlaylist(genre, artist));
    ui->tableViewPlaylist->setColumnHidden(COLUMN_ID, true);
    ui->tableViewPlaylist->setColumnHidden(COLUMN_ALBUM, true);
    ui->tableViewPlaylist->setColumnHidden(COLUMN_PATH, true);
    ui->tableViewPlaylist->setColumnWidth(COLUMN_ARTIST, 250);
    ui->tableViewPlaylist->setColumnWidth(COLUMN_ALBUM, 250);
    ui->tableViewPlaylist->setColumnWidth(COLUMN_TITLE, 300);
    ui->tableViewPlaylist->setColumnWidth(COLUMN_GENRE, 250);
    ui->tableViewPlaylist->setColumnWidth(COLUMN_RATING, 124);
    ui->tableViewPlaylist->show();
}

void MainWindow::initGenreTableView()
{
    selectGenres();
    selectionModelTableViewGenre = ui->tableViewGenre->selectionModel();
    if (selectionModelTableViewGenre != nullptr) {
            connect(selectionModelTableViewGenre,
                    SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
                    this,
                    SLOT(changeTableViewGenreCurrentSelection(const QModelIndex &, const QModelIndex &)),
                    Qt::UniqueConnection);
    }
}

void MainWindow::initArtistsTableView(QString genre)
{
    selectArtists();
    selectionModelTableViewArtists = ui->tableViewArtist->selectionModel();
    if (selectionModelTableViewArtists != nullptr) {
            connect(selectionModelTableViewArtists,
                    SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
                    this,
                    SLOT(changeTableViewArtistCurrentSelection(const QModelIndex &, const QModelIndex &)),
                    Qt::UniqueConnection);
    }
}

void MainWindow::initPlaylistTableView(QString genre, QString artist)
{
    selectPlaylist(selectedGenre, selectedArtist);
    connect(ui->tableViewPlaylist,
            SIGNAL(doubleClicked(const QModelIndex &)),
            this,
            SLOT(playlistDoubleClicked(const QModelIndex &)),
            Qt::UniqueConnection);
}

void MainWindow::initMainMenu()
{
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(menuAboutDialog()));
    connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(menuSettingsDialog()));
    connect(ui->actionRescanMusicFolder, SIGNAL(triggered()), this, SLOT(startRescanLibrary()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(menuExit()));
    connect(ui->actionExitAndStopServer, SIGNAL(triggered()), this, SLOT(menuExitAndStopServer()));

    // ratings submenu
    connect(ui->actionNoRating, SIGNAL(triggered()), this, SLOT(menuSetRating()));
    connect(ui->actionRating1, SIGNAL(triggered()), this, SLOT(menuSetRating()));
    connect(ui->actionRating2, SIGNAL(triggered()), this, SLOT(menuSetRating()));
    connect(ui->actionRating3, SIGNAL(triggered()), this, SLOT(menuSetRating()));
    connect(ui->actionRating4, SIGNAL(triggered()), this, SLOT(menuSetRating()));
    connect(ui->actionRating5, SIGNAL(triggered()), this, SLOT(menuSetRating()));

    ui->actionNoRating->setData(QVariant(0));
    ui->actionRating1->setData(QVariant(1));
    ui->actionRating2->setData(QVariant(2));
    ui->actionRating3->setData(QVariant(3));
    ui->actionRating4->setData(QVariant(4));
    ui->actionRating5->setData(QVariant(5));
}

void MainWindow::setArtistAndTitleLabels(QString artist, QString title)
{
    ui->labelArtist->setText(artist);
    ui->labelTitle->setText(title);
}

void MainWindow::lockPlayerControls(bool lock)
{
    ui->btnPlay->setEnabled(!lock);
    ui->btnPlayPause->setEnabled(!lock);
    ui->btnStop->setEnabled(!lock);
    ui->sliderPlayProgress->setEnabled(!lock);
    ui->actionRescanMusicFolder->setEnabled(!lock);
    ui->tableViewGenre->setEnabled(!lock);
    ui->tableViewArtist->setEnabled(!lock);
    ui->tableViewPlaylist->setEnabled(!lock);
}

void MainWindow::createDbPath()
{
    QString homeDir = QDir::homePath();
    QDir dir(homeDir);
    if (!dir.exists(homeDir + PROGRAM_DIR)) {
        dir.mkdir(homeDir + PROGRAM_DIR);
    }
}

void MainWindow::startRescanLibrary()
{
    if (libScanner == nullptr) {
        lockPlayerControls(true);
        libScanner = new LibScanner(appSettings->getLibraryPath(), true);
        connect(libScanner, SIGNAL(scanOnProgress(QString, int)), this, SLOT(showScanProgress(QString, int)));
        connect(libScanner, SIGNAL(scanCompleted()), this, SLOT(scanFinished()));
        connect(libScanner, SIGNAL(progressIsInit(uint)), this, SLOT(setupProgress(uint)));
        libScanner->start();
    }
}

void MainWindow::tooglePlayPause()
{
    playManager->togglePlayPause();
}

void MainWindow::play()
{
    if (!currentPlayListItem.path.isEmpty()) {
        setArtistAndTitleLabels(currentPlayListItem.artist, currentPlayListItem.title);
        playManager->play(currentPlayListItem.path.toLocal8Bit().data());
    }
}

void MainWindow::stop()
{
    ui->labelArtist->setText("Nothing plays");
    ui->labelTitle->clear();
    playManager->stop(true);
}

void MainWindow::playNext()
{
    if (currentPlayListItem.row + 1 < ui->tableViewPlaylist->model()->rowCount()) {
        dbModule->updatePlayCount(currentPlayListItem.path);
        currentPlayListItem = dbModule->getPlayListItem(currentPlayListItem.row + 1);
        ui->tableViewPlaylist->selectRow(currentPlayListItem.row);
        playManager->stop();
        play();
    }
}

void MainWindow::serverOnLine(bool value)
{
    ui->labelErrorMessage->setText("Server is offline. Please, try to restart this application.");
    ui->labelErrorMessage->setVisible(!value);
    ui->btnPlay->setEnabled(value);
    ui->btnPlayPause->setEnabled(value);
    ui->btnStop->setEnabled(value);
    ui->sliderPlayProgress->setEnabled(value);
    ui->sliderVolume->setEnabled(value);
}

void MainWindow::displayData(PlayData value)
{
    ui->sliderPlayProgress->setMaximum(value.totalTime);
    if (!ui->sliderPlayProgress->isSliderDown()) {
        ui->sliderPlayProgress->setValue(value.progress);
    }
    ui->labelPlayTime->setText(value.playTime);
    if (!ui->sliderVolume->isSliderDown()) {
        ui->sliderVolume->setValue(value.volume);
    }
}

void MainWindow::showScanProgress(QString value, int progress)
{
    statusBarLabel->setText(value);
    statusProgressBar->setValue(progress);
}

void MainWindow::setupProgress(uint max)
{
    statusProgressBar->setMaximum(max);
    statusProgressBar->setVisible(true);
}

void MainWindow::scanFinished()
{
    appSettings->setScanCompleted(true);
    statusBarLabel->clear();
    statusProgressBar->setVisible(false);
    if (libScanner != nullptr) {
        delete libScanner;
        libScanner = nullptr;
    }
    selectGenres();
    selectArtists();
    selectPlaylist();
    lockPlayerControls(false);
}

void MainWindow::sliderProgressReleasedByUser()
{
    playManager->goToTime(ui->sliderPlayProgress->value());
}

void MainWindow::sliderVolumeReleasedByUser()
{
    playManager->setVolume(ui->sliderVolume->value());
}

void MainWindow::changeTableViewGenreCurrentSelection(const QModelIndex &current, const QModelIndex &previous)
{
    selectedGenre = dbModule->getSelectedGenre(current.row());
    selectArtists(selectedGenre);
    selectPlaylist(selectedGenre, "");
}

void MainWindow::changeTableViewArtistCurrentSelection(const QModelIndex &current, const QModelIndex &previous)
{
    selectedArtist = dbModule->getSelectedArtist(current.row());
    selectPlaylist(selectedGenre, selectedArtist);
}

void MainWindow::playlistDoubleClicked(const QModelIndex &current)
{
    playManager->stop();
    currentPlayListItem = dbModule->getPlayListItem(current.row());
    play();
}

void MainWindow::showSliderTime(int value)
{
    QPoint globalPos = ui->sliderPlayProgress->mapToGlobal(ui->sliderPlayProgress->rect().topLeft());
    QToolTip::showText(QPoint(QCursor::pos().x() - 54, globalPos.y() - 56),
                       QDateTime::fromTime_t(value).toUTC().toString("hh:mm:ss"),
                       nullptr);
}

void MainWindow::menuSettingsDialog()
{
    SettingsDialog *dialog = new SettingsDialog(this);
    dialog->setLibraryPath(appSettings->getLibraryPath());
    if (dialog->exec() == QDialog::Accepted) {
        if (appSettings->setLibraryPath(dialog->getLibraryPath())) {
            startRescanLibrary();
        }
    }
    delete dialog;
}

void MainWindow::menuAboutDialog()
{
    AboutDialog *dialog = new AboutDialog(this);
    dialog->exec();
    delete dialog;
}

void MainWindow::menuExit()
{
    close();
}

void MainWindow::menuExitAndStopServer()
{
    playManager->stopServer();
    close();
}

void MainWindow::menuSetRating()
{
    QAction *sender = (QAction*) QObject::sender();
    dbModule->updateRating(currentPlayListItem.path, sender->data().toUInt());
    selectPlaylist(selectedGenre, selectedArtist);
}
