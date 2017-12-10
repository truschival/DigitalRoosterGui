#include <config.h>

#include <QDebug>

#include <QApplication>
#include <QTime>
#include <QMainWindow>
#include <QFileSystemWatcher>
#include <QSettings>
#include <QFileSystemWatcher>


#include <thread>
#include <chrono>

#include "mediaplayer_wrapper.hpp"
#include "confmon.hpp"

using namespace std::chrono_literals;


int main(int argc, char *argv[])
{
	qDebug() << __FUNCTION__;
    QApplication app(argc, argv);
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

    MediaPlayerWrapper player;
    player.play(QUrl::fromLocalFile("/data/Music/ACDC/Black Ice/15 - Black Ice.mp3"));
    player.getMedia();


    return app.exec();
}
