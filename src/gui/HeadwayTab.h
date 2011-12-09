#ifndef HEADWAYTAB_H
#define HEADWAYTAB_H

#include <QWidget>

class QLabel;

class HeadwayTab : public QWidget
{
    Q_OBJECT

public:
    HeadwayTab( QWidget *parent );
    ~HeadwayTab();

    void setSafeTimeHeadway( float time );
    void setIcePresent( bool ice );
    void setRainPresent( bool rain );
    void setReactionTime( float time );

private:
    QLabel *safeTimeHeadwayLabel;
    QLabel *icePresentLabel;
    QLabel *rainPresentLabel;
    QLabel *reactionTimeLabel;
};
#endif
