#define QT_QML_DEBUG
#include <QtQuick>

#include <qqml.h>
#include <qqmlcontext.h>
#include <qqmlengine.h>

#include <QDebug>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QLoggingCategory>
#include <QQmlApplicationEngine>

#include "config.h"
#include "configuration_manager.hpp"
#include "podcastepisodemodel.hpp"
#include "podcastsourcemodel.hpp"

using namespace DigitalRooster;

int main(int argc, char* argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    // QLoggingCategory::setFilterRules("*.debug=true");
    QFontDatabase::addApplicationFont(":/materialdesignicons-webfont.ttf");

    QGuiApplication app(argc, argv);
    qmlRegisterType<PodcastEpisodeModel>(
        "ruschi.PodcastEpisodeModel", 1, 0, "PodcastEpisodeModel");
	qmlRegisterType<PodcastEpisode>("ruschi.PodcastEpisode", 1, 0, "PodcastEpisode");

    /*Get avaibable Podcasts */
	QFile file(DigitalRooster::SYSTEM_CONFIG_PATH);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << file.errorString();
		throw std::system_error(
			make_error_code(std::errc::no_such_file_or_directory), "Cannot read file");
	}

	 ConfigurationManager cm(DigitalRooster::SYSTEM_CONFIG_PATH);
    /* and let the model access the config */
    PodcastSourceModel psmodel(&cm);

    QQmlApplicationEngine view(QUrl("qrc:/main.qml"));
    QQmlContext* ctxt = view.rootContext();
    ctxt->setContextProperty("podcastmodel", &psmodel);

    return app.exec();
}
