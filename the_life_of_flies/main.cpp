#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QIcon>
#include "boardmodel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQuickStyle::setStyle("Imagine");

    app.setWindowIcon(QIcon(":/img/fly__1.png"));

    // регистрация модели, чтобы ее можно было использовать в qml
    qmlRegisterType<CBoardModel>("BoardModelURI", 1, 0, "BoardModel");

    QQmlApplicationEngine engine;
    engine.addImportPath(":/qml");
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {if (!obj && url == objUrl){QCoreApplication::exit(-1);}},
        Qt::QueuedConnection
                     );
    engine.load(url);

    return app.exec();
}
