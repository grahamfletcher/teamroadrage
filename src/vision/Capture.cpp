#include <QDebug>

#include "globals.h"
#include "Capture.h"

Capture::Capture( QObject *parent ) : QThread( parent ) {
    inputVideoPath = settings->value( "VideoCapture/InputVideoPath" ).toString();
    outputVideoPath = settings->value( "VideoCapture/OutputVideoPath" ).toString();

    capture = cv::VideoCapture( inputVideoPath.toLocal8Bit().data() );

    if ( !capture.isOpened() ) {
        qDebug() << "Could not open video file: " << inputVideoPath;
    }

    captureCrop = cv::Rect_<int>( 0,
                                  settings->value( "VideoCapture/CropTop" ).toInt(),
                                  (int) capture.get( CV_CAP_PROP_FRAME_WIDTH ),
                                  (int) capture.get( CV_CAP_PROP_FRAME_HEIGHT ) -
                                  settings->value( "VideoCapture/CropBottom" ).toInt() -
                                  settings->value( "VideoCapture/CropTop" ).toInt() );

    captureDimensions = cv::Size( captureCrop.width, captureCrop.height );
    captureFourCC = (int) capture.get( CV_CAP_PROP_FOURCC );
    captureFPS = (int) capture.get( CV_CAP_PROP_FPS );


    start();
}

Capture::~Capture() {

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
        currentFrame = currentFrame( cv::Rect( 0, 50, 960, 540 ) );
        emit newFrameAvailable( currentFrame );
    }
    currentFrameMutex.unlock();

    return returnValue;
}

void Capture::run() {
    while( getNextFrame() ) {
        usleep(10000000 / captureFPS);
        //sleep( 1 );
    }
}
