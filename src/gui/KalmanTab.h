#ifndef KALMANTAB_H
#define KALMANTAB_H

#include <QDialog>
#include <QLabel>
#include <QtGui>

class QLabel;

class KalmanTab : public QWidget
{
    Q_OBJECT

public:
    KalmanTab( QWidget *parent );
    ~KalmanTab();

    void setCorrectedErrorEstimateMinusValue( float value ) ;
    void setCorrectedStateEstimateValue( float value ) ;
    void setCorrectedErrorEstimateValue( float value ) ;
    void setKalmanGainValue( float value ) ;
    void setTimeHeadwayAccumulatorValue( float value ) ;
    void setTimeDistanceAccumulatorValue( float value ) ;
    void setLeadVelocityAccumulatorValue( float value ) ;
    void setLeadAccelerationAccumulatorValue( float value ) ;
    void setFollowingVelocityAccumulatorValue( float value ) ;
    void setFollowingAccelerationAccumulatorValue( float value ) ;


private:
    QLabel *xkmLabel;
    QLabel *pkmLabel;
    QLabel *xkLabel;
    QLabel *pkLabel;
    QLabel *kkLabel;
    QLabel *transitionMatrixLabel00;
    QLabel *transitionMatrixLabel01;
    QLabel *transitionMatrixLabel02;
    QLabel *transitionMatrixLabel03;
    QLabel *transitionMatrixLabel10;
    QLabel *transitionMatrixLabel11;
    QLabel *transitionMatrixLabel12;
    QLabel *transitionMatrixLabel13;
    QLabel *transitionMatrixLabel20;
    QLabel *transitionMatrixLabel21;
    QLabel *transitionMatrixLabel22;
    QLabel *transitionMatrixLabel23;
    QLabel *transitionMatrixLabel30;
    QLabel *transitionMatrixLabel31;
    QLabel *transitionMatrixLabel32;
    QLabel *transitionMatrixLabel33;
    QLabel *errorCovPostLabel00;
    QLabel *errorCovPostLabel01;
    QLabel *errorCovPostLabel02;
    QLabel *errorCovPostLabel03;
    QLabel *errorCovPostLabel10;
    QLabel *errorCovPostLabel11;
    QLabel *errorCovPostLabel12;
    QLabel *errorCovPostLabel13;
    QLabel *errorCovPostLabel20;
    QLabel *errorCovPostLabel21;
    QLabel *errorCovPostLabel22;
    QLabel *errorCovPostLabel23;
    QLabel *errorCovPostLabel30;
    QLabel *errorCovPostLabel31;
    QLabel *errorCovPostLabel32;
    QLabel *errorCovPostLabel33;
    QLabel *statePostLabel0;
    QLabel *statePostLabel1;
    QLabel *statePostLabel2;
    QLabel *statePostLabel3;

    QLabel *timeHeadwayAccumulatorLabel;
    QLabel *timeDistanceAccumulatorLabel;
    QLabel *leadVelocityAccumulatorLabel;
    QLabel *leadAccelerationAccumulatorLabel;
    QLabel *followingVelocityAccumulatorLabel;
    QLabel *followingAccelerationAccumulatorLabel;

};
#endif
