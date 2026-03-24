#pragma once
#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QMetaType>

struct TelemetryData {
    double v_est_kmh = 0.0;
    QString currentLevel = "Level 2";

    bool hasCurves = false;

    double x_sbi1 = 0.0, y_sbi1 = 0.0;
    double x_sbi2 = 0.0, y_sbi2 = 0.0;
    double x_ebi = 0.0, y_ebi = 0.0;
    double x_warning = 0.0, y_warning = 0.0;
    double x_permitted = 0.0, y_permitted = 0.0;
    double x_indication = 0.0, y_indication = 0.0;

    bool hasEoa = false;
    double dEoa = 0.0;

    bool hasSvl = false;
    double dSvl = 0.0;
};

Q_DECLARE_METATYPE(TelemetryData)

class TelemetryClient : public QObject {
    Q_OBJECT
public:
    explicit TelemetryClient(QObject *parent = nullptr);
    void connectToServer(const QString& ip, quint16 port);

signals:
    void telemetryReceived(TelemetryData data);

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
};
