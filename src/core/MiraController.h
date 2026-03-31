#pragma once
#include <QObject>
#include <QtCharts/QXYSeries>
#include "../network/TelemetryClient.h"
#include "../network/TelemetryServer.h"

class MiraController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentSpeedText READ currentSpeedText NOTIFY uiDataChanged)
    Q_PROPERTY(QString currentLevelText READ currentLevelText NOTIFY uiDataChanged)
    Q_PROPERTY(QString currentModeText READ currentModeText NOTIFY uiDataChanged)
    Q_PROPERTY(QString trainMaxSpeedText READ trainMaxSpeedText NOTIFY uiDataChanged)
    Q_PROPERTY(int distanceRange READ distanceRange NOTIFY uiDataChanged)
    Q_PROPERTY(int trainMaxSpeed READ trainMaxSpeed NOTIFY uiDataChanged)
    Q_PROPERTY(int speedRange READ speedRange NOTIFY uiDataChanged)

public:
    explicit MiraController(QObject *parent = nullptr);
    Q_INVOKABLE void startSystem();
    Q_INVOKABLE void clearAllSeries();
    Q_INVOKABLE void increaseRange500();
    Q_INVOKABLE void increaseRange50();
    Q_INVOKABLE void decreaseRange500();
    Q_INVOKABLE void decreaseRange50();

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
    QString currentModeText() const { return m_currentModeText; }
    QString trainMaxSpeedText() const { return m_trainMaxSpeedText; }
    int trainMaxSpeed() const { return m_trainMaxSpeed; }
    int distanceRange() const { return m_distanceRange; }
    int speedRange() const { return m_speedRange; }

signals:
    void uiDataChanged();

private slots:
    void handleTelemetryData(TelemetryData *data);
    void printer(TelemetryData *data);

private:
    TelemetryClient* m_telemetryClient;
    TelemetryServer* m_telemetryServer;
    double m_lastDistanceToTarget = -1.0;

    QString m_currentSpeedText = "0 km/h";
    QString m_currentLevelText = "Level -";
    QString m_currentModeText = "Standby";
    QString m_trainMaxSpeedText = "0 km/h";
    int m_distanceRange = 2000;
    int m_speedRange = 200;
    int m_trainMaxSpeed = 160;

    QXYSeries* m_ebiSeries = nullptr;
    QXYSeries* m_permittedSeries = nullptr;
    QXYSeries* m_warningSeries = nullptr;
    QXYSeries* m_sbi1Series = nullptr;
    QXYSeries* m_sbi2Series = nullptr;
    QXYSeries* m_indicationSeries = nullptr;
    QXYSeries* m_eoaSeries = nullptr;
    QXYSeries* m_svlSeries = nullptr;
};
