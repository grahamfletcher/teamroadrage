#include <QComboBox>
#include <QDebug>
#include <QMutexLocker>
#include <QReadLocker>
#include <QVBoxLayout>

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
    videoLabel->setScaledContents( true );

    QVBoxLayout *layout = new QVBoxLayout( this );
    layout->addWidget( videoLabel );
    layout->addWidget( comboBox );

    layout->setMargin( 0 );

    setLayout( layout );

    setWindowTitle( tr( "Vision" ) );
}

VideoWindow::~VideoWindow() {
    qDebug() << "~VideoWindow()";
}

void VideoWindow::comboBoxChanged( int index ) {
    QMutexLocker locker( &videoSelectionMutex );
    videoSelection = index;
}

void VideoWindow::updateVideoFrame( cv::Mat *frame, int type, QReadWriteLock *lock ) {
    QMutexLocker locker( &videoSelectionMutex );

    qDebug() << "VideoWindow::updateVideoFrame()" << type;

    if ( type == videoSelection ) {
        QReadLocker locker( lock );

        videoLabel->setPixmap( QPixmap::fromImage( MatToQImage( *frame ) ) );
    }
}

