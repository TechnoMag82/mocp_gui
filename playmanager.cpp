#include "playmanager.h"

PlayManager::PlayManager(QObject *object) : QObject(object)
{
    qRegisterMetaType<PlayData>("PlayData");
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), SLOT(dataChanged()));
//    timer->start(1000);
}

PlayManager::~PlayManager()
{
}

void PlayManager::connectToServer()
{
    options_init();
    socket = server_connect();
}

bool PlayManager::pingServer()
{
    if (socket == -1) {
        qDebug() << "ping: no socket";
        emit serverIsRunning(false);
        return false;
    }
    if (ping_server(socket) == 1) {
        qDebug() << "ping ok: socket: " << socket;
        emit serverIsRunning(true);
        return true;
    } else {
        qDebug() << "ping no: socket" << socket ;
        emit serverIsRunning(false);
        return false;
    }
}

void PlayManager::disconnectFromServer()
{
    options_free();
    server_disconnect(socket);
    close_socket(socket);
    socket = -1;
}

void PlayManager::togglePlayPause()
{
    if (socket != -1 /*&& getFileState() != 0x02*/ /*STATE_STOP*/) {
        switchPause(socket);
    }
}

void PlayManager::play(char *song)
{
    if (socket != -1 && getFileState() == 0x02 /*STATE_STOP*/) {
        interface_cmdline_playit(socket,  song);
    }
}

void PlayManager::stop()
{
    if (socket != -1) {
        stopPlay(socket);
    }
}

void PlayManager::getCurrentFileInformation()
{
    getCurrentFileInfo(socket);
}

int PlayManager::getVolume()
{
    return get_mixer(socket);
}

void PlayManager::setVolume(int vol)
{
    set_mixer(socket, vol);
}

void PlayManager::stopWatch()
{
    timer->stop();
}

bool PlayManager::startServer()
{
    if (process == nullptr) {
        process = new QProcess(this);
    }
    connect(process, SIGNAL(readyReadStandardError()), this, SLOT(slotDataOnStdout()));
    process->start("mocp -S");
    return process->waitForReadyRead();
}

bool PlayManager::stopServer()
{
    if (process == nullptr) {
        process = new QProcess(this);
    }
    process->start("mocp -x");
    return process->waitForReadyRead();
}

//int PlayManager::getCurrTime ()
//{
//    return getCurrentTime(socket);
//}

void PlayManager::dataChanged()
{
//    if (pingServer()) {
        playData.progress = -1;
        playData.totalTime = -1;
        playData.volume = -1;
        getCurrentFileInformation();
        playData.progress = getCurrentTime(socket);
        playData.volume = getVolume();
        playData.totalTime = getTime();
        playData.playTime = QDateTime::fromTime_t(playData.progress).toUTC().toString("hh:mm:ss") +
                " / " + QDateTime::fromTime_t(playData.totalTime).toUTC().toString("hh:mm:ss");
        emit playDataChanged(playData);
//    }
}

void PlayManager::slotDataOnStdout()
{
    qDebug() << process->readAllStandardError();
}

void PlayManager::doWork()
{
    timer->start(1000);
}
