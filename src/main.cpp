#include <signal.h>
#include <opencv2/core/core.hpp>
#include <QApplication>
#include <QObject>
#include <QSettings>

#include "Controller.h"
#include "GUIController.h"

QSettings *settings;

int main( int argc, char **argv )
{
    QApplication app( argc, argv );
    QApplication::setApplicationName("Collision Detector");
    QApplication::setOrganizationName("Team Road Rage");
    QApplication::setOrganizationDomain("gatech.edu");

    settings = new QSettings( &app );

    signal( SIGABRT, &app.exit );
    signal( SIGQUIT, &app.exit );

    qRegisterMetaType<cv::Mat>( "cv::Mat" );

    GUIController guiController( &app );
    Controller controller( &app, &guiController );

    app.exec();
}
