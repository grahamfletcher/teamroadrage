#ifndef LANEDETECTOR_H
#define LANEDETECTOR_H

#include <opencv2/core/core.hpp>
#include <QObject>

class LaneDetector : public QObject
{
    Q_OBJECT

public:
    LaneDetector( QObject *parent );
    ~LaneDetector();

signals:
    void gotFrame( cv::Mat &frame, int type );
};

#endif

