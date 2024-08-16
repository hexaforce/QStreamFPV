#include "telemetry_socket_transmitter.h"
#include "global_context.h"
#include <QHostAddress>

TelemetrySocket::TelemetrySocket(QObject *parent)
    : QObject(parent)
    , udpSocket(new QUdpSocket(this))
    , streamer(nullptr)
    , commandRunner(nullptr) {
  auto &ctx = GlobalContext::instance();
  int transmitterTelemetryPort = ctx.settings->loadIntParameter("transmitter/listen_telemetry_port");
  if (!udpSocket->bind(QHostAddress::Any, transmitterTelemetryPort)) {
    qCritical() << "Failed to bind to port" << transmitterTelemetryPort << ":" << udpSocket->errorString();
    return;
  }
  udpSocket->bind(QHostAddress::Any, transmitterTelemetryPort);
  connect(udpSocket, &QUdpSocket::readyRead, this, &TelemetrySocket::processPendingDatagrams);
  qInfo() << QString("Listen telemetry port: %1").arg(transmitterTelemetryPort).toUtf8().constData();
}

TelemetrySocket::~TelemetrySocket() {
  delete udpSocket;
}

// Process Pending Datagrams
// ----------------------------------------------------------------------------
void TelemetrySocket::processPendingDatagrams() {
  auto &ctx = GlobalContext::instance();

  while (udpSocket->hasPendingDatagrams()) {

    QByteArray datagram;
    datagram.resize(int(udpSocket->pendingDatagramSize()));
    QHostAddress sender;
    quint16 senderPort;
    udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
    QString message = QString::fromUtf8(datagram);

    if (message == "AcceptIPAddress") {
      ctx.connectNetwork = true;
    } else if (message == "VideoPipelineInfo") {
      sendMessage(message + streamer->getVideoPipelineInfo());
    } else if (message == "AudioPipelineInfo") {
      if (ctx.settings->loadBoolParameter("transmitter/enable_audio")) {
        sendMessage(message + streamer->getAudioPipelineInfo());
      }
    } else if (datagram == "NetworkInterfaceInfo") {
      if (ctx.interfaceType == "Ethernet") {
        sendMessage(message + commandRunner->runCommand("sudo ethtool " + ctx.networkInterface));
      } else if (ctx.interfaceType == "WiFi") {
        // commandRunner.runCommand("iw phy$(iw dev " + ctx.networkInterface + " info | grep wiphy | awk '{print $2}') info");
        sendMessage(message + commandRunner->runCommand("iw dev " + ctx.networkInterface + " info"));
      }
    } else if (datagram == "Video/Source" || datagram == "Audio/Source" || datagram == "Audio/Sink") {
      sendMessage(datagram + listDevices(datagram));
    } else if (message == "V4l2CtlInfo") {
      sendMessage(message + commandRunner->runCommand("v4l2-ctl --info"));
    } else if (message == "V4l2CtlListFormats") {
      sendMessage(message + commandRunner->runCommand("v4l2-ctl --list-formats"));
    } else if (message == "V4l2CtlListFormatsExt") {
      sendMessage(message + commandRunner->runCommand("v4l2-ctl --list-formats-ext"));
    } else if (message == "V4l2CtlListCtrls") {
      sendMessage(message + commandRunner->runCommand("v4l2-ctl --list-ctrls"));
    } else {
      streamer->handleTelemetryMessage(message);
    }
  }
}

// Send Message
// ----------------------------------------------------------------------------
void TelemetrySocket::sendMessage(const QString &message) {
  auto &ctx = GlobalContext::instance();
  int receiverTelemetryPort = ctx.settings->loadIntParameter("receiver/listen_telemetry_port");
  udpSocket->writeDatagram(message.toUtf8(), QHostAddress(ctx.targetIpAddress), receiverTelemetryPort);
}

// Set Streamer
// ----------------------------------------------------------------------------
void TelemetrySocket::setStreamer(Streamer *streamer) {
  this->streamer = streamer;
}

// GstDeviceMonitor device list
// ----------------------------------------------------------------------------
QString TelemetrySocket::listDevices(const QString &deviceType) {
  GstDeviceMonitor *deviceMonitor = gst_device_monitor_new();
  gst_device_monitor_add_filter(deviceMonitor, deviceType.toStdString().c_str(), nullptr);
  gst_device_monitor_start(deviceMonitor);
  GList *devices = gst_device_monitor_get_devices(deviceMonitor);
  QJsonArray devicesArray;
  for (GList *l = devices; l != nullptr; l = l->next) {
    GstDevice *device = GST_DEVICE(l->data);
    devicesArray.append(deviceToJson(device));
    gst_object_unref(device);
  }
  g_list_free(devices);
  gst_device_monitor_stop(deviceMonitor);
  gst_object_unref(deviceMonitor);
  QJsonDocument doc(devicesArray);
  return QString::fromUtf8(doc.toJson(QJsonDocument::Indented));
}

