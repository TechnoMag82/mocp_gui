#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->sliderPlayProgress->setMinimum(0);
    ui->sliderVolume->setMinimum(0);
    ui->sliderVolume->setMaximum(100);
    dbModule = new DbModule();
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
}

MainWindow::~MainWindow()
{
    playManager->stopWatch();
    thread.quit();
    thread.wait();
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

