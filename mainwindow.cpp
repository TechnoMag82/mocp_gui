#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "libscanner.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->sliderPlayProgress->setMinimum(0);
    ui->sliderVolume->setMinimum(0);
    ui->sliderVolume->setMaximum(100);
    ui->tableViewGenre->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableViewGenre->verticalHeader()->hide();
    ui->tableViewArtist->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableViewArtist->verticalHeader()->hide();

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

    dbModule = new DbModule();

    // Scan music library

    appSettings->setLibraryPath("/mnt/BIG_GEEK/Music"); // TODO: set path to lib from Options dialog

    if (!appSettings->isScanHasBeenFinished()) {
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
        connect(&thread, SIGNAL(started()), playManager, SLOT(doWork()));
        thread.start();
    }
    connect(ui->btnPlayPause, SIGNAL(clicked()), this, SLOT(tooglePlayPause()));
    connect(ui->btnPlay, SIGNAL(clicked()), this, SLOT(play()));
    connect(ui->btnStop, SIGNAL(clicked()), this, SLOT(stop()));

    initGenreTableView();
    initAeristsTableView();
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

void MainWindow::initGenreTableView()
{
    selectGenres();
    selectionModelTableViewGenre = ui->tableViewGenre->selectionModel();
    if (selectionModelTableViewGenre != nullptr) {
            connect(selectionModelTableViewGenre,
                    SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
                    this, SLOT(changeTableViewGenreCurrentSelection(const QModelIndex &, const QModelIndex &)), Qt::UniqueConnection);
    }
}

void MainWindow::initAeristsTableView(QString genre)
{
    selectArtists();
    selectionModelTableViewArtists = ui->tableViewArtist->selectionModel();
    if (selectionModelTableViewArtists != nullptr) {
            connect(selectionModelTableViewArtists,
                    SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
                    this, SLOT(changeTableViewArtistCurrentSelection(const QModelIndex &, const QModelIndex &)), Qt::UniqueConnection);
    }
}

void MainWindow::initPlaylist(QString genre, QString artist)
{

}

void MainWindow::tooglePlayPause()
{
    playManager->togglePlayPause();
}

void MainWindow::play()
{
    playManager->play("/mnt/BIG_GEEK/Music/Demon Days 2005/5. Dirty Harry.mp3");
}

void MainWindow::stop()
{
    playManager->stop();
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
    ui->sliderPlayProgress->setValue(value.progress);
    ui->labelPlayTime->setText(value.playTime);
    ui->sliderVolume->setValue(value.volume);
    //    qDebug() << "Total time: " << value.totalTime << ", time: " << value.progress << ", volume: " << value.vulume;
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
}

void MainWindow::changeTableViewGenreCurrentSelection(const QModelIndex &current, const QModelIndex &previous)
{
    selectArtists(dbModule->getSelectedGenre(current.row()));
}

void MainWindow::changeTableViewArtistCurrentSelection(const QModelIndex &current, const QModelIndex &previous)
{

}

