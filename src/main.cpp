#include <signal.h>
//#include <opencv2/core/core.hpp>
#include <QApplication>
#include <QObject>
#include <QSettings>

#include "Controller.h"
#include "GUIController.h"

Q_DECLARE_METATYPE( cv::Mat )
//Q_DECLARE_METATYPE( cv::Mat& )

QSettings *settings;

int main( int argc, char **argv )
{
    QApplication app( argc, argv );
    QApplication::setApplicationName("Collision Detector");
    QApplication::setOrganizationName("Team Road Rage");
    QApplication::setOrganizationDomain("gatech.edu");

    settings = new QSettings( &app );
    //settings->setValue( "Capture/InputVideoPath", "~/Desktop/in.mkv" );
    settings->setValue( "Capture/FrameWidth", "640" );
    //settings->setValue( "Capture/FrameHeight", "480" );
    settings->setValue( "Capture/FrameHeight", "472" );
    settings->setValue( "Capture/CropX", "0" );
    settings->setValue( "Capture/CropY", "33" );
    settings->setValue( "Capture/CropWidth", "640" );
    settings->setValue( "Capture/CropHeight", "360" );

    signal( SIGABRT, &app.exit );
    signal( SIGQUIT, &app.exit );

    GUIController guiController( &app );
    Controller controller( &app, &guiController );

    app.exec();
}
