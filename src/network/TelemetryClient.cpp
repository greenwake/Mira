#include "TelemetryClient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

TelemetryClient::TelemetryClient(QObject *parent) : QObject(parent) {
    m_socket = new QTcpSocket(this);
    m_reconnectTimer = new QTimer(this);

    connect(m_socket, &QTcpSocket::readyRead, this, &TelemetryClient::onReadyRead);
    connect(m_socket, &QTcpSocket::connected, this, &TelemetryClient::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &TelemetryClient::onDisconnected);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &TelemetryClient::onErrorOccurred);
    connect(m_reconnectTimer, &QTimer::timeout, this, &TelemetryClient::attemptReconnect);
}

void TelemetryClient::connectToServer(const QString& ip, quint16 port) {
    m_serverIp = ip;
    m_serverPort = port;
    attemptReconnect();
}

void TelemetryClient::attemptReconnect() {
    if (m_socket->state() != QAbstractSocket::ConnectedState && m_socket->state() != QAbstractSocket::ConnectingState) {
        m_socket->connectToHost(m_serverIp, m_serverPort);
    }
}

void TelemetryClient::onConnected() {
    qDebug() << "Sunucuya basariyla baglanildi!";
    m_reconnectTimer->stop();
}

void TelemetryClient::onDisconnected() {
    qDebug() << "Sunucu ile baglanti koptu. Yeniden baglanma bekleniyor...";
    m_reconnectTimer->start(3000);
}

void TelemetryClient::onErrorOccurred(QAbstractSocket::SocketError socketError) {
    if (!m_reconnectTimer->isActive()) m_reconnectTimer->start(3000);
}

void TelemetryClient::onReadyRead() {
    m_buffer.append(m_socket->readAll());

    while (m_buffer.contains("##")) {
        int separatorIndex = m_buffer.indexOf("##");
        QByteArray rawData = m_buffer.left(separatorIndex);
        m_buffer.remove(0, separatorIndex + 2);

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(rawData, &parseError);
        if (parseError.error != QJsonParseError::NoError) continue;

        QJsonObject root = doc.object();

        if (root.contains("monitoring")) {
            QJsonObject monitoring = root["monitoring"].toObject();

            // HER DÖNGÜDE YENİ BİR STRUCT OLUŞTURUYORUZ
            TelemetryData tData;

            if (monitoring.contains("odometer")) {
                tData.v_est_kmh = (monitoring["odometer"].toObject()["v_est"].toDouble() / 100.0) * 3.6;
            }

            if (monitoring.contains("speedAndDistance")) {
                QJsonObject sAndD = monitoring["speedAndDistance"].toObject();
                tData.hasCurves = true;

                // Struct içini dolduruyoruz
                if (sAndD.contains("dEbi")) tData.dEbi = sAndD["dEbi"].toDouble() / 100.0;
                if (sAndD.contains("vEbi")) tData.vEbi = (sAndD["vEbi"].toDouble() / 100.0) * 3.6;

                if (sAndD.contains("dPermitted")) tData.dPermitted = sAndD["dPermitted"].toDouble() / 100.0;
                if (sAndD.contains("vPermitted")) tData.vPermitted = (sAndD["vPermitted"].toDouble() / 100.0) * 3.6;

                if (sAndD.contains("dWarning")) tData.dWarning = sAndD["dWarning"].toDouble() / 100.0;
                if (sAndD.contains("vWarning")) tData.vWarning = (sAndD["vWarning"].toDouble() / 100.0) * 3.6;

                if (sAndD.contains("dSbi1")) tData.dSbi1 = sAndD["dSbi1"].toDouble() / 100.0;
                if (sAndD.contains("dSbi2")) tData.dSbi2 = sAndD["dSbi2"].toDouble() / 100.0;
                if (sAndD.contains("vSbi")) tData.vSbi = (sAndD["vSbi"].toDouble() / 100.0) * 3.6;

                if (sAndD.contains("dIndication")) tData.dIndication = sAndD["dIndication"].toDouble() / 100.0;
            }

            if (monitoring.contains("trainPosition")) {
                QJsonObject tp = monitoring["trainPosition"].toObject();

                if (tp.contains("solr_ref")) {
                    tData.hasEoa = true;
                    tData.dEoa = tp["solr_ref"].toObject()["d_est_front_end"].toDouble() / 100.0;
                }
                if (tp.contains("max_safe_front_end")) {
                    tData.hasSvl = true;
                    tData.dSvl = tp["max_safe_front_end"].toDouble() / 100.0;
                }
            }

            // STRUCT DOLDU! Şimdi tek seferde paketi fırlatıyoruz.
            emit telemetryReceived(tData);
        }
    }
}
