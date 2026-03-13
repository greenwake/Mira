#pragma once
#include <QObject>
#include <QtCharts/QXYSeries>
#include "../network/TelemetryClient.h"

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
    void handleEbiData(double d, double v);
    void handlePermittedData(double d, double v);
    void handleWarningData(double d, double v);
    void handleSbi1Data(double d, double v);
    void handleSbi2Data(double d, double v);
    void handleIndicationData(double d, double v);
    void handleEoaData(double d, double v);
    void handleSvlData(double d, double v);

    // YENİ: Temizleme slotu
    void clearAllSeries();

private:
    TelemetryClient* m_telemetryClient;
    QXYSeries* m_ebiSeries = nullptr;
    QXYSeries* m_permittedSeries = nullptr;
    QXYSeries* m_warningSeries = nullptr;
    QXYSeries* m_sbi1Series = nullptr;
    QXYSeries* m_sbi2Series = nullptr;
    QXYSeries* m_indicationSeries = nullptr;
    QXYSeries* m_eoaSeries = nullptr;
    QXYSeries* m_svlSeries = nullptr;
};
