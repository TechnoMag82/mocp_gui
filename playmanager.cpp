#include "playmanager.h"

PlayManager::PlayManager()
{

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
