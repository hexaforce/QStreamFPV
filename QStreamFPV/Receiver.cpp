
#include "pipeline_manager.h"
#include "telemetry_socket_receiver.h"
#include "wifi_manager.h"
#include <QApplication>
#include <QDebug>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWindow>

// Receiver main
// ----------------------------------------------------------------------------
int main(int argc, char *argv[]) {
  gst_init(&argc, &argv);
  // GST_LEVEL_NONE = 0,
  // GST_LEVEL_ERROR = 1,
  // GST_LEVEL_WARNING = 2,
  // GST_LEVEL_FIXME = 3,
  // GST_LEVEL_INFO = 4,
  // GST_LEVEL_DEBUG = 5,
  gst_debug_set_default_threshold(GST_LEVEL_ERROR);

  int ret;

  {
    QGuiApplication app(argc, argv);
    app.setApplicationName("QStremeFPV");
    app.setApplicationVersion("0.0.1");
    app.setOrganizationName("FPV Japan");
    app.setOrganizationDomain("fpv.jp");

    auto &ctx = GlobalContext::instance();
    ctx.mode = "Receiver";

    ctx.systemInfo = new SystemInfo();
    ctx.commandRunner = new CommandRunner();
    ctx.settings = new SettingsManager("QStreamerFPV", "settings");
    ctx.type = ctx.systemInfo->getOSType();
    switch (ctx.type) {
    case OSType::Fedora40:
    case OSType::Ubuntu24:
    case OSType::MacBook:
      qInfo() << ctx.systemInfo->prettyProductName().toUtf8().constData();
      qInfo() << ctx.systemInfo->qtVersion().toUtf8().constData();
      break;
    default:
      qCritical() << "Not supported";
      return EXIT_FAILURE;
    }

    if ("RouterMode" == ctx.settings->loadStringParameter("network/mode") || ctx.type == OSType::MacBook) {
      ctx.myIpAddress = ctx.settings->loadStringParameter("receiver/ip_address");
      ctx.targetIpAddress = ctx.settings->loadStringParameter("transmitter/ip_address");
      ctx.connectNetwork = true;
      ctx.networkInterface = ctx.systemInfo->getInterfaceNameFromIpAddress(ctx.myIpAddress);
      qInfo() << "targetIpAddress:" << ctx.targetIpAddress.toUtf8().constData();
    } else if ("AccessPointMode" == ctx.settings->loadStringParameter("network/mode")) {
      if ("Receiver" == ctx.settings->loadStringParameter("wifi/access_point")) {
        launch_access_point();
      } else {
        connect_access_point();
        qInfo() << "targetIpAddress:" << ctx.targetIpAddress.toUtf8().constData();
      }
    }

    ctx.interfaceType = ctx.systemInfo->getInterfaceTypeFromIpAddress(ctx.myIpAddress);

    qInfo() << "networkInterface:" << ctx.networkInterface.toUtf8().constData();
    qInfo() << "interfaceType:" << ctx.interfaceType.toUtf8().constData();
    qInfo() << "myIpAddress:" << ctx.myIpAddress.toUtf8().constData();

#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
#endif

    qmlRegisterType<TelemetrySocket>("TelemetrySocket", 1, 0, "TelemetrySocket");

    QQmlApplicationEngine engine;
    auto root = engine.rootContext();

    root->setContextProperty("networkInterface", ctx.networkInterface);
    root->setContextProperty("interfaceType", ctx.interfaceType);
    root->setContextProperty("systemInfo", ctx.systemInfo);
    root->setContextProperty("settings", ctx.settings);
    root->setContextProperty("commandRunner", ctx.commandRunner);

    PipelineManager videoPipelineManager;
    GstElement *pipeline = videoPipelineManager.pipeline();
    GstElement *sink = videoPipelineManager.setupVideoReceivePipeline();
    root->setContextProperty("videoPipelineManager", &videoPipelineManager);

    PipelineManager audioPipelineManager;
    audioPipelineManager.setupAudioReceivePipeline();
    root->setContextProperty("audioPipelineManager", &audioPipelineManager);

    const QUrl url(QStringLiteral("qrc:/QStreamFPV/main.qml"));
    engine.load(url);

    QQuickItem *videoItem;
    QQuickWindow *rootObject;

    /* find and set the videoItem on the sink */
    rootObject = static_cast<QQuickWindow *>(engine.rootObjects().first());
    videoItem = rootObject->findChild<QQuickItem *>("videoItem");
    g_assert(videoItem);
    g_object_set(sink, "widget", videoItem, NULL);

    rootObject->scheduleRenderJob(videoPipelineManager.getVideoItemWorker(), QQuickWindow::BeforeSynchronizingStage);

    ret = app.exec();

    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
  }

  gst_deinit();

  return ret;
}
