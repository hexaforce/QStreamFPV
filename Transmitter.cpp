#include "global_context.h"
#include "streamer.h"
#include "telemetry_socket_transmitter.h"
#include "wifi_manager.h"
#include <QCoreApplication>

// Transmitter main
// ----------------------------------------------------------------------------
int main(int argc, char *argv[]) {
  gst_init(&argc, &argv);

  QCoreApplication a(argc, argv);

  auto &ctx = GlobalContext::instance();
  if (!ctx.initialize(ProcessMode::Transmitter)) {
    return EXIT_FAILURE;
  }

  if (ctx.networkMode == NetworkMode::AccessPointMode) {
    if (ctx.wifiAccessPoint == WifiAccessPoint::Transmitter) {
      launch_access_point();
    } else {
      connect_access_point();
    }
  }

  TelemetrySocket socket;
  Streamer streamer(&socket);
  QObject::connect(&streamer, &Streamer::errorOccurred, [](const QString &error) { qCritical() << "Error:" << error; });

  if (ctx.is_network_ready()) {
    streamer.start_streaming();
  }

  return a.exec();
}
