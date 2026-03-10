#include "MiraController.h"
#include "../config/ConfigManager.h"

MiraController::MiraController(QObject *parent) 
    : QObject(parent), m_ebiSeries(nullptr), m_permittedSeries(nullptr) {
    m_telemetryClient = new TelemetryClient(this);
    
    connect(m_telemetryClient, &TelemetryClient::ebiDataReceived, this, &MiraController::handleEbiData);
    connect(m_telemetryClient, &TelemetryClient::permittedDataReceived, this, &MiraController::handlePermittedData);
}

void MiraController::startSystem() {
    QString ip = ConfigManager::instance().getServerIP();
    quint16 port = ConfigManager::instance().getServerPort();
    m_telemetryClient->connectToServer(ip, port);
}

void MiraController::setEbiSeries(QXYSeries* series) { m_ebiSeries = series; }
void MiraController::setPermittedSeries(QXYSeries* series) { m_permittedSeries = series; }

void MiraController::handleEbiData(double distance, double speed) {
    if (m_ebiSeries) {
        m_ebiSeries->append(distance, speed);
    }
}

void MiraController::handlePermittedData(double distance, double speed) {
    if (m_permittedSeries) {
        m_permittedSeries->append(distance, speed);
    }
}