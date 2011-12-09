#ifndef SENSORSTAB_H
#define SENSORSTAB_H

#include <QWidget>

class QLabel;

class SensorsTab : public QWidget
{
    Q_OBJECT

public:
    SensorsTab( QWidget *parent );
    ~SensorsTab();

    void setRawDistance( float distance );
    void setRawFollowingVehicleVelocity( float velocity );
    void setHumidity( float humidity );
    void setRainSensorVoltage( float voltage );
    void setTemperature( float temperature );

private:
    QLabel *distanceLabel;
    QLabel *speedLabel;
    QLabel *humidityLabel;
    QLabel *rainSensorVoltageLabel;
    QLabel *temperatureLabel;
};
#endif
