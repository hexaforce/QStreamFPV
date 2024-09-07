
#include "pipeline_manager.h"
#include "telemetry_socket_receiver.h"
#include "wifi_manager.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWindow>

// Receiver main
// ----------------------------------------------------------------------------
int main(int argc, char *argv[]) {
  gst_init(&argc, &argv);

  int ret;

  {
    QGuiApplication app(argc, argv);
    app.setApplicationName("QStremeFPV");
    app.setApplicationVersion("0.0.2");
    app.setOrganizationName("FPV Japan");
    app.setOrganizationDomain("fpv.jp");

    auto &ctx = GlobalContext::instance();
    if (!ctx.initialize(ProcessMode::Receiver)) {
      return EXIT_FAILURE;
    }

    if (ctx.networkMode == NetworkMode::AccessPointMode) {
      if (ctx.wifiAccessPoint == WifiAccessPoint::Receiver) {
        launch_access_point();
      } else {
        connect_access_point();
      }
    }

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
    root->setContextProperty("commandRunner", ctx.cmd);

    PipelineManager videoPipelineManager;
    GstElement *pipeline = videoPipelineManager.pipeline();
    GstElement *sink = videoPipelineManager.setup_video_receive_pipeline();
    root->setContextProperty("videoPipelineManager", &videoPipelineManager);

    PipelineManager audioPipelineManager;
    audioPipelineManager.setup_audio_receive_pipeline();
    root->setContextProperty("audioPipelineManager", &audioPipelineManager);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    engine.load(url);

    QQuickItem *videoItem;
    QQuickWindow *rootObject;

    /* find and set the videoItem on the sink */
    rootObject = static_cast<QQuickWindow *>(engine.rootObjects().first());
    videoItem = rootObject->findChild<QQuickItem *>("videoItem");
    g_assert(videoItem);
    g_object_set(sink, "widget", videoItem, NULL);

    rootObject->scheduleRenderJob(videoPipelineManager.get_video_item_worker(), QQuickWindow::BeforeSynchronizingStage);

    ret = app.exec();

    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
  }

  gst_deinit();

  return ret;
}
