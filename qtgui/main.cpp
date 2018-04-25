
#define QT_QML_DEBUG

#include <QtQuick>
#include <QDebug>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QLoggingCategory>
#include <QQmlApplicationEngine>

#include "configuration_manager.hpp"
#include "podcastepisodemodel.hpp"
#include "podcastsourcemodel.hpp"
#include <mediaplayerproxy.hpp>
#include "alarm.hpp"

using namespace DigitalRooster;

int main(int argc, char* argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    // QLoggingCategory::setFilterRules("*.debug=true");
	qDebug() << "SSL Support: " << QSslSocket::supportsSsl() << QSslSocket::sslLibraryVersionString();

    QGuiApplication app(argc, argv);
    qmlRegisterType<PodcastEpisodeModel>(
        "ruschi.PodcastEpisodeModel", 1, 0, "PodcastEpisodeModel");
	qmlRegisterType<PodcastEpisode>("ruschi.PodcastEpisode", 1, 0, "PodcastEpisode");
	qmlRegisterType<Alarm>("ruschi.Alarm", 1, 0, "Alarm");
	qmlRegisterType<DigitalRooster::MediaPlayerProxy>("ruschi.MediaPlayerProxy", 1, 0, "MediaPlayerProxy");

    /*Get avaibable Podcasts */
	ConfigurationManager cm(DigitalRooster::SYSTEM_CONFIG_PATH);
    /* and let the model access the config */
    PodcastSourceModel psmodel(&cm);
    MediaPlayerProxy playerproxy;

    QQmlApplicationEngine view;
    QQmlContext* ctxt = view.rootContext();
    ctxt->setContextProperty("podcastmodel", &psmodel);
    ctxt->setContextProperty("playerProxy", &playerproxy);
	view.load(QUrl("qrc:/main.qml"));

    return app.exec();
}
