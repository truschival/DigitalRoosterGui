#include <qqmlengine.h>
#include <qqmlcontext.h>
#include <qqml.h>
#include <QtQuick/qquickitem.h>
#include <QtQuick/qquickview.h>

#include <QQmlApplicationEngine>
#include <QGuiApplication>
#include <QDebug>
#include <QLoggingCategory>

#include "config.h"
#include "configuration_manager.hpp"
#include "podcastsourcemodel.hpp"

using namespace DigitalRooster;

int main(int argc, char* argv[]) {
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  //  QLoggingCategory::setFilterRules("*.debug=true");
    QGuiApplication app(argc, argv);

    /*Get avaibable Podcasts */
    ConfigurationManager cm(DigitalRooster::SYSTEM_CONFIG_PATH);
    /* and let the model access the config */
   //PodcastSourceModel psmodel(&cm);

    QQmlApplicationEngine  view(QUrl("qrc:/main.qml"));
    QQmlContext* ctxt = view.rootContext();
    //ctxt->setContextProperty("podcastmodel", &psmodel);

    return app.exec();
}
