#include <QDirIterator>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QObject>
#include <QPushButton>
#include <QRadioButton>
#include <QString>

#include "VideoSelectDialog.h"

VideoSelectDialog::VideoSelectDialog()
{
    makeLayout();

    setWindowTitle( tr( "Select Video File or Device" ) );
}

void VideoSelectDialog::makeLayout()
{
    fileRadioButton      = new QRadioButton( tr( "File:" ), this );
    deviceRadioButton    = new QRadioButton( tr( "Device:" ), this );
    fileLineEdit         = new QLineEdit( this );
    fileBrowsePushButton = new QPushButton( tr( "Browse..." ), this );
    deviceListWidget     = new QListWidget;

    /* Put items in a layout */
    {
        QGridLayout *layout = new QGridLayout;

        layout->addWidget( fileRadioButton,       0, 0, 1, 1, Qt::AlignLeft );
        layout->addWidget( deviceRadioButton,     1, 0, 1, 1, Qt::AlignLeft | Qt::AlignTop );
        layout->addWidget( fileLineEdit,          0, 1, 1, 1 );
        layout->addWidget( fileBrowsePushButton,  0, 2, 1, 1, Qt::AlignRight );
        layout->addWidget( deviceListWidget,      1, 1, 1, 2 );

        setLayout( layout );
    }

    /* Make connections */
    {
        connect( fileLineEdit, SIGNAL( textChanged( const QString & ) ),
                 this,         SLOT(   filePathChanged( const QString & ) ) );

        connect( fileBrowsePushButton, SIGNAL( clicked() ),
                 this,                 SLOT(   browse() ) );

        connect( deviceListWidget, SIGNAL( currentRowChanged( int ) ),
                 this,             SLOT(   deviceChanged( int ) ) );
    }

    findDevices();
}

void VideoSelectDialog::browse()
{
    fileLineEdit->setText( QFileDialog::getOpenFileName( this,
                                                         tr( "Open Video File" ),
                                                         "/home",
                                                         tr( "Videos (*.avi *.mkv *.mov)" ) ) );
}

void VideoSelectDialog::deviceChanged( int row )
{
    deviceRadioButton->setChecked( true );
}

void VideoSelectDialog::findDevices()
{
    QDirIterator iterator( "/dev/v4l/by-id",
                           QDirIterator::NoIteratorFlags );

    while ( iterator.hasNext() ) {
        QFileInfo fi( iterator.next() );

        if ( !fi.fileName().isEmpty() && fi.fileName() != "." && fi.fileName() 
            != ".." ) {
            deviceListWidget->addItem( fi.fileName() );
        }
    }
}

void VideoSelectDialog::filePathChanged( const QString &path )
{
    fileRadioButton->setChecked( true );
}
