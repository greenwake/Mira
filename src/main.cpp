#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "MiraController.h"
#include "ConfigManager.h"

int main(int argc, char *argv[])
{

    qputenv("QT_QUICK_CONTROLS_STYLE", "Basic");


    QApplication app(argc, argv);


    ConfigManager::instance().loadConfig("mira_config.ini");

    QQmlApplicationEngine engine;


    MiraController miraController;
    engine.rootContext()->setContextProperty("MiraController", &miraController);


    const QUrl url(QStringLiteral("qrc:/Mira/ui/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
                         if (!obj && url == objUrl)
                             QCoreApplication::exit(-1);
                     }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
