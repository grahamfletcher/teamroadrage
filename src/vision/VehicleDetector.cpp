#include <QDebug>
#include <QTimer>

#include "VehicleDetector.h"

VehicleDetector::VehicleDetector( QObject *parent ) : QThread( parent ) {
    shouldContinue = true;
}

VehicleDetector::~VehicleDetector() {
    qDebug() << "~VehicleDetector()";
    shouldContinue = false;
    wait();
}

void VehicleDetector::dummyLoop() {
    while ( shouldContinue ) {
        emit gotLeadVehiclePresent( true, true );

        sleep( 1 );
    }

    qDebug() << "exit VehicleDetector";
    exit( 0 );
}

void VehicleDetector::run() {
    QTimer::singleShot( 1000, this, SLOT( dummyLoop() ) );

    exec();
}
