#pragma once
#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QMetaType> // Struct'ı Qt'ye tanıtmak için gerekli

// YENİ: Veri Taşıma Objesi (DTO)
struct TelemetryData {
    double v_est_kmh = 0.0;

    // Eğri Verileri (Mesafe ve Hız)
    bool hasCurves = false;
    double dEbi = 0.0, vEbi = 0.0;
    double dPermitted = 0.0, vPermitted = 0.0;
    double dWarning = 0.0, vWarning = 0.0;
    double dSbi1 = 0.0, vSbi = 0.0;
    double dSbi2 = 0.0;
    double dIndication = 0.0;

    // Hedef Verileri
    bool hasEoa = false;
    double dEoa = 0.0;

    bool hasSvl = false;
    double dSvl = 0.0;
};

// Struct'ı Qt sinyal sistemine kaydediyoruz
Q_DECLARE_METATYPE(TelemetryData)

class TelemetryClient : public QObject {
    Q_OBJECT
public:
    explicit TelemetryClient(QObject *parent = nullptr);
    void connectToServer(const QString& ip, quint16 port);

signals:
    // ARTIK TEK BİR SİNYALİMİZ VAR: Tüm paketi tek seferde gönderir
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
