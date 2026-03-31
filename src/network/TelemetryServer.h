#pragma once
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>

class TelemetryServer : public QObject {
    Q_OBJECT
public:
    explicit TelemetryServer(quint16 port, QObject *parent = nullptr);

public slots:
    void broadcastData(const QByteArray &data);

private slots:
    void onNewConnection();
    void onClientDisconnected();

private:
    QTcpServer *m_server;
    QList<QTcpSocket*> m_clients;
};
