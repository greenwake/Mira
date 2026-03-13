#include "TelemetryClient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

// Constructor içinde m_lastDistanceToTarget başlangıç değeri -1 yapıldı
TelemetryClient::TelemetryClient(QObject *parent)
    : QObject(parent), m_lastDistanceToTarget(-1.0)
{
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
        qDebug() << "Sunucuya baglanilmaya calisiliyor:" << m_serverIp << ":" << m_serverPort;
        m_socket->connectToHost(m_serverIp, m_serverPort);
    }
}

void TelemetryClient::onConnected() {
    qDebug() << "Sunucuya basariyla baglanildi!";
    m_lastDistanceToTarget = -1.0; // Bağlantı yenilendiğinde mesafeyi sıfırla
    m_reconnectTimer->stop();
}

void TelemetryClient::onDisconnected() {
    qDebug() << "Sunucu ile baglanti koptu. Yeniden baglanma bekleniyor...";
    m_reconnectTimer->start(3000);
}

void TelemetryClient::onErrorOccurred(QAbstractSocket::SocketError socketError) {
    qDebug() << "Soket Hatasi:" << socketError;
    if (!m_reconnectTimer->isActive()) {
        m_reconnectTimer->start(3000);
    }
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

            double v_est_kmh = 0;
            if (monitoring.contains("odometer")) {
                v_est_kmh = (monitoring["odometer"].toObject()["v_est"].toDouble() / 100.0) * 3.6;
            }

            if (monitoring.contains("speedAndDistance")) {
                QJsonObject sAndD = monitoring["speedAndDistance"].toObject();

                // YENİ EĞRİ KONTROLÜ (MA Uzatması / Yeni Hedef)
                if (sAndD.contains("dEbi")) {
                    double currentDistance_m = sAndD["dEbi"].toDouble() / 100.0;

                    // Eğer yeni mesafe, eskisinden 10 metreden daha fazlaysa (tren geriye gitmeyeceğine göre yeni eğri gelmiştir)
                    if (m_lastDistanceToTarget != -1.0 && currentDistance_m > (m_lastDistanceToTarget + 10.0)) {
                        qDebug() << "Yeni Fren Egrisi Algilandi! Mesafe sictamasi:" << m_lastDistanceToTarget << "->\n" << currentDistance_m;
                        emit curveResetTriggered(); // Controller'a sıfırla komutu ver
                    }
                    m_lastDistanceToTarget = currentDistance_m; // Yeni değeri hafızaya al
                }

                auto emitCurve = [&](const QString& dKey, const QString& vKey, void (TelemetryClient::*signal)(double, double)) {
                    if (sAndD.contains(dKey) && sAndD.contains(vKey)) {
                        double d_m = sAndD[dKey].toDouble() / 100.0;
                        double v_kmh = (sAndD[vKey].toDouble() / 100.0) * 3.6;
                        emit (this->*signal)(d_m, v_kmh);
                    }
                };

                emitCurve("dEbi", "vEbi", &TelemetryClient::ebiDataReceived);
                emitCurve("dPermitted", "vPermitted", &TelemetryClient::permittedDataReceived);
                emitCurve("dWarning", "vWarning", &TelemetryClient::warningDataReceived);
                emitCurve("dSbi1", "vSbi", &TelemetryClient::sbi1DataReceived);
                emitCurve("dSbi2", "vSbi", &TelemetryClient::sbi2DataReceived);

                if (sAndD.contains("dIndication")) {
                    double dIndication_m = sAndD["dIndication"].toDouble() / 100.0;
                    emit indicationDataReceived(dIndication_m, v_est_kmh);
                }
            }

            if (monitoring.contains("trainPosition")) {
                QJsonObject tp = monitoring["trainPosition"].toObject();

                if (tp.contains("solr_ref")) {
                    double solr_d = tp["solr_ref"].toObject()["d_est_front_end"].toDouble() / 100.0;
                    emit eoaTargetReceived(solr_d, v_est_kmh);
                }

                if (tp.contains("max_safe_front_end")) {
                    double max_safe_d = tp["max_safe_front_end"].toDouble() / 100.0;
                    emit svlTargetReceived(max_safe_d, v_est_kmh);
                }
            }
        }
    }
}
