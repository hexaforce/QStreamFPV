#include "global_context.h"
#include "streamer.h"
#include "telemetry_socket_transmitter.h"
#include "wifi_manager.h"
#include <QCoreApplication>
#include <QTimer>

// Transmitter main
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

  QCoreApplication a(argc, argv);

  auto &ctx = GlobalContext::instance();
  ctx.mode = "Transmitter";

  ctx.systemInfo = new SystemInfo();
  ctx.commandRunner = new CommandRunner();
  ctx.settings = new SettingsManager("QStreamerFPV", "settings");
  ctx.type = ctx.systemInfo->getOSType();

  switch (ctx.type) {
  case OSType::RaspberryPi5:
  case OSType::RaspberryPi4:
  case OSType::JetsonNano:
    if (OSType::JetsonNano == ctx.type) {
      ctx.commandRunner->runCommand("sudo cat /etc/nv_tegra_release").toUtf8().constData();
    } else {
      ctx.commandRunner->runCommand("cat /proc/device-tree/model").toUtf8().constData();
    }
    [[fallthrough]];
  case OSType::Fedora40:
  case OSType::Ubuntu24:
  case OSType::MacBook:
    qInfo() << ctx.systemInfo->prettyProductName().toUtf8().constData();
    qInfo() << ctx.systemInfo->qtVersion().toUtf8().constData();
    break;
  case OSType::UNKNOWN:
  default:
    qCritical() << "Not supported";
    return EXIT_FAILURE;
  }

  if ("RouterMode" == ctx.settings->loadStringParameter("network/mode") || ctx.type == OSType::MacBook) {
    ctx.myIpAddress = ctx.settings->loadStringParameter("transmitter/ip_address");
    ctx.targetIpAddress = ctx.settings->loadStringParameter("receiver/ip_address");
    ctx.connectNetwork = true;
    ctx.networkInterface = ctx.systemInfo->getInterfaceNameFromIpAddress(ctx.myIpAddress);
    qInfo() << "targetIpAddress:" << ctx.targetIpAddress.toUtf8().constData();
  } else if ("AccessPointMode" == ctx.settings->loadStringParameter("network/mode")) {
    if ("Transmitter" == ctx.settings->loadStringParameter("wifi/access_point")) {
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

  TelemetrySocket socket;
  Streamer streamer(&socket);
  QObject::connect(&streamer, &Streamer::errorOccurred, [](const QString &error) { qCritical() << "Error:" << error; });
  streamer.startStreaming();

  return a.exec();
}
