#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QString>

#include "HeadwayTab.h"

HeadwayTab::HeadwayTab( QWidget *parent = 0 ) : QWidget( parent ) {
    /* Formula */
    QLabel *formulaLabel = new QLabel( "<b><i>t</i></b><sub>safe</sub>  =  1.0 &sdot; <i>ice</i>  +  0.4 &sdot; <i>rain</i>  +  <b><i>t</i></b><sub>d</sub>" );

    /* Safe time headway */
    QLabel *safeTimeHeadwayBuddy = new QLabel( "Safe time headway (<i>t</i><sub>safe</sub>):" );
    safeTimeHeadwayLabel = new QLabel( "-" );

    /* Ice */
    QLabel *icePresentBuddy = new QLabel( tr( "Ice Present" ) );
    icePresentLabel = new QLabel( tr( "false" ) );

    /* Rain */
    QLabel *rainPresentBuddy = new QLabel( tr( "Rain Present:" ) );
    rainPresentLabel = new QLabel( tr( "false" ) );

    /* Reaction time */
    QLabel *reactionTimeBuddy = new QLabel( tr( "Reaction time (<i>t</i><sub>d</sub>):" ) );
    reactionTimeLabel = new QLabel( tr( "-" ) );

    /* Layout */
    QGridLayout *layout = new QGridLayout();
    layout->addWidget( formulaLabel,         0, 0, 1, 2, Qt::AlignHCenter );
    layout->addWidget( safeTimeHeadwayBuddy, 2, 0 );
    layout->addWidget( safeTimeHeadwayLabel, 2, 1 );
    layout->addWidget( icePresentBuddy,      3, 0 );
    layout->addWidget( icePresentLabel,      3, 1 );
    layout->addWidget( rainPresentBuddy,     4, 0 );
    layout->addWidget( rainPresentLabel,     4, 1 );
    layout->addWidget( reactionTimeBuddy,    5, 0 );
    layout->addWidget( reactionTimeLabel,    5, 1 );

    layout->setRowStretch( 6, 1 );

    setLayout( layout );
}

HeadwayTab::~HeadwayTab() {

}

void HeadwayTab::setSafeTimeHeadway( float time ) {
    QString s;
    s.sprintf( "%.1f", time );

    reactionTimeLabel->setText( s );

}

void HeadwayTab::setIcePresent( bool ice ) {
    if ( ice ) {
        icePresentLabel->setText( "true" );
    } else {
        /* I think we decided this wasn't allowed */
    }
}

void HeadwayTab::setRainPresent( bool rain ) {
    if ( rain ) {
        rainPresentLabel->setText( tr( "true" ) );
    } else {
        rainPresentLabel->setText( tr( "false" ) );
    }
}

void HeadwayTab::setReactionTime( float reactionTime ) {
    QString s;
    s.sprintf( "%.1f", reactionTime );

    reactionTimeLabel->setText( s );
}

