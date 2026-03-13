#include "MiraController.h"
#include "../config/ConfigManager.h"
#include <QDebug>

MiraController::MiraController(QObject *parent) : QObject(parent) {
    // ÇOK ÖNEMLİ: Kendi oluşturduğumuz struct tipini Qt'ye kaydediyoruz
    qRegisterMetaType<TelemetryData>("TelemetryData");

    m_telemetryClient = new TelemetryClient(this);

    // Tek bir sinyali bağlıyoruz
    connect(m_telemetryClient, &TelemetryClient::telemetryReceived, this, &MiraController::handleTelemetryData);
}

void MiraController::startSystem() {
    QString ip = ConfigManager::instance().getServerIP();
    quint16 port = ConfigManager::instance().getServerPort();
    m_telemetryClient->connectToServer(ip, port);
}

// QML Setter Fonksiyonları...
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

// YENİ: Struct paketi buraya düşer ve ekran güncellenir
void MiraController::handleTelemetryData(TelemetryData data) {

    if (data.hasCurves) {
        // Yeni Yetki (Movement Authority) Sıfırlama Kontrolü
        if (m_lastDistanceToTarget != -1.0 && data.dEbi > (m_lastDistanceToTarget + 10.0)) {
            qDebug() << "Grafik Sifirlaniyor! Mesafe sictamasi:" << m_lastDistanceToTarget << "->\n" << data.dEbi;
            clearAllSeries();
        }
        m_lastDistanceToTarget = data.dEbi;

        // Eğrileri struct'tan okuyup grafiğe ekliyoruz
        if (m_ebiSeries) m_ebiSeries->append(data.dEbi, data.vEbi);
        if (m_permittedSeries) m_permittedSeries->append(data.dPermitted, data.vPermitted);
        if (m_warningSeries) m_warningSeries->append(data.dWarning, data.vWarning);
        if (m_sbi1Series) m_sbi1Series->append(data.dSbi1, data.vSbi);
        if (m_sbi2Series) m_sbi2Series->append(data.dSbi2, data.vSbi);
        if (m_indicationSeries) m_indicationSeries->append(data.dIndication, data.v_est_kmh); // Y ekseni olarak v_est kullanıyoruz
    }

    // Hedef noktalarını struct'tan okuyup grafiğe ekliyoruz
    if (data.hasEoa && m_eoaSeries) m_eoaSeries->append(data.dEoa, data.v_est_kmh);
    if (data.hasSvl && m_svlSeries) m_svlSeries->append(data.dSvl, data.v_est_kmh);
}
