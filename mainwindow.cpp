#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // TODO: add start mocp server code
    dbModule = new DbModule();
    playManager = new PlayManager();
    playManager->connectToServer();
    connect(playManager, SIGNAL(serverIsRunning(bool)), this, SLOT(serverOnLine(bool)));
    if (playManager->pingServer() == true) {
        playManager->getCurrentFileInformation();
    }
    connect(ui->btnPlayPause, SIGNAL(clicked()), this, SLOT(tooglePlayPause()));
    connect(ui->btnPlay, SIGNAL(clicked()), this, SLOT(play()));
    connect(ui->btnStop, SIGNAL(clicked()), this, SLOT(stop()));
}

MainWindow::~MainWindow()
{
    playManager->disconnectFromServer();
    delete playManager;
    delete dbModule;
    delete ui;
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
    ui->labelErrorMessage->setText("Server is offline");
    ui->labelErrorMessage->setVisible(!value);
    ui->btnPlay->setEnabled(value);
    ui->btnPlayPause->setEnabled(value);
    ui->btnStop->setEnabled(value);
    ui->sliderPlayProgress->setEnabled(value);
    ui->sliderVolume->setEnabled(value);
}

