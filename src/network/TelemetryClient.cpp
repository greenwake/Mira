#include "TelemetryClient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

TelemetryClient::TelemetryClient(QObject *parent) : QObject(parent) {
    m_socket = new QTcpSocket(this);
    connect(m_socket, &QTcpSocket::readyRead, this, &TelemetryClient::onReadyRead);
    connect(m_socket, &QTcpSocket::connected, this, &TelemetryClient::onConnected);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &TelemetryClient::onErrorOccurred);
}

void TelemetryClient::connectToServer(const QString& ip, quint16 port) {
    qDebug() << "Baglaniliyor:" << ip << ":" << port;
    m_socket->connectToHost(ip, port);
}

void TelemetryClient::onConnected() {
    qDebug() << "Sunucuya baglanildi!";
}

void TelemetryClient::onErrorOccurred(QAbstractSocket::SocketError socketError) {
    qDebug() << "Soket Hatasi:" << socketError;
}

void TelemetryClient::onReadyRead2() {
    QByteArray data = m_socket->readAll();
    qDebug() << "QByteArray:" << data;
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error == QJsonParseError::NoError && doc.isObject()) {
        QJsonObject root = doc.object();
        
        if (root.contains("speedAndDistance")) {
            QJsonObject sAndD = root["speedAndDistance"].toObject();
            
            // Verisetinden gerekli alanlari cekiyoruz
            if (sAndD.contains("dEbi") && sAndD.contains("vEbi")) {
                emit ebiDataReceived(sAndD["dEbi"].toDouble(), sAndD["vEbi"].toDouble()); // [cite: 1, 2]
            }
            
            if (sAndD.contains("dPermitted") && sAndD.contains("vPermitted")) {
                emit permittedDataReceived(sAndD["dPermitted"].toDouble(), sAndD["vPermitted"].toDouble()); // [cite: 1, 2]
            }
        }
    }
}

void TelemetryClient::onReadyRead() {
    // Mock server'dan satır satır (\n) okuyoruz
    while (m_socket->canReadLine()) {
        QByteArray data = m_socket->readLine();
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

        if (parseError.error == QJsonParseError::NoError && doc.isObject()) {
            QJsonObject root = doc.object();

            if (root.contains("speedAndDistance")) {
                QJsonObject sAndD = root["speedAndDistance"].toObject();

                // === EBI VERİSİ ===
                if (sAndD.contains("dEbi") && sAndD.contains("vEbi")) {
                    double dEbi_cm = sAndD["dEbi"].toDouble();
                    double vEbi_cms = sAndD["vEbi"].toDouble();

                    // Dönüşümler: cm -> Metre, cm/s -> km/h
                    double dEbi_m = dEbi_cm / 100.0;
                    double vEbi_kmh = (vEbi_cms / 100.0) * 3.6;

                    emit ebiDataReceived(dEbi_m, vEbi_kmh);
                }

                // === PERMITTED VERİSİ ===
                if (sAndD.contains("dPermitted") && sAndD.contains("vPermitted")) {
                    double dPermitted_cm = sAndD["dPermitted"].toDouble();
                    double vPermitted_cms = sAndD["vPermitted"].toDouble();

                    // Dönüşümler: cm -> Metre, cm/s -> km/h
                    double dPermitted_m = dPermitted_cm / 100.0;
                    double vPermitted_kmh = (vPermitted_cms / 100.0) * 3.6;

                    emit permittedDataReceived(dPermitted_m, vPermitted_kmh);
                }
            }
        }
    }
}
