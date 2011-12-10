#ifndef CAMERADEVICE_H
#define CAMERADEVICE_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QMutex>
#include <QObject>

class CameraDevice : public QObject
{
    Q_OBJECT

public:
    CameraDevice( QObject *parent );
    ~CameraDevice();

    void getFrame( cv::Mat &destination );

private:
    QMutex captureMutex;

    cv::VideoCapture *capture;

    cv::Rect_<int> crop;
};
#endif
