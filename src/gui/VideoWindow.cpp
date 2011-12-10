#include <QComboBox>
#include <QDebug>
#include <QMutexLocker>
#include <QVBoxLayout>

#include "globals.h"
#include "MatToQImage.h"
#include "VideoWindow.h"

VideoWindow::VideoWindow( QWidget *parent = 0 ) : QDialog( parent ), videoSelection( 0 ) {

    QComboBox *comboBox = new QComboBox();
    comboBox->insertItem( ORIGINAL, "Original" );
    comboBox->insertItem( IPM, "IPM" );
    comboBox->insertItem( IPM_LANES, "IPM (Lane Detection)" );
    comboBox->insertItem( LANES_VEHICLES, "Detected Lanes and Vehicles" );
    QObject::connect( comboBox, SIGNAL( currentIndexChanged( int ) ),
                      this,     SLOT( comboBoxChanged( int ) ) );

    videoLabel = new QLabel( this );

    QVBoxLayout *layout = new QVBoxLayout( this );
    layout->addWidget( videoLabel );
    layout->addWidget( comboBox );

    setLayout( layout );

    setWindowTitle( tr( "Vision" ) );
}

VideoWindow::~VideoWindow() {

}

void VideoWindow::comboBoxChanged( int index ) {
    QMutexLocker locker( &videoSelectionMutex );
    videoSelection = index;
}

void VideoWindow::updateVideoFrame( cv::Mat &frame, int type ) {
    QMutexLocker locker( &videoSelectionMutex );

    qDebug() << "poo" << type;

    if ( type == videoSelection ) {
        videoLabel->setPixmap( QPixmap::fromImage( MatToQImage( frame ) ) );
    }
}

