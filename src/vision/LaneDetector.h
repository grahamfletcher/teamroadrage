#ifndef LANEDETECTOR_H
#define LANEDETECTOR_H

#include <QReadWriteLock>
#include <QThread>

#include "globals.h"
#include "CameraDevice.h"

class LaneDetector : public QThread
{
    Q_OBJECT

public:
    LaneDetector( QObject *parent, CameraDevice *cameraDevice );
    ~LaneDetector();

    bool shouldContinue;

signals:
    void gotFrame( _Mat frame, int type, _QReadWriteLock lock );

protected:
    void run();

private slots:
    void getFrameFromCamera();
    void convertToGrayscale();

private:
    CameraDevice *cameraDevice;

    QReadWriteLock originalFrameLock;
    cv::Mat originalFrame;
    
    QReadWriteLock grayscaleFrameLock;
    cv::Mat grayscaleFrame;

    QReadWriteLock ipmFrameLock;
    cv::Mat ipmFrame;

};

#endif

