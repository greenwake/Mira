#pragma once
#include <QObject>
#include <QtCharts/QXYSeries>
#include "../network/TelemetryClient.h"

class MiraController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentSpeedText READ currentSpeedText NOTIFY uiDataChanged)
    Q_PROPERTY(QString currentLevelText READ currentLevelText NOTIFY uiDataChanged)

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

    QString currentSpeedText() const { return m_currentSpeedText; }
    QString currentLevelText() const { return m_currentLevelText; }

signals:
    void uiDataChanged();

private slots:
    void handleTelemetryData(TelemetryData data);
    void clearAllSeries();
    void printer(TelemetryData data);

private:
    TelemetryClient* m_telemetryClient;
    double m_lastDistanceToTarget = -1.0;

    QString m_currentSpeedText = "0 km/h";
    QString m_currentLevelText = "Level -";

    QXYSeries* m_ebiSeries = nullptr;
    QXYSeries* m_permittedSeries = nullptr;
    QXYSeries* m_warningSeries = nullptr;
    QXYSeries* m_sbi1Series = nullptr;
    QXYSeries* m_sbi2Series = nullptr;
    QXYSeries* m_indicationSeries = nullptr;
    QXYSeries* m_eoaSeries = nullptr;
    QXYSeries* m_svlSeries = nullptr;
};
