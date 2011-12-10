#ifndef CAPTURE_H
#define CAPTURE_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QMutex>
#include <QObject>
#include <QSettings>
#include <QThread>
#include <QWaitCondition>

#include "globals.h"

class Capture : public QThread
{
    Q_OBJECT

public:
    Capture( QObject *parent );
    ~Capture();
    void getCurrentFrame( cv::Mat &destination );
    cv::Rect_<int> getCrop();
    cv::Size getDimensions();
    int getFourCC();
    int getFPS();
    QString getOutputVideoPath();

    //QWaitCondition newFrameAvailable;

signals:
    void gotNewFrame( _Mat mat, int type );

protected:
    void run();

private:
    bool getNextFrame();

    /* Video file paths */
    QString inputVideoPath;
    QString outputVideoPath;

    /* Capture and associated properties */
    cv::VideoCapture capture;
    cv::Rect_<int>   captureCrop;
    cv::Size         captureDimensions;
    int              captureFourCC;
    int              captureFPS;

    QMutex          currentFrameMutex;
    cv::Mat         currentFrame;
};

#endif
