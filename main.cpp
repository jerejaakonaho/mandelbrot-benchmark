#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "MandelbrotCPU.hpp"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    MandelbrotGenCPU cpuGen(-2.0, 0.47, -1.12, 1.12, 10000, 3840, 2160);
    engine.rootContext()->setContextProperty("cpuGenerator", &cpuGen);
    engine.loadFromModule("mandelbrot", "Main");

    return QGuiApplication::exec();
}
