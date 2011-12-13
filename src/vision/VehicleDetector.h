#ifndef VEHICLEDETECTOR_H
#define VEHICLEDETECTOR_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QThread>

class VehicleDetector : public QThread
{
    Q_OBJECT

public:
    VehicleDetector( QObject *parent );
    ~VehicleDetector();

    bool shouldContinue;

signals:
    void gotFrame( cv::Mat &frame, int type );
    void gotLeadVehiclePresent( bool present, bool aligned );

protected:
    void run();

private slots:
    void dummyLoop();

};

#endif

