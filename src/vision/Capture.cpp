#include <QDebug>

#include "globals.h"
#include "Capture.h"

Capture::Capture( QObject *parent ) : QThread( parent ) {
    //inputVideoPath = settings->value( "VideoCapture/InputVideoPath" ).toString();
    //outputVideoPath = settings->value( "VideoCapture/OutputVideoPath" ).toString();

    capture = cv::VideoCapture( 0 );//cv::VideoCapture( inputVideoPath.toLocal8Bit().data() );

    if ( !capture.isOpened() ) {
        qDebug() << "Could not open camera";// video file: " << inputVideoPath;
    }

    capture.set( CV_CAP_PROP_FRAME_WIDTH, settings->value( "Capture/FrameWidth" ).toInt() );
    capture.set( CV_CAP_PROP_FRAME_HEIGHT, settings->value( "Capture/FrameHeight" ).toInt() );

    qDebug() << capture.get( CV_CAP_PROP_FRAME_WIDTH );
    qDebug() << capture.get( CV_CAP_PROP_FRAME_HEIGHT );

    captureCrop = cv::Rect_<int>( settings->value( "Capture/CropX" ).toInt(),
                                  settings->value( "Capture/CropY" ).toInt(),
                                  settings->value( "Capture/CropWidth" ).toInt(),
                                  settings->value( "Capture/CropHeight" ).toInt() );
    
    captureDimensions = cv::Size( captureCrop.width, captureCrop.height );

    captureFourCC = (int) capture.get( CV_CAP_PROP_FOURCC );
    captureFPS = (int) capture.get( CV_CAP_PROP_FPS );

    qDebug() << captureFPS;

    captureFPS = 15;

    //moveToThread( this );

    start();
}

Capture::~Capture() {
    capture.release();
}

void Capture::getCurrentFrame( cv::Mat &destination ) {
    currentFrameMutex.lock();
    destination = currentFrame.clone();
    currentFrameMutex.unlock();
}

cv::Size Capture::getDimensions() {
    return captureDimensions;
}

int Capture::getFourCC() {
    return captureFourCC;
}

int Capture::getFPS() {
    return captureFPS;
}

QString Capture::getOutputVideoPath() {
    return outputVideoPath;
}

bool Capture::getNextFrame() {
    bool returnValue = true;

    currentFrameMutex.lock();
    if ( !capture.read( currentFrame ) ) {
        returnValue = false;
    } else {
        currentFrame = currentFrame( captureCrop );
        emit gotNewFrame( &currentFrame, ORIGINAL );
    }
    currentFrameMutex.unlock();

    return returnValue;
}

void Capture::run() {
    while( getNextFrame() ) {
        usleep(1000000 / captureFPS);
        //sleep( 1 );
    }
    qDebug() << "End of Capture.run()";
}
