#ifndef VIDEOWINDOW_H
#define VIDEOWINDOW_H

#include <QDialog>
#include <QLabel>
#include <QMutex>

#include "globals.h"

class VideoWindow : public QDialog
{
    Q_OBJECT

public:
    VideoWindow( QWidget *parent );
    ~VideoWindow();

signals:
    void gotVideoSelection( int type );

public slots:
    void updateVideoFrame( _Mat frame, int type, _QReadWriteLock lock );

private slots:
    void comboBoxChanged( int index );

private:
    QLabel *videoLabel;

    QMutex videoSelectionMutex;
    int videoSelection;
};

#endif
