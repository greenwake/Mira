#pragma once
#include <QObject>
#include <QtCharts/QXYSeries>
#include "../network/TelemetryClient.h" // Struct'ı kullanabilmek için

class MiraController : public QObject {
    Q_OBJECT
public:
    explicit MiraController(QObject *parent = nullptr);
    Q_INVOKABLE void startSystem();

    Q_INVOKABLE void setEbiSeries(QXYSeries* series);
    Q_INVOKABLE void setPermittedSeries(QXYSeries* series);
    Q_INVOKABLE void setWarningSeries(QXYSeries* series);
    Q_INVOKABLE void setSbi1Series(QXYSeries* series);
    Q_INVOKABLE void setSbi2Series(QXYSeries* series);
    Q_INVOKABLE void setIndicationSeries(QXYSeries* series);
    Q_INVOKABLE void setEoaSeries(QXYSeries* series);
    Q_INVOKABLE void setSvlSeries(QXYSeries* series);

private slots:
    // YENİ: Paketi tek seferde alan ve işleyen slot
    void handleTelemetryData(TelemetryData data);
    void clearAllSeries();

private:
    TelemetryClient* m_telemetryClient;

    // YENİ: Zıplama/Sıfırlama kontrolünü artık Controller yapacak
    double m_lastDistanceToTarget = -1.0;

    QXYSeries* m_ebiSeries = nullptr;
    QXYSeries* m_permittedSeries = nullptr;
    QXYSeries* m_warningSeries = nullptr;
    QXYSeries* m_sbi1Series = nullptr;
    QXYSeries* m_sbi2Series = nullptr;
    QXYSeries* m_indicationSeries = nullptr;
    QXYSeries* m_eoaSeries = nullptr;
    QXYSeries* m_svlSeries = nullptr;
};
