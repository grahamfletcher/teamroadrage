#include <QCoreApplication>
#include <QDebug>
#include <QReadLocker>
#include <QTimer>

#include "LaneDetector.h"

LaneDetector::LaneDetector( QObject *parent, CameraDevice *cameraDevice ) : QThread( parent ), cameraDevice( cameraDevice ) {
    shouldContinue = true;
    moveToThread( this );
}

LaneDetector::~LaneDetector() {
    qDebug() << "~LaneDector()";
    shouldContinue = false;
    wait();
}

void LaneDetector::getFrameFromCamera() {
    if ( !shouldContinue ) {
        exit( 0 );
        return;
    }

    originalFrameLock.lockForWrite();
    cameraDevice->getFrame( originalFrame );
    originalFrameLock.unlock();

    emit gotFrame( &originalFrame, ORIGINAL, &grayscaleFrameLock );

    QTimer::singleShot( 0, this, SLOT( convertToGrayscale() ) );
}

void LaneDetector::convertToGrayscale() {
    if ( !shouldContinue ) {
        exit( 0 );
        return;
    }

    QReadLocker locker( &originalFrameLock );

    grayscaleFrameLock.lockForWrite();
    grayscaleFrame = originalFrame;
    grayscaleFrameLock.unlock();

    emit gotFrame( &grayscaleFrame, GRAYSCALE, &grayscaleFrameLock );

    QTimer::singleShot( 0, this, SLOT( getFrameFromCamera() ) );
}

void LaneDetector::run() {
    QTimer::singleShot( 0, this, SLOT( getFrameFromCamera() ) );

    exec();
}

