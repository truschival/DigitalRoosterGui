
#include <QDebug>
#include <QLoggingCategory>

#include <QFileSystemWatcher>
#include <QSettings>
#include <QTime>

#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <chrono>
#include <thread>



using namespace std::chrono_literals;


int main(int argc, char* argv[]) {
    QLoggingCategory::setFilterRules("*.debug=true");
    qDebug() << __FUNCTION__;

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;


    return app.exec();
}
