#ifndef VIDEOWINDOW_H
#define VIDEOWINDOW_H

#include <opencv2/core/core.hpp>
#include <QDialog>
#include <QLabel>
#include <QMutex>

class VideoWindow : public QDialog
{
    Q_OBJECT

public:
    VideoWindow( QWidget *parent );
    ~VideoWindow();

signals:
    void gotVideoSelection( int type );

public slots:
    void updateVideoFrame( cv::Mat &frame, int type );

private slots:
    void comboBoxChanged( int index );

private:
    QLabel *videoLabel;

    QMutex videoSelectionMutex;
    int videoSelection;
};

#endif
