#ifndef INSPECTORWINDOW_H
#define INSPECTORWINDOW_H

#include <QDialog>
#include <QLabel>

#include "HeadwayTab.h"
#include "SensorsTab.h"

class InspectorWindow : public QDialog
{
    Q_OBJECT

public:
    InspectorWindow( QWidget *parent );
    ~InspectorWindow();
    
    void setSafeTimeHeadway( float time );
    void setIcePresent( bool ice );
    void setRainPresent( bool rain );
    void setReactionTime( float time );

    void setRawDistance( float distance );
    void setRawFollowingVehicleVelocity( float velocity );
    void setHumidity( float humidity );
    void setRainSensorVoltage( float voltage );
    void setTemperature( float temperature );

    void setTimeHeadway( float time );

private:
    HeadwayTab *headwayTab;
    SensorsTab *sensorsTab;
    QLabel *timeHeadwayLabel;
};
#endif
