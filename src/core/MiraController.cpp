#include "MiraController.h"
#include "../config/ConfigManager.h"

MiraController::MiraController(QObject *parent)
    : QObject(parent)
{
    m_telemetryClient = new TelemetryClient(this);

    connect(m_telemetryClient, &TelemetryClient::ebiDataReceived, this, &MiraController::handleEbiData);
    connect(m_telemetryClient, &TelemetryClient::permittedDataReceived, this, &MiraController::handlePermittedData);
    connect(m_telemetryClient, &TelemetryClient::warningDataReceived, this, &MiraController::handleWarningData);
    connect(m_telemetryClient, &TelemetryClient::sbi1DataReceived, this, &MiraController::handleSbi1Data);
    connect(m_telemetryClient, &TelemetryClient::sbi2DataReceived, this, &MiraController::handleSbi2Data);
    connect(m_telemetryClient, &TelemetryClient::indicationDataReceived, this, &MiraController::handleIndicationData);
    connect(m_telemetryClient, &TelemetryClient::eoaTargetReceived, this, &MiraController::handleEoaData);
    connect(m_telemetryClient, &TelemetryClient::svlTargetReceived, this, &MiraController::handleSvlData);

    // YENİ BAĞLANTI: Sıfırlama sinyalini bağla
    connect(m_telemetryClient, &TelemetryClient::curveResetTriggered, this, &MiraController::clearAllSeries);
}

void MiraController::startSystem() {
    QString ip = ConfigManager::instance().getServerIP();
    quint16 port = ConfigManager::instance().getServerPort();
    m_telemetryClient->connectToServer(ip, port);
}

void MiraController::setEbiSeries(QXYSeries* series) { m_ebiSeries = series; }
void MiraController::setPermittedSeries(QXYSeries* series) { m_permittedSeries = series; }
void MiraController::setWarningSeries(QXYSeries* series) { m_warningSeries = series; }
void MiraController::setSbi1Series(QXYSeries* series) { m_sbi1Series = series; }
void MiraController::setSbi2Series(QXYSeries* series) { m_sbi2Series = series; }
void MiraController::setIndicationSeries(QXYSeries* series) { m_indicationSeries = series; }
void MiraController::setEoaSeries(QXYSeries* series) { m_eoaSeries = series; }
void MiraController::setSvlSeries(QXYSeries* series) { m_svlSeries = series; }

// YENİ FONKSİYON: Tüm serileri temizler (Grafik sıfırlanır)
void MiraController::clearAllSeries() {
    if (m_ebiSeries) m_ebiSeries->clear();
    if (m_permittedSeries) m_permittedSeries->clear();
    if (m_warningSeries) m_warningSeries->clear();
    if (m_sbi1Series) m_sbi1Series->clear();
    if (m_sbi2Series) m_sbi2Series->clear();
    if (m_indicationSeries) m_indicationSeries->clear();
    if (m_eoaSeries) m_eoaSeries->clear();
    if (m_svlSeries) m_svlSeries->clear();
}

void MiraController::handleEbiData(double distance, double speed) { if (m_ebiSeries) m_ebiSeries->append(distance, speed); }
void MiraController::handlePermittedData(double distance, double speed) { if (m_permittedSeries) m_permittedSeries->append(distance, speed); }
void MiraController::handleWarningData(double distance, double speed) { if (m_warningSeries) m_warningSeries->append(distance, speed); }
void MiraController::handleSbi1Data(double distance, double speed) { if (m_sbi1Series) m_sbi1Series->append(distance, speed); }
void MiraController::handleSbi2Data(double distance, double speed) { if (m_sbi2Series) m_sbi2Series->append(distance, speed); }
void MiraController::handleIndicationData(double distance, double speed) { if (m_indicationSeries) m_indicationSeries->append(distance, speed); }
void MiraController::handleEoaData(double distance, double speed) { if (m_eoaSeries) m_eoaSeries->append(distance, speed); }
void MiraController::handleSvlData(double distance, double speed) { if (m_svlSeries) m_svlSeries->append(distance, speed); }
