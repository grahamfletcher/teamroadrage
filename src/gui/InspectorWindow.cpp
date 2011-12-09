#include <QDebug>
#include <QHBoxLayout>
#include <QString>
#include <QTabWidget>
#include <QVBoxLayout>

#include "InspectorWindow.h"

InspectorWindow::InspectorWindow( QWidget *parent = 0 ) : QDialog( parent ) {
    QTabWidget *tabWidget = new QTabWidget( this );

    headwayTab = new HeadwayTab( this );
    sensorsTab = new SensorsTab( this );

    tabWidget->addTab( headwayTab, tr( "Headway" ) );
    tabWidget->addTab( sensorsTab, tr( "Sensors" ) );

    QLabel *timeHeadwayBuddy = new QLabel( tr( "Time headway:" ) );
    timeHeadwayLabel = new QLabel( tr( "-" ) );

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget( timeHeadwayBuddy );
    hLayout->addWidget( timeHeadwayLabel );

    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addWidget( tabWidget );
    vLayout->addLayout( hLayout );

    setLayout( vLayout );

    setWindowTitle( tr( "Inspector" ) );
}

InspectorWindow::~InspectorWindow() {

}

void InspectorWindow::setSafeTimeHeadway( float time ) {
    headwayTab->setSafeTimeHeadway( time );
}

void InspectorWindow::setIcePresent( bool ice ) {
    headwayTab->setIcePresent( ice );
}

void InspectorWindow::setRainPresent( bool rain ) {
    headwayTab->setRainPresent( rain );
}

void InspectorWindow::setReactionTime( float time ) {
    headwayTab->setReactionTime( time );
}

void InspectorWindow::setRawDistance( float distance ) {
    sensorsTab->setRawDistance( distance );
}

void InspectorWindow::setRawFollowingVehicleVelocity( float velocity ) {
    sensorsTab->setRawFollowingVehicleVelocity( velocity );
}

void InspectorWindow::setHumidity( float humidity ) {
    sensorsTab->setHumidity( humidity );
}

void InspectorWindow::setRainSensorVoltage( float voltage ) {
    sensorsTab->setRainSensorVoltage( voltage );
}

void InspectorWindow::setTemperature( float temperature ) {
    sensorsTab->setTemperature( temperature );
}

void InspectorWindow::setTimeHeadway( float time ) {
    QString s;
    s.sprintf( "%.2f m/s", time );

    timeHeadwayLabel->setText( s );
}
