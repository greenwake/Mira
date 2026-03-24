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
            TelemetryData tData;

            double v_est = 0;
            if (monitoring.contains("odometer")) {
                v_est = (monitoring["odometer"].toObject()["v_est"].toDouble() / 100.0) * 3.6;
                tData.v_est_kmh = v_est;
            }

            double solr_d = 0, max_safe_d = 0;
            if (monitoring.contains("trainPosition")) {
                QJsonObject tp = monitoring["trainPosition"].toObject();
                if (tp.contains("solr_ref")) solr_d = tp["solr_ref"].toObject()["d_est_front_end"].toDouble() / 100.0;
                if (tp.contains("max_safe_front_end")) max_safe_d = tp["max_safe_front_end"].toDouble() / 100.0;

                if (solr_d > 0) { tData.hasEoa = true; tData.dEoa = solr_d; }
                if (max_safe_d > 0) { tData.hasSvl = true; tData.dSvl = max_safe_d; }
            }

            if (monitoring.contains("speedAndDistance")) {
                QJsonObject sAndD = monitoring["speedAndDistance"].toObject();
                tData.hasCurves = true;

                auto getD = [&](const QString& k) { return sAndD.contains(k) ? sAndD[k].toDouble() / 100.0 : 0.0; };
                auto getV = [&](const QString& k) { return sAndD.contains(k) ? (sAndD[k].toDouble() / 100.0) * 3.6 : 0.0; };

                double dSbi1 = getD("dSbi1"), dSbi2 = getD("dSbi2"), dEbi = getD("dEbi");
                double dWarn = getD("dWarning"), dPerm = getD("dPermitted"), dInd = getD("dIndication");

                double vSbi = getV("vSbi"), vEbi = getV("vEbi"), vWarn = getV("vWarning"), vPerm = getV("vPermitted");
                double vInd = getV("vIndication");

                // X Ekseni
                tData.x_sbi1 = (dSbi1 != 0) ? dSbi1 : ((solr_d != 0) ? solr_d : (10000.0 - max_safe_d));
                tData.x_sbi2 = (dSbi2 != 0) ? dSbi2 : ((max_safe_d != 0) ? max_safe_d : (10000.0 - max_safe_d));
                tData.x_ebi = (dEbi != 0) ? dEbi : ((max_safe_d != 0) ? max_safe_d : (10000.0 - max_safe_d));
                tData.x_warning = (dWarn != 0) ? dWarn : ((max_safe_d != 0) ? max_safe_d : (10000.0 - max_safe_d));
                tData.x_permitted = (dPerm != 0) ? dPerm : ((max_safe_d != 0) ? max_safe_d : (10000.0 - max_safe_d));
                tData.x_indication = (dInd != 0) ? dInd : ((max_safe_d != 0) ? max_safe_d : (10000.0 - max_safe_d));

                // Y Ekseni
                tData.y_sbi1 = (vSbi != 0) ? vSbi : v_est;
                tData.y_sbi2 = (vSbi != 0) ? vSbi : v_est;
                tData.y_ebi = (vEbi != 0) ? vEbi : v_est;
                tData.y_warning = (vWarn != 0) ? vWarn : v_est;
                tData.y_permitted = (vPerm != 0) ? vPerm : v_est;
                tData.y_indication = (vInd != 0) ? vInd : v_est;
            }

            emit telemetryReceived(tData);
        }
    }
}
