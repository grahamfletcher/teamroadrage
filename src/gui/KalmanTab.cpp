#include <QDebug>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QString>
#include <QSpacerItem>
#include <QVBoxLayout>

#include "KalmanTab.h"

KalmanTab::KalmanTab( QWidget *parent = 0 ) : QWidget( parent ) {


	/* "Time Update Equations" Label */
    QLabel *timeUpdateBuddy = new QLabel( tr( "Time Update Equations" ) );
	
    /* xkm Equation Label */
    xkmLabel = new QLabel( "-" );

    /* pkm Equation Label */
    pkmLabel = new QLabel( "-" );

	/* xk Equation Label */
    xkLabel = new QLabel( "-" );
	
	/* pk Equation Label */
    pkLabel = new QLabel( "-" );
	
	 /* kk  Label */
    kkLabel = new QLabel( "-" );
	
    /* "Measurement Update Equations" Label */
    QLabel *measurementUpdateBuddy = new QLabel( tr( "Measurement Update Equations" ) );

    /* Space */
    QLabel *space = new QLabel( tr( "  " ) );
    QLabel *space1 = new QLabel( tr( "  " ) );

    QLabel *matrixABuddy = new QLabel( tr( "A =" ) );
    /* Matrix A's element labels */
    transitionMatrixLabel00 = new QLabel( "-" );
    transitionMatrixLabel01 = new QLabel( "-" );
    transitionMatrixLabel02 = new QLabel( "-" );
    transitionMatrixLabel03 = new QLabel( "-" );
    transitionMatrixLabel10 = new QLabel( "-" );
    transitionMatrixLabel11 = new QLabel( "-" );
    transitionMatrixLabel12 = new QLabel( "-" );
    transitionMatrixLabel13 = new QLabel( "-" );
    transitionMatrixLabel20 = new QLabel( "-" );
    transitionMatrixLabel21 = new QLabel( "-" );
    transitionMatrixLabel22 = new QLabel( "-" );
    transitionMatrixLabel23 = new QLabel( "-" );
    transitionMatrixLabel30 = new QLabel( "-" );
    transitionMatrixLabel31 = new QLabel( "-" );
    transitionMatrixLabel32 = new QLabel( "-" );
    transitionMatrixLabel33 = new QLabel( "-" );

     QLabel *xkEqualsBuddy = new QLabel( tr( "Xk =" ) );


     QLabel *matrixpkBuddy = new QLabel( tr( "Pk =" ) );
     /* Matrix A's element labels */
     errorCovPostLabel00 = new QLabel( "-" );
     errorCovPostLabel01 = new QLabel( "-" );
     errorCovPostLabel02 = new QLabel( "-" );
     errorCovPostLabel03 = new QLabel( "-" );
     errorCovPostLabel10 = new QLabel( "-" );
     errorCovPostLabel11 = new QLabel( "-" );
     errorCovPostLabel12 = new QLabel( "-" );
     errorCovPostLabel13 = new QLabel( "-" );
     errorCovPostLabel20 = new QLabel( "-" );
     errorCovPostLabel21 = new QLabel( "-" );
     errorCovPostLabel22 = new QLabel( "-" );
     errorCovPostLabel23 = new QLabel( "-" );
     errorCovPostLabel30 = new QLabel( "-" );
     errorCovPostLabel31 = new QLabel( "-" );
     errorCovPostLabel32 = new QLabel( "-" );
     errorCovPostLabel33 = new QLabel( "-" );

     //xk table labels
     statePostLabel0 = new QLabel( "-" );
     statePostLabel1 = new QLabel( "-" );
     statePostLabel2 = new QLabel( "-" );
     statePostLabel3 = new QLabel( "-" );

     //final table labels
     timeHeadwayAccumulatorLabel = new QLabel( "-" );
     timeDistanceAccumulatorLabel = new QLabel( "-" );
     leadVelocityAccumulatorLabel = new QLabel( "-" );
     leadAccelerationAccumulatorLabel = new QLabel( "-" );
     followingVelocityAccumulatorLabel = new QLabel( "-" );
     followingAccelerationAccumulatorLabel = new QLabel( "-" );

    QLabel *timeHeadwayAccumulatorBuddy = new QLabel( tr( "eta t, acc = " ) );
    QLabel *timeDistanceAccumulatorBuddy = new QLabel( tr( "eta d, acc = " ) );
    QLabel *leadVelocityAccumulatorBuddy = new QLabel( tr( "vl, acc = " ) );
    QLabel *leadAccelerationAccumulatorBuddy = new QLabel( tr( "al, acc = " ) );
    QLabel *followingVelocityAccumulatorBuddy = new QLabel( tr( "vf, acc = " ) );
    QLabel *followingAccelerationAccumulatorBuddy = new QLabel( tr( "af, acc = " ) );


     QFrame *line = new QFrame;
     line->setObjectName(QString::fromUtf8("line"));
     line->setGeometry(QRect(320, 150, 118, 3));
     line->setFrameShape(QFrame::HLine);
     line->setFrameShadow(QFrame::Sunken);

     QFrame *line1 = new QFrame;
     line1->setObjectName(QString::fromUtf8("line"));
     line1->setGeometry(QRect(320, 150, 118, 3));
     line1->setFrameShape(QFrame::HLine);
     line1->setFrameShadow(QFrame::Sunken);

     QFrame *line2 = new QFrame;
     line2->setObjectName(QString::fromUtf8("line"));
     line2->setGeometry(QRect(320, 150, 118, 3));
     line2->setFrameShape(QFrame::HLine);
     line2->setFrameShadow(QFrame::Sunken);

    /* Layout */
    QVBoxLayout *mainLayout = new QVBoxLayout();
    QGridLayout *layout1 = new QGridLayout();
    layout1->addWidget( timeUpdateBuddy,         0, 0, 2, 1, Qt::AlignVCenter );
    layout1->addWidget( xkmLabel,          	0, 1 );
    layout1->addWidget( pkmLabel,                1, 1 );
    layout1->addWidget( space,                   2, 0 );
    layout1->addWidget( measurementUpdateBuddy,         3, 0, 3, 1, Qt::AlignVCenter );

    layout1->addWidget( kkLabel,          	3, 1 );
    layout1->addWidget( xkLabel,                 4, 1 );
    layout1->addWidget( pkLabel,                 5, 1 );
    mainLayout->addLayout( layout1 );
    mainLayout->addWidget(line);
    // End of Table 1
  QGridLayout *layout2 = new QGridLayout();
    layout2->addWidget( matrixABuddy,         0, 0, 4, 1, Qt::AlignVCenter );
    layout2->addWidget( transitionMatrixLabel00,          	0, 1 );
    layout2->addWidget( transitionMatrixLabel01,          	0, 2 );
    layout2->addWidget( transitionMatrixLabel02,          	0, 3 );
    layout2->addWidget( transitionMatrixLabel03,          	0, 4 );

    layout2->addWidget( transitionMatrixLabel10,          	1, 1 );
    layout2->addWidget( transitionMatrixLabel11,          	1, 2 );
    layout2->addWidget( transitionMatrixLabel12,          	1, 3 );
    layout2->addWidget( transitionMatrixLabel13,          	1, 4 );

    layout2->addWidget( transitionMatrixLabel20,          	2, 1 );
    layout2->addWidget( transitionMatrixLabel21,          	2, 2 );
    layout2->addWidget( transitionMatrixLabel22,          	2, 3 );
    layout2->addWidget( transitionMatrixLabel23,          	2, 4 );

    layout2->addWidget( transitionMatrixLabel30,          	3, 1 );
    layout2->addWidget( transitionMatrixLabel31,          	3, 2 );
    layout2->addWidget( transitionMatrixLabel32,          	3, 3 );
    layout2->addWidget( transitionMatrixLabel33,          	3, 4 );
    mainLayout->addLayout( layout2 );
    mainLayout->addWidget(line1);
    //End of Table 2

    QGridLayout *layout3 = new QGridLayout();
      layout3->addWidget( xkEqualsBuddy,         0, 0, 4, 1, Qt::AlignVCenter );
      layout3->addWidget( statePostLabel0,          	0, 1 );
      layout3->addWidget( statePostLabel1,          	1, 1 );
      layout3->addWidget( statePostLabel2,          	2, 1 );
      layout3->addWidget( statePostLabel3,          	3, 1 );
      layout3->addWidget( space1,                   2, 0 );
      mainLayout->addLayout( layout3 );
      //End of Table 3

    QGridLayout *layout4 = new QGridLayout();
        layout4->addWidget( matrixpkBuddy,         0, 0, 4, 1, Qt::AlignVCenter );
        layout4->addWidget( errorCovPostLabel00,          	0, 1 );
        layout4->addWidget( errorCovPostLabel01,          	0, 2 );
        layout4->addWidget( errorCovPostLabel02,          	0, 3 );
        layout4->addWidget( errorCovPostLabel03,          	0, 4 );

        layout4->addWidget( errorCovPostLabel10,          	1, 1 );
        layout4->addWidget( errorCovPostLabel11,          	1, 2 );
        layout4->addWidget( errorCovPostLabel12,          	1, 3 );
        layout4->addWidget( errorCovPostLabel13,          	1, 4 );

        layout4->addWidget( errorCovPostLabel20,          	2, 1 );
        layout4->addWidget( errorCovPostLabel21,          	2, 2 );
        layout4->addWidget( errorCovPostLabel22,          	2, 3 );
        layout4->addWidget( errorCovPostLabel23,          	2, 4 );

        layout4->addWidget( errorCovPostLabel30,          	3, 1 );
        layout4->addWidget( errorCovPostLabel31,          	3, 2 );
        layout4->addWidget( errorCovPostLabel32,          	3, 3 );
        layout4->addWidget( errorCovPostLabel33,          	3, 4 );
        mainLayout->addLayout( layout4 );
        mainLayout->addWidget(line2);
        //End of Table 4


    QGridLayout *layout5 = new QGridLayout();
        layout5->addWidget( timeHeadwayAccumulatorBuddy,                    0, 0 );
        layout5->addWidget( timeDistanceAccumulatorBuddy,                   1, 0 );
        layout5->addWidget( leadVelocityAccumulatorBuddy,                   2, 0 );
        layout5->addWidget( leadAccelerationAccumulatorBuddy,               3, 0 );
        layout5->addWidget( followingVelocityAccumulatorBuddy,              4, 0 );
        layout5->addWidget( followingAccelerationAccumulatorBuddy,          5, 0 );

        layout5->addWidget( timeHeadwayAccumulatorLabel,                    0, 1 );
        layout5->addWidget( timeDistanceAccumulatorLabel,                   1, 1 );
        layout5->addWidget( leadVelocityAccumulatorLabel,                   2, 1 );
        layout5->addWidget( leadAccelerationAccumulatorLabel,               3, 1 );
        layout5->addWidget( followingVelocityAccumulatorLabel,              4, 1 );
        layout5->addWidget( followingAccelerationAccumulatorLabel,          5, 1 );

        mainLayout->addLayout( layout5 );
        //End of Table 5

    mainLayout->addStretch(1);
    //layout->setRowStretch( 5, 1 );
    setLayout( mainLayout );

}

