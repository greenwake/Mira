#pragma once
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QMetaType>

typedef enum
{
    EVC_MODE_FULL_SUPERVISION,
    EVC_MODE_ON_SIGHT,
    EVC_MODE_STAFF_RESPONSIBLE,
    EVC_MODE_SHUNTING,
    EVC_MODE_UNFITTED,
    EVC_MODE_SLEEPING,
    EVC_MODE_STANDBY,
    EVC_MODE_TRIP,
    EVC_MODE_POST_TRIP,
    EVC_MODE_SYSTEM_FAILURE,
    EVC_MODE_ISOLATION,
    EVC_MODE_NON_LEADING,
    EVC_MODE_LIMITED_SUPERVISION,
    EVC_MODE_NATIONAL_SYSTEM,
    EVC_MODE_REVERSING,
    EVC_MODE_PASSIVE_SHUNTING
} ertms_mode_t;

typedef enum
{
    M_LEVEL_0  = 0,
    M_LEVEL_NTC,
    M_LEVEL_1,
    M_LEVEL_2 ,
    M_LEVEL_SPAR_START,
    M_LEVEL_MAX
} ertms_mLevel_t;

struct TelemetryData {
    double v_est_kmh = 0.0;
    double trainMaxSpeed = 50.0;
    int currentLevel = 4;
    QString currentModeText = "";
    QString currentLevelText= "";

    bool hasCurves = false;

    double x_sbi1 = 0.0, y_sbi1 = 0.0;
    double x_sbi2 = 0.0, y_sbi2 = 0.0;
    double x_ebi = 0.0, y_ebi = 0.0;
    double x_warning = 0.0, y_warning = 0.0;
    double x_permitted = 0.0, y_permitted = 0.0;
    double x_indication = 0.0, y_indication = 0.0;

    double distanceSinceStart = 0.0;
    double default_distance = 2500.0;

    bool hasEoa = false;
    double dEoa = 0.0;

    bool hasSvl = false;
    double dSvl = 0.0;
    bool monitoringStatus =false;

};

Q_DECLARE_METATYPE(TelemetryData)

class TelemetryClient : public QObject {
    Q_OBJECT
public:
    explicit TelemetryClient(QObject *parent = nullptr);
    void connectToServer(const QString& ip, quint16 port);
    void broadcastData(const QByteArray &data);

signals:
    void telemetryReceived(TelemetryData *data);
    void rawDataReceived(const QByteArray &data);

private slots:
    QString getModeName(int mode);
    QString getLevelName(int lvl);
    void onReadyRead();
    void onConnected();
    void onDisconnected();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);
    void attemptReconnect();

private:
    QTcpServer *m_server;
    QList<QTcpSocket*> m_clients;
    QTcpSocket* m_socket;
    TelemetryData *tData;
    QByteArray m_buffer;
    QTimer* m_reconnectTimer;
    QString m_serverIp;
    quint16 m_serverPort;
    int _currentLevel = 0;
    int _currentMode = 6;
    double _currentSpeed = 0.0;
    double _trainMaxSpeed = 0;
    double maDistance = 2500.0;

};
