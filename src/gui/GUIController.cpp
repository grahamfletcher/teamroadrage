#include <QDebug>

#include "GUIController.h"

GUIController::GUIController( QObject *parent ) : QObject( parent ) {
    inspectorWindow = new InspectorWindow( NULL );
    videoWindow = new VideoWindow( NULL );

    inspectorWindow->show();
    videoWindow->show();
}

GUIController::~GUIController() {
    qDebug() << "~GUIController()";
}

void GUIController::updateSafeTimeHeadway( float time ) {
    inspectorWindow->setSafeTimeHeadway( time );
}

void GUIController::updateIcePresent( bool ice ) {
    inspectorWindow->setIcePresent( ice );
}

void GUIController::updateRainPresent( bool rain ) {
    inspectorWindow->setRainPresent( rain );
}

void GUIController::updateRawDistance( float distance ) {
    inspectorWindow->setRawDistance( distance );
}

void GUIController::updateHumidity( float humidity ) {
    inspectorWindow->setHumidity( humidity );
}

void GUIController::updateRainSensorVoltage( float voltage ) {
    inspectorWindow->setRainSensorVoltage( voltage );
}

void GUIController::updateRawFollowingVehicleVelocity( float velocity ) {
    inspectorWindow->setRawFollowingVehicleVelocity( velocity );
}

void GUIController::updateTemperature( float temperature ) {
    inspectorWindow->setTemperature( temperature );
}

void GUIController::updateTimeHeadway( float time ) {
    inspectorWindow->setTimeHeadway( time );
}

void GUIController::updateDistanceHeadway( float distance ) {
    //qDebug() << "Distance headway: " << distance;
}

void GUIController::updateLeadVehicleVelocity( float velocity ) {
    //qDebug() << "Lead vehicle velocity: " << velocity;
}

void GUIController::updateFollowingVehicleVelocity( float velocity ) {
    //qDebug() << "Following vehicle velocity: " << velocity;
}

void GUIController::updateFollowingVehicleAcceleration( float acceleration ) {
    //qDebug() << "Following vehicle acceleration: " << acceleration;
}

void GUIController::updateReactionTime( float time ) {
    inspectorWindow->setReactionTime( time );
}

void GUIController::updateCurrentFrame( cv::Mat *frame, int type, QReadWriteLock *lock ) {
    videoWindow->updateVideoFrame( frame, type, lock );
}
