#pragma once
#include <QObject>
#include <QTcpSocket>
#include <QTimer>

class TelemetryClient : public QObject {
    Q_OBJECT
public:
    explicit TelemetryClient(QObject *parent = nullptr);
    void connectToServer(const QString& ip, quint16 port);

signals:
    void ebiDataReceived(double distance, double speed);
    void permittedDataReceived(double distance, double speed);
    void warningDataReceived(double distance, double speed);
    void sbi1DataReceived(double distance, double speed);
    void sbi2DataReceived(double distance, double speed);
    void indicationDataReceived(double distance, double speed);

    void eoaTargetReceived(double x_pos, double y_speed);
    void svlTargetReceived(double x_pos, double y_speed);

    // YENİ: Grafiği sıfırlama sinyali
    void curveResetTriggered();

private slots:
    void onReadyRead();
    void onConnected();
    void onDisconnected();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);
    void attemptReconnect();

private:
    QTcpSocket* m_socket;
    QByteArray m_buffer;
    QTimer* m_reconnectTimer;
    QString m_serverIp;
    quint16 m_serverPort;

    // YENİ: Bir önceki mesafeyi tutacağımız değişken
    double m_lastDistanceToTarget;
};
