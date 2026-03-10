#pragma once
#include <QObject>
#include <QTcpSocket>

class TelemetryClient : public QObject {
    Q_OBJECT
public:
    explicit TelemetryClient(QObject *parent = nullptr);
    void connectToServer(const QString& ip, quint16 port);

signals:
    // JSON'dan parse edilen veriler bu sinyalle sisteme dağıtılır
    void ebiDataReceived(double distance, double speed);
    void permittedDataReceived(double distance, double speed);

private slots:
    void onReadyRead();
    void onReadyRead2();
    void onConnected();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket* m_socket;
};
