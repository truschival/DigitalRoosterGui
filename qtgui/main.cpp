#include <config.h>

#include <QApplication>
#include <QTime>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QFileSystemWatcher>
#include <QSettings>
#include <QFileSystemWatcher>


#include "confmon.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

    auto player = new QMediaPlayer;
    player->setMedia(QUrl::fromLocalFile("/usr/share/sounds/Oxygen-Im-Connection-Lost.ogg"));
    player->setVolume(50);
    player->play();

    auto syswatch = new QFileSystemWatcher;
    syswatch->addPath("/tmp/foo");
    
    QMainWindow::connect(syswatch,SIGNAL(fileChanged(const QString &)),player,SLOT(setMedia(qint64)));
    
    return app.exec();
}
