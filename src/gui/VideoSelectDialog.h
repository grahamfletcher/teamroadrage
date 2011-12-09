#ifndef VIDEOSELECTDIALOG_H
#define VIDEOSELECTDIALOG_H

#include <QDialog>

class QLineEdit;
class QListWidget;
class QPushButton;
class QRadioButton;

class VideoSelectDialog : public QDialog
{
    Q_OBJECT

public:
    VideoSelectDialog();

private slots:
    void browse();
    void deviceChanged( int row );
    void filePathChanged( const QString &path );
    void findDevices();

private:
    QRadioButton *fileRadioButton;
    QRadioButton *deviceRadioButton;
    QLineEdit    *fileLineEdit;
    QPushButton  *fileBrowsePushButton;
    QListWidget  *deviceListWidget;

    void makeLayout();
};

#endif
