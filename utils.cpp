#include <QString>
#include <QTime>

static QString timeToString(uint milli) {
    uint hr = milli / 3600000;
    milli = milli - 3600000 * hr;
    //60000 milliseconds in a minute
    uint min = milli / 60000;
    milli = milli - 60000 * min;
    //1000 milliseconds in a second
    uint sec = milli / 1000;
    QTime timestamp(hr, min, sec);
    return timestamp.toString("hh:mm:ss");
}
