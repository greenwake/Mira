#include "ConfigManager.h"
#include <QSettings>

ConfigManager& ConfigManager::instance() {
    static ConfigManager instance;
    return instance;
}

void ConfigManager::loadConfig(const QString& filePath) {
    QSettings settings(filePath, QSettings::IniFormat);
    m_serverIP = settings.value("Network/ServerIP", "127.0.0.1").toString();
    m_serverPort = settings.value("Network/ServerPort", 3002).toUInt();
    qDebug() << "Data:" << m_serverIP << ":" << m_serverPort;
}

QString ConfigManager::getServerIP() const { return m_serverIP; }
quint16 ConfigManager::getServerPort() const { return m_serverPort; }