// GstDeviceMonitor device
// ----------------------------------------------------------------------------
QJsonObject TelemetrySocket::deviceToJson(GstDevice *device) {
  QJsonObject deviceJson;
  const gchar *deviceName = gst_device_get_display_name(device);
  deviceJson["Device"] = QString::fromUtf8(deviceName);
  GstStructure *properties = gst_device_get_properties(device);
  if (properties) {
    const gchar *name;
    const GValue *value;
    for (gint i = 0; i < gst_structure_n_fields(properties); ++i) {
      name = gst_structure_nth_field_name(properties, i);
      value = gst_structure_get_value(properties, name);
      if (G_VALUE_HOLDS_STRING(value)) {
        deviceJson[QString::fromUtf8(name)] = QString::fromUtf8(g_value_get_string(value));
      } else if (G_VALUE_HOLDS_INT(value)) {
        deviceJson[QString::fromUtf8(name)] = g_value_get_int(value);
      } else if (G_VALUE_HOLDS_BOOLEAN(value)) {
        deviceJson[QString::fromUtf8(name)] = g_value_get_boolean(value);
      } else if (G_VALUE_HOLDS_FLOAT(value)) {
        deviceJson[QString::fromUtf8(name)] = g_value_get_float(value);
      } else {
        deviceJson[QString::fromUtf8(name)] = "Unsupported value type";
      }
    }
    gst_structure_free(properties);
  }
  GstCaps *caps = gst_device_get_caps(device);
  if (caps) {
    deviceJson["caps"] = capsToJson(caps);
    gst_caps_unref(caps);
  }
  return deviceJson;
}

// GstDeviceMonitor caps
// ----------------------------------------------------------------------------
QJsonArray TelemetrySocket::capsToJson(GstCaps *caps) {
  QJsonArray capsArray;
  for (guint i = 0; i < gst_caps_get_size(caps); ++i) {
    GstStructure *structure = gst_caps_get_structure(caps, i);
    QJsonObject capsObject;
    const gchar *mediaType = gst_structure_get_name(structure);
    capsObject["media"] = QString::fromUtf8(mediaType);
    const gchar *fieldName;
    const GValue *value;
    for (gint j = 0; j < gst_structure_n_fields(structure); ++j) {
      fieldName = gst_structure_nth_field_name(structure, j);
      value = gst_structure_get_value(structure, fieldName);
      if (G_VALUE_HOLDS_STRING(value)) {
        capsObject[QString::fromUtf8(fieldName)] = QString::fromUtf8(g_value_get_string(value));
      } else if (G_VALUE_HOLDS_INT(value)) {
        capsObject[QString::fromUtf8(fieldName)] = g_value_get_int(value);
      } else if (G_VALUE_HOLDS_BOOLEAN(value)) {
        capsObject[QString::fromUtf8(fieldName)] = g_value_get_boolean(value);
      } else if (G_VALUE_HOLDS_FLOAT(value)) {
        capsObject[QString::fromUtf8(fieldName)] = g_value_get_float(value);
      } else if (GST_VALUE_HOLDS_FRACTION(value)) {
        int numerator = gst_value_get_fraction_numerator(value);
        int denominator = gst_value_get_fraction_denominator(value);
        capsObject[QString::fromUtf8(fieldName)] = QString("%1/%2").arg(numerator).arg(denominator);
      } else if (GST_VALUE_HOLDS_LIST(value)) {
        QJsonArray listArray;
        for (guint k = 0; k < gst_value_list_get_size(value); ++k) {
          const GValue *listValue = gst_value_list_get_value(value, k);
          if (GST_VALUE_HOLDS_FRACTION(listValue)) {
            int numerator = gst_value_get_fraction_numerator(listValue);
            int denominator = gst_value_get_fraction_denominator(listValue);
            listArray.append(QString("%1/%2").arg(numerator).arg(denominator));
          } else if (G_VALUE_HOLDS_STRING(listValue)) {
            listArray.append(QString::fromUtf8(g_value_get_string(listValue)));
          }
        }
        capsObject[QString::fromUtf8(fieldName)] = listArray;
      }
    }
    capsArray.append(capsObject);
  }
  return capsArray;
}
