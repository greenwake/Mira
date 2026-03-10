#pragma once
#include <QObject>
#include <QtCharts/QXYSeries>
#include "../network/TelemetryClient.h"

class MiraController : public QObject {
    Q_OBJECT
public:
    explicit MiraController(QObject *parent = nullptr);
    
    // QML'den cagrilarak sistemi baslatir
    Q_INVOKABLE void startSystem();

    // QML'deki grafik serilerini C++'a baglamak icin kullanilir
    Q_INVOKABLE void setEbiSeries(QXYSeries* series);
    Q_INVOKABLE void setPermittedSeries(QXYSeries* series);

private slots:
    void handleEbiData(double distance, double speed);
    void handlePermittedData(double distance, double speed);

private:
    TelemetryClient* m_telemetryClient;
    QXYSeries* m_ebiSeries;
    QXYSeries* m_permittedSeries;
};