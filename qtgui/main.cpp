#include "graphwidget.h"

#include <QApplication>
#include <QTime>
#include <QMainWindow>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

    GraphWidget *widget = new GraphWidget;

    QMainWindow mainWindow;
    mainWindow.setCentralWidget(widget);

    mainWindow.show();
    return app.exec();
}
