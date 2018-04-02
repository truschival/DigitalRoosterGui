
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

using namespace DigitalRooster;

int main(int argc, char* argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    // QLoggingCategory::setFilterRules("*.debug=true");

    QGuiApplication app(argc, argv);
    qmlRegisterType<PodcastEpisodeModel>(
        "ruschi.PodcastEpisodeModel", 1, 0, "PodcastEpisodeModel");
	qmlRegisterType<PodcastEpisode>("ruschi.PodcastEpisode", 1, 0, "PodcastEpisode");

    /*Get avaibable Podcasts */
	ConfigurationManager cm(DigitalRooster::SYSTEM_CONFIG_PATH);
    /* and let the model access the config */
    PodcastSourceModel psmodel(&cm);

    QQmlApplicationEngine view(QUrl("qrc:/main.qml"));
    QQmlContext* ctxt = view.rootContext();
    ctxt->setContextProperty("podcastmodel", &psmodel);

    return app.exec();
}
