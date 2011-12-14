#include <QDebug>
#include <QMutexLocker>

#include "globals.h"
#include "CameraDevice.h"

CameraDevice::CameraDevice( QObject *parent ) : QObject( parent ) {
    /* Create the video capture */
    capture = new cv::VideoCapture( 0 );

    if ( !capture->isOpened() ) {
        qDebug() << "Could not open camera";
    } else {
        qDebug() << "Camera opened";
    }

    capture->set( CV_CAP_PROP_FRAME_WIDTH, settings->value( "Capture/FrameWidth" ).toInt() );
    capture->set( CV_CAP_PROP_FRAME_HEIGHT, settings->value( "Capture/FrameHeight" ).toInt() );

    crop = cv::Rect_<int>( settings->value( "Capture/CropX" ).toInt(),
                           settings->value( "Capture/CropY" ).toInt(),
                           settings->value( "Capture/CropWidth" ).toInt(),
                           settings->value( "Capture/CropHeight" ).toInt() );
	fps = settings->value("Capture/FPS").toInt();
}

CameraDevice::~CameraDevice() {
    QMutexLocker locker( &captureMutex );    // for some reason this feels necessary

    capture->release();

    qDebug() << "~CameraDevice()";
}

void CameraDevice::getFrame( cv::Mat &destination ) {
    QMutexLocker locker( &captureMutex );

    while ( !capture->read( destination ) ) {
        usleep( 1000000 / fps );    // wait 10 milliseconds
    }

    destination = destination( crop );
}

