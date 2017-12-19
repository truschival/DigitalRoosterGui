#include <config.h>

#include <QLoggingCategory>
#include <QDebug>

#include <QApplication>
#include <QTime>
#include <QMainWindow>
#include <QFileSystemWatcher>
#include <QSettings>
#include <QFileSystemWatcher>


#include <thread>
#include <chrono>


#include "confmon.hpp"

using namespace std::chrono_literals;


int main(int argc, char *argv[])
{
	QLoggingCategory::setFilterRules("*.debug=true");
	qDebug() << __FUNCTION__;
    QApplication app(argc, argv);
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));


    return app.exec();
}
