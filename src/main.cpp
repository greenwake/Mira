#include <QApplication> // QGuiApplication yerine QApplication eklendi
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "MiraController.h"
#include "ConfigManager.h"

int main(int argc, char *argv[])
{
    // Qt Charts QML eklentisi QApplication gerektirir!
    QApplication app(argc, argv);

    // Konfigurasyonu yukle
    ConfigManager::instance().loadConfig("mira_config.ini");

    QQmlApplicationEngine engine;

    // MiraController nesnesini olustur ve QML baglamina ekle
    MiraController miraController;
    engine.rootContext()->setContextProperty("MiraController", &miraController);

    // QML dosyasını yükle (CMake'deki URI ile eşleşen yol)
    const QUrl url(QStringLiteral("qrc:/Mira/ui/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
                         if (!obj && url == objUrl)
                             QCoreApplication::exit(-1);
                     }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
