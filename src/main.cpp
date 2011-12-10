#include <signal.h>
#include <QApplication>
#include <QDebug>
#include <QObject>
#include <QSettings>

#include "Controller.h"

#include "globals.h"

//Q_DECLARE_METATYPE( _Mat )
//Q_DECLARE_METATYPE( _QReadWriteLock );

QSettings *settings;

void updateSettings() {
    //settings->setValue( "Capture/InputVideoPath", "~/Desktop/in.mkv" );
    settings->setValue( "Capture/FrameWidth", "640" );
    //settings->setValue( "Capture/FrameHeight", "480" );
    settings->setValue( "Capture/FrameHeight", "472" );
    settings->setValue( "Capture/CropX", "0" );
    settings->setValue( "Capture/CropY", "33" );
    settings->setValue( "Capture/CropWidth", "640" );
    settings->setValue( "Capture/CropHeight", "360" );
}

int main( int argc, char **argv ) {
    QApplication app( argc, argv );
    QApplication::setApplicationName("Collision Detector");
    QApplication::setOrganizationName("Team Road Rage");
    QApplication::setOrganizationDomain("gatech.edu");

    settings = new QSettings( &app );
    updateSettings();

    signal( SIGABRT, &app.exit );
    signal( SIGQUIT, &app.exit );

    qRegisterMetaType<_Mat>( "_Mat" );
    qRegisterMetaType<_QReadWriteLock>( "_QReadWriteLock" );

    Controller controller( &app );

    app.exec();
}
