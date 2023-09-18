#ifndef PLAYMANAGER_H
#define PLAYMANAGER_H

#include <QDebug>

#include "mocp/client.h"
#include "mocp/options.h"

class PlayManager : public QObject
{
    Q_OBJECT

    signals:
        void serverIsRunning(bool value);
    public:
        PlayManager();
        ~PlayManager();

        void connectToServer();
        bool pingServer();
        void disconnectFromServer();

        void togglePlayPause();
        void play(char *song);
        void stop();

        void getCurrentFileInformation();

        int getVolume();
        void setVolume(int vol);

    private:
        int socket;

};

#endif // PLAYMANAGER_H