KalmanTab::~KalmanTab() {
    qDebug() << "~KalmansTab()";
}

/*
 DON'T
 FORGET
 ABOUT
 FUNCTIONS
 THAT
 TAKE
 IN MATRICES
 OKAY?
 THERE'S
 THREE
 OF
 THEM
 THREE
  */

void KalmanTab::setCorrectedErrorEstimateMinusValue( float value ) {
    QString s;
    s.sprintf( "%.3f ", value );

    pkmLabel->setText( s );
}

void KalmanTab::setCorrectedStateEstimateValue( float value ) {
    QString s;
    s.sprintf( "%.3f ", value );

    xkLabel->setText( s );
}

void KalmanTab::setCorrectedErrorEstimateValue( float value ) {
    QString s;
    s.sprintf( "%.3f ", value );

    pkLabel->setText( s );
}

void KalmanTab::setKalmanGainValue( float value ) {
    QString s;
    s.sprintf( "%.3f ", value );

    kkLabel->setText( s );
}

void KalmanTab::setTimeHeadwayAccumulatorValue( float value ) {
    QString s;
    s.sprintf( "%.3f ", value );

    timeHeadwayAccumulatorLabel->setText( s );
}

void KalmanTab::setTimeDistanceAccumulatorValue( float value ) {
    QString s;
    s.sprintf( "%.3f ", value );

    timeDistanceAccumulatorLabel->setText( s );
}

void KalmanTab::setLeadVelocityAccumulatorValue( float value ) {
    QString s;
    s.sprintf( "%.3f ", value );

    leadVelocityAccumulatorLabel->setText( s );
}

void KalmanTab::setLeadAccelerationAccumulatorValue( float value ) {
    QString s;
    s.sprintf( "%.3f ", value );

    leadAccelerationAccumulatorLabel->setText( s );
}

void KalmanTab::setFollowingVelocityAccumulatorValue( float value ) {
    QString s;
    s.sprintf( "%.3f ", value );

    followingVelocityAccumulatorLabel->setText( s );
}

void KalmanTab::setFollowingAccelerationAccumulatorValue( float value ) {
    QString s;
    s.sprintf( "%.3f ", value );

    followingAccelerationAccumulatorLabel->setText( s );
}


