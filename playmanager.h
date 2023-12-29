#ifndef PLAYMANAGER_H
#define PLAYMANAGER_H

#include <QTimer>
#include <QDebug>
#include <QtCore>
#include <QProcess>

#include "mocp/client.h"
#include "mocp/options.h"

struct PlayData
{
    int totalTime = -1;
    int progress = -1;
    int volume = -1;
    QString playTime = "";
};

Q_DECLARE_METATYPE( PlayData );

class PlayManager : public QObject
{
    Q_OBJECT

    signals:
        void serverIsRunning(bool value);
        void playDataChanged(PlayData value);
        void songIsEnd();

    private slots:
        void dataChanged();
        void slotDataOnStdout();

    public slots:
        void doWork();

    public:
        PlayManager(QObject *object = 0);
        ~PlayManager();

        void connectToServer();
        bool pingServer();
        void disconnectFromServer();

        void togglePlayPause();
        void play(char *song);
        void stop(bool byUser=false);
        void goToTime(int time);

        void getCurrentFileInformation();

        int getVolume();
        void setVolume(int vol);
        void stopWatch();
        bool startServer();
        bool stopServer();
//        int getCurrTime ();

    private:
        int socket = -1;
        QTimer *timer;
        PlayData playData;
        QProcess *process = nullptr;
        bool stopByUser = true;

};

#endif // PLAYMANAGER_H
