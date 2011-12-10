#ifndef VEHICLEDETECTOR_H
#define VEHICLEDETECTOR_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QObject>

class VehicleDetector : public QObject
{
    Q_OBJECT

public:
    VehicleDetector( QObject *parent );
    ~VehicleDetector();

signals:
    void gotFrame( cv::Mat &frame, int type );
    void gotLeadVehiclePresent( bool present, bool aligned );

};

#endif

