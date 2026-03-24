#include "MiraController.h"
#include "../config/ConfigManager.h"
#include <QDebug>

MiraController::MiraController(QObject *parent) : QObject(parent) {
    qRegisterMetaType<TelemetryData>("TelemetryData");
    m_telemetryClient = new TelemetryClient(this);
    connect(m_telemetryClient, &TelemetryClient::telemetryReceived, this, &MiraController::handleTelemetryData);
    connect(m_telemetryClient, &TelemetryClient::telemetryReceived, this, &MiraController::printer);

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

void MiraController::printer(TelemetryData data)
{
    qDebug() << "#############################################\n";
    qDebug() << "Current Speed : " << data.v_est_kmh;
    qDebug() << "X-SBI1 : " << data.x_sbi1 << " -- Y-SBI1 : "<<data.y_sbi1;
    qDebug() << "X-SBI2 : " << data.x_sbi2 << " -- Y-SBI2 : "<<data.y_sbi2;
    qDebug() << "X-EBI : " << data.x_ebi << " -- Y-EBI : "<<data.y_ebi;
    qDebug() << "X-Warning : " << data.x_warning << " -- Y-Warning : "<<data.y_warning;
    qDebug() << "X-Permitted : " << data.x_permitted << " -- Y-Permitted : "<<data.y_permitted;
    qDebug() << "X-Indication : " << data.x_indication << " -- Y-Indication : "<<data.y_indication;
    qDebug() << "\n#############################################\n";
}

void MiraController::handleTelemetryData(TelemetryData data) {
    // QML Arayüz Güncellemesi
    m_currentSpeedText = QString::number(data.v_est_kmh, 'f', 0) + " km/h";
    m_currentLevelText = data.currentLevel;
    emit uiDataChanged();

    if (data.hasCurves) {
        if (m_lastDistanceToTarget != -1.0 && data.x_ebi > (m_lastDistanceToTarget + 10.0)) {
            clearAllSeries();
        }
        m_lastDistanceToTarget = data.x_ebi;

        if (m_ebiSeries) m_ebiSeries->append(data.x_ebi, data.y_ebi);
        if (m_permittedSeries) m_permittedSeries->append(data.x_permitted, data.y_permitted);
        if (m_warningSeries) m_warningSeries->append(data.x_warning, data.y_warning);
        if (m_sbi1Series) m_sbi1Series->append(data.x_sbi1, data.y_sbi1);
        if (m_sbi2Series) m_sbi2Series->append(data.x_sbi2, data.y_sbi2);
        if (m_indicationSeries) m_indicationSeries->append(data.x_indication, data.y_indication);
    }

    if (data.hasEoa && m_eoaSeries) m_eoaSeries->append(data.dEoa, data.v_est_kmh);
    if (data.hasSvl && m_svlSeries) m_svlSeries->append(data.dSvl, data.v_est_kmh);
}
