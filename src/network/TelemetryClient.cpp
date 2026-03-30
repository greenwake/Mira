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

void TelemetryClient::printer()
{

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

QString TelemetryClient::getModeName(int mode) {
    switch (mode) {
    case EVC_MODE_FULL_SUPERVISION:      return "Full Supervision";
    case EVC_MODE_ON_SIGHT:              return "On Sight";
    case EVC_MODE_STAFF_RESPONSIBLE:     return "Staff Responsible";
    case EVC_MODE_SHUNTING:              return "Shunting";
    case EVC_MODE_UNFITTED:              return "Unfitted";
    case EVC_MODE_SLEEPING:              return "Sleeping";
    case EVC_MODE_STANDBY:               return "Standby";
    case EVC_MODE_TRIP:                  return "Trip";
    case EVC_MODE_POST_TRIP:             return "Post Trip";
    case EVC_MODE_SYSTEM_FAILURE:        return "System Failure";
    case EVC_MODE_ISOLATION:             return "Isolation";
    case EVC_MODE_NON_LEADING:           return "Non Leading";
    case EVC_MODE_LIMITED_SUPERVISION:   return "Limited\nSupervision";
    case EVC_MODE_NATIONAL_SYSTEM:       return "National System";
    case EVC_MODE_REVERSING:             return "Reversing";
    case EVC_MODE_PASSIVE_SHUNTING:      return "Passive Shunting";
    default:
        return "Unknown Mode (" + QString::number(mode) + ")";
    }
}

QString TelemetryClient::getLevelName(int lvl)
{
    switch (lvl) {
    case M_LEVEL_0:         return "Level 0";
    case M_LEVEL_NTC:       return "Level NTC";
    case M_LEVEL_1:         return "Level 1";
    case M_LEVEL_2:         return "Level 2";
    default:
        return "Level 0";
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
        tData = new TelemetryData();
        tData->currentLevelText = getLevelName(_currentLevel);
        tData->currentModeText = getModeName(_currentMode);
        tData->trainMaxSpeed = _trainMaxSpeed;
        tData->v_est_kmh = _currentSpeed;

        if (root.contains("monitoring")) {
            QJsonObject monitoring = root["monitoring"].toObject();
            if (monitoring.contains("Level")) {
                int mLevel = monitoring["Level"].toInt();
                _currentLevel = mLevel;
            }
            if (monitoring.contains("Mode")) {
                int modeValue = monitoring["Mode"].toInt();
                _currentMode = modeValue;
            }
            if (monitoring.contains("TrainSpeed")) {
               _trainMaxSpeed = monitoring["TrainSpeed"].toDouble();
            }
            if (monitoring.contains("MADistance")) {
                maDistance = (monitoring["MADistance"].toDouble() != 0) ? monitoring["MADistance"].toDouble() : 2500.0 ;
            }
            double v_est = 0;
            if (monitoring.contains("odometer")) {
                v_est = monitoring["odometer"].toObject()["v_est"].toDouble();
                _currentSpeed= v_est;
            }

            double solr_d = 0, max_safe_d = 0, distanceSinceStart = 0;
            if (monitoring.contains("trainPosition")) {
                QJsonObject tp = monitoring["trainPosition"].toObject();

                if (tp.contains("solr_ref")) solr_d = tp["solr_ref"].toObject()["d_est_front_end"].toDouble();
                if (tp.contains("max_safe_front_end")) max_safe_d = tp["max_safe_front_end"].toDouble();
                if (tp.contains("distanceSinceStart")) distanceSinceStart = tp["distanceSinceStart"].toDouble();
                tData->distanceSinceStart = distanceSinceStart;

                if (solr_d > 0) { tData->hasEoa = true; tData->dEoa = solr_d; }
                if (max_safe_d > 0) { tData->hasSvl = true; tData->dSvl = max_safe_d; }
            }

            if (monitoring.contains("speedAndDistance")) {
                QJsonObject sAndD = monitoring["speedAndDistance"].toObject();
                tData->hasCurves = true;
                bool monitoringStatus = false;
                if (sAndD.contains("monitoringStatus"))
                {
                    if(sAndD["monitoringStatus"].toInt()) monitoringStatus = true;
                }

                tData->monitoringStatus = monitoringStatus;

                // Lambda fonksiyonlarından çevrimler kaldırıldı. Gelen veri doğrudan double olarak alınıyor.
                auto getD = [&](const QString& k) { return sAndD.contains(k) ? sAndD[k].toDouble() : 0.0; };
                auto getV = [&](const QString& k) { return sAndD.contains(k) ? sAndD[k].toDouble() : 0.0; };

                double dSbi1 = getD("dSbi1"), dSbi2 = getD("dSbi2"), dEbi = getD("dEbi");
                double dWarn = getD("dWarning"), dPerm = getD("dPermitted"), dInd = getD("dIndication");

                double vSbi = getV("vSbi"), vEbi = getV("vEbi"), vWarn = getV("vWarning"), vPerm = getV("vPermitted");
                double vInd = getV("vIndication");

                // X Ekseni Yedekleme Mantıkları (Fallback)
                tData->x_sbi1 = (dSbi1 != 0) ? maDistance-dSbi1 : ((solr_d != 0) ? solr_d : (tData->default_distance - distanceSinceStart));
                tData->x_sbi2 = (dSbi2 != 0) ? maDistance-dSbi2 : (tData->default_distance - distanceSinceStart);
                tData->x_ebi = (dEbi != 0) ? maDistance-dEbi : ((tData->default_distance - distanceSinceStart));
                tData->x_warning = (dWarn != 0) ? maDistance-dWarn : ((tData->default_distance - distanceSinceStart));
                tData->x_permitted = (dPerm != 0) ? maDistance-dPerm : ((tData->default_distance - distanceSinceStart));
                tData->x_indication = (dInd != 0) ? maDistance-dInd : ((tData->default_distance - distanceSinceStart));

                tData->y_sbi1 = (vSbi != 0) ? vSbi : v_est;
                tData->y_sbi2 = (dSbi2 != 0) ? v_est : vSbi;
                tData->y_ebi = (dEbi != 0) ? v_est : vEbi;
                tData->y_warning = (dWarn != 0) ? v_est : vWarn;
                tData->y_permitted = (dPerm != 0) ? v_est : vPerm;
                tData->y_indication =  v_est;

                qDebug() << "#############################################\n";

                qDebug() << "Mode : " <<_currentMode ;
                qDebug() << "Level : " <<_currentLevel ;
                qDebug() << "monitoringStatus : " << monitoringStatus;
                qDebug() << "distanceSinceStart : " << distanceSinceStart;
                qDebug() << "dSbi1 : " << dSbi1 << " -- vSbi : "<<vSbi;
                qDebug() << "dSbi2 : " << dSbi2 << " -- vSbi : "<<vSbi;
                qDebug() << "dEbi : " << dEbi << " -- vEbi : "<<vEbi;
                qDebug() << "dWarng : " << dWarn << " -- vWarng : "<<vWarn;
                qDebug() << "dPerm : " << dPerm << " -- vPerm : "<<vPerm;
                qDebug() << "dInd : " << dInd << " -- vInd : "<<vInd;
                qDebug() << "\n#############################################\n";
            }

        }
        emit telemetryReceived(tData);
    }
}
