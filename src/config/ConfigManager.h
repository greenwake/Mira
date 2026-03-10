#pragma once
#include <QString>

class ConfigManager {
public:
    static ConfigManager& instance();
    void loadConfig(const QString& filePath);

    QString getServerIP() const;
    quint16 getServerPort() const;

private:
    ConfigManager() = default;
    QString m_serverIP;
    quint16 m_serverPort;
};