#include "MiraController.h"
#include "../config/ConfigManager.h"
#include <QDebug>

MiraController::MiraController(QObject *parent) : QObject(parent) {
    qRegisterMetaType<TelemetryData>("TelemetryData");
    m_telemetryClient = new TelemetryClient(this);

    m_telemetryServer = new TelemetryServer(9586, this);

    connect(m_telemetryClient, &TelemetryClient::rawDataReceived,
            m_telemetryServer, &TelemetryServer::broadcastData);


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

void MiraController::increaseRange500()
{
    m_distanceRange = m_distanceRange + 500;
    emit uiDataChanged();
}

void MiraController::increaseRange50()
{
    m_speedRange = m_speedRange + 50;
    emit uiDataChanged();
}

void MiraController::decreaseRange500()
{
    m_distanceRange = (m_distanceRange - 500 >=1000) ? (m_distanceRange - 500) : 1000.0;
    emit uiDataChanged();
}

void MiraController::decreaseRange50()
{
    m_speedRange = (m_speedRange - 50 >=50) ? (m_speedRange - 50) : 50;
    emit uiDataChanged();
}

void MiraController::printer(TelemetryData *Data)
{
    qDebug() << "#############################################\n";
    qDebug() << "Current Speed : " << Data->v_est_kmh;
    qDebug() << "X-SBI1 : " << Data->x_sbi1 << " -- Y-SBI1 : "<<Data->y_sbi1;
    qDebug() << "X-SBI2 : " << Data->x_sbi2 << " -- Y-SBI2 : "<<Data->y_sbi2;
    qDebug() << "X-EBI : " << Data->x_ebi << " -- Y-EBI : "<<Data->y_ebi;
    qDebug() << "X-Warning : " << Data->x_warning << " -- Y-Warning : "<<Data->y_warning;
    qDebug() << "X-Permitted : " << Data->x_permitted << " -- Y-Permitted : "<<Data->y_permitted;
    qDebug() << "X-Indication : " << Data->x_indication << " -- Y-Indication : "<<Data->y_indication;
    qDebug() << "\n#############################################\n";
}


void MiraController::handleTelemetryData(TelemetryData *tData) {

    m_currentSpeedText = QString::number(tData->v_est_kmh, 'f', 0) + " km/h";

    m_trainMaxSpeedText = QString::number(tData->trainMaxSpeed, 'f', 0) + " km/h";
    m_trainMaxSpeed = tData->trainMaxSpeed;

    m_currentLevelText = tData->currentLevelText;

    qDebug() << "m_currentModeText : " << tData->currentModeText;
    m_currentModeText = tData->currentModeText;
    emit uiDataChanged();

    if (tData->hasCurves) {
        m_lastDistanceToTarget = tData->x_ebi;

        if (m_ebiSeries) m_ebiSeries->append(tData->x_ebi, tData->y_ebi);
        if (m_permittedSeries) m_permittedSeries->append(tData->x_permitted, tData->y_permitted);
        if (m_warningSeries) m_warningSeries->append(tData->x_warning, tData->y_warning);
        if (m_sbi1Series) m_sbi1Series->append(tData->x_sbi1, tData->y_sbi1);
        if (m_sbi2Series) m_sbi2Series->append(tData->x_sbi2, tData->y_sbi2);
        if(tData->monitoringStatus){
            if (m_indicationSeries) m_indicationSeries->append(tData->x_indication, tData->y_indication);
        }

    }

    if (tData->hasEoa && m_eoaSeries) m_eoaSeries->append(tData->dEoa, tData->v_est_kmh);
    if (tData->hasSvl && m_svlSeries) m_svlSeries->append(tData->dSvl, tData->v_est_kmh);
}
