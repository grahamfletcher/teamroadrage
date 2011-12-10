#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QString>

#include "SensorsTab.h"

SensorsTab::SensorsTab( QWidget *parent = 0 ) : QWidget( parent ) {
    /* Distance */
    QLabel *distanceBuddy = new QLabel( tr( "Measured distance:" ) );
    distanceLabel = new QLabel( "-" );

    /* Speed */
    QLabel *speedBuddy = new QLabel( tr( "Following vehicle speed:" ) );
    speedLabel = new QLabel( "-" );

    /* Humidity */
    QLabel *humidityBuddy = new QLabel( tr( "Humidity:" ) );
    humidityLabel = new QLabel( "-" );

    /* Rain */
    QLabel *rainSensorVoltageBuddy = new QLabel( tr( "Rain sensor voltage:" ) );
    rainSensorVoltageLabel = new QLabel( "-" );

    /* Temperature */
    QLabel *temperatureBuddy = new QLabel( tr( "Temperature:" ) );
    temperatureLabel = new QLabel( "-" );

    /* Layout */
    QGridLayout *layout = new QGridLayout();
    layout->addWidget( distanceBuddy,          0, 0 );
    layout->addWidget( distanceLabel,          0, 1 );
    layout->addWidget( speedBuddy,             1, 0 );
    layout->addWidget( speedLabel,             1, 1 );
    layout->addWidget( humidityBuddy,          2, 0 );
    layout->addWidget( humidityLabel,          2, 1 );
    layout->addWidget( rainSensorVoltageBuddy, 3, 0 );
    layout->addWidget( rainSensorVoltageLabel, 3, 1 );
    layout->addWidget( temperatureBuddy,       4, 0 );
    layout->addWidget( temperatureLabel,       4, 1 );

    layout->setRowStretch( 5, 1 );

    setLayout( layout );
}

SensorsTab::~SensorsTab() {

}
void SensorsTab::setRawDistance( float distance ) {
    QString s;
    s.sprintf( "%.1f m", distance );

    distanceLabel->setText( s );
}

void SensorsTab::setRawFollowingVehicleVelocity( float velocity ) {
    QString s;
    s.sprintf( "%.1f m/s", velocity );

    speedLabel->setText( s );
}

void SensorsTab::setHumidity( float humidity ) {
    QString s;
    s.sprintf( "%.1f %%", humidity );

    humidityLabel->setText( s );
}

void SensorsTab::setRainSensorVoltage( float voltage ) {
    QString s;
    s.sprintf( "%.1f V", voltage );

    rainSensorVoltageLabel->setText( s );
}

void SensorsTab::setTemperature( float temperature ) {
    QString s;
    s.sprintf( "%.1f %cC", temperature, 0x00B0 );

    temperatureLabel->setText( s );
}
