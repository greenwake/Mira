#include "TelemetryServer.h"
#include <QDebug>

TelemetryServer::TelemetryServer(quint16 port, QObject *parent) : QObject(parent) {
    m_server = new QTcpServer(this);

    connect(m_server, &QTcpServer::newConnection, this, &TelemetryServer::onNewConnection);

    if (m_server->listen(QHostAddress::Any, port)) {
        qDebug() << "TelemetryServer başlatıldı. Port:" << port;
    } else {
        qDebug() << "TelemetryServer başlatılamadı:" << m_server->errorString();
    }
}

void TelemetryServer::onNewConnection() {
    while (m_server->hasPendingConnections()) {
        QTcpSocket *clientSocket = m_server->nextPendingConnection();
        connect(clientSocket, &QTcpSocket::disconnected, this, &TelemetryServer::onClientDisconnected);

        m_clients.append(clientSocket);
        qDebug() << "TelemetryServer: Yeni Bağlantı:" << clientSocket->peerAddress().toString();
    }
}

void TelemetryServer::onClientDisconnected() {
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (clientSocket) {
        m_clients.removeOne(clientSocket);
        clientSocket->deleteLater();
        qDebug() << "TelemetryServer: Bir alıcının bağlantısı koptu.";
    }
}

void TelemetryServer::broadcastData(const QByteArray &data) {
    for (QTcpSocket *client : m_clients) {
        if (client->state() == QAbstractSocket::ConnectedState) {
            client->write(data);
        }
    }
}
