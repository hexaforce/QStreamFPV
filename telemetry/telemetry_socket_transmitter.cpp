#include "telemetry_socket_transmitter.h"

TelemetrySocket::TelemetrySocket(QObject *parent)
    : QObject(parent)
    , udpSocket(new QUdpSocket(this))
    , m_streamer(nullptr) {
  int telemetryPort = GlobalContext::instance().telemetryPort;
  if (!udpSocket->bind(QHostAddress::Any, telemetryPort)) {
    g_printerr(QString("Failed to bind to port: %1 %2\n").arg(telemetryPort).arg(udpSocket->errorString()).toUtf8().constData());
    return;
  }
  connect(udpSocket, &QUdpSocket::readyRead, this, &TelemetrySocket::process_pending_datagrams);
  g_print(QString("Listen telemetry port: %1\n").arg(telemetryPort).toUtf8().constData());

  if (GlobalContext::instance().networkMode == NetworkMode::AccessPointMode && GlobalContext::instance().wifiAccessPoint == WifiAccessPoint::Receiver) {
    notify_address();
  }
}

TelemetrySocket::~TelemetrySocket() {
  delete udpSocket;
}

// Notification IPAddress
// ----------------------------------------------------------------------------
void TelemetrySocket::notify_address() {
  if (!m_timer) {
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, [this]() {
      if (!GlobalContext::instance().acceptConnect) {
        send_message("IPAddress:" + GlobalContext::instance().myIpAddress().toString());
      } else {
        m_timer->stop();
      }
    });
    m_timer->start(100);
  }
}

// Send Message
// ----------------------------------------------------------------------------
void TelemetrySocket::send_message(const QString &message) {
  dump_message("Send", GlobalContext::instance().targetIpAddress(), GlobalContext::instance().telemetryPort, message);
  udpSocket->writeDatagram(                        //
      message.toUtf8(),                            //
      GlobalContext::instance().targetIpAddress(), //
      GlobalContext::instance().telemetryPort      //
  );
}

// Process Pending Datagrams
// ----------------------------------------------------------------------------
void TelemetrySocket::process_pending_datagrams() {
  auto &ctx = GlobalContext::instance();

  while (udpSocket->hasPendingDatagrams()) {

    QByteArray datagram;
    datagram.resize(int(udpSocket->pendingDatagramSize()));
    QHostAddress sender;
    quint16 senderPort;
    udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
    QString message = QString::fromUtf8(datagram);
    dump_message("Receive", sender, senderPort, message);

    if (message.contains("IPAddress:")) {
      ctx.setTargetIpAddress(QHostAddress(message.split(':')[1]));
      send_message("AcceptIPAddress");
      streamer()->start_streaming();
      ctx.acceptConnect = true;
    } else if (message == "AcceptIPAddress") {
      ctx.acceptConnect = true;
    } else if (message == "VideoPipelineInfo") {
      send_message(message + streamer()->get_video_pipeline_info());
    } else if (message == "AudioPipelineInfo") {
      if (ctx.enableAudio) {
        send_message(message + streamer()->get_audio_pipeline_info());
      }
    } else if (datagram == "NetworkInterfaceInfo") {
      if (ctx.interfaceType == "Ethernet") {
        send_message(message + ctx.cmd->run("sudo ethtool " + ctx.networkInterface));
      } else if (ctx.interfaceType == "WiFi") {
        // commandRunner.runCommand("iw phy$(iw dev " + ctx.networkInterface + " info | grep wiphy | awk '{print $2}') info");
        send_message(message + ctx.cmd->run("iw dev " + ctx.networkInterface + " info"));
      }
    } else if (datagram == "Video/Source" || datagram == "Audio/Source" || datagram == "Audio/Sink") {
      send_message(datagram + list_devices(datagram));
    } else if (message == "V4l2CtlInfo") {
      send_message(message + ctx.cmd->run("v4l2-ctl --info"));
    } else if (message == "V4l2CtlListFormats") {
      send_message(message + ctx.cmd->run("v4l2-ctl --list-formats"));
    } else if (message == "V4l2CtlListFormatsExt") {
      send_message(message + ctx.cmd->run("v4l2-ctl --list-formats-ext"));
    } else if (message == "V4l2CtlListCtrls") {
      send_message(message + ctx.cmd->run("v4l2-ctl --list-ctrls"));
    } else {
      streamer()->handle_telemetry_message(message);
    }
  }
}

// Dump Message
// ----------------------------------------------------------------------------
void TelemetrySocket::dump_message(QString type, QHostAddress address, quint16 port, QString message) {
  g_print(QString("%1 | %2:%3 | %4\n").arg(type).arg(address.toString()).arg(port).arg(message).toUtf8().constData());
}

// Set/Get Streamer
// ----------------------------------------------------------------------------
Streamer *TelemetrySocket::streamer() const {
  return m_streamer;
}
void TelemetrySocket::setStreamer(Streamer *streamer) {
  m_streamer = streamer;
}

// GstDeviceMonitor device list
// ----------------------------------------------------------------------------
QString TelemetrySocket::list_devices(const QString &deviceType) {
  GstDeviceMonitor *deviceMonitor = gst_device_monitor_new();
  gst_device_monitor_add_filter(deviceMonitor, deviceType.toStdString().c_str(), nullptr);
  gst_device_monitor_start(deviceMonitor);
  GList *devices = gst_device_monitor_get_devices(deviceMonitor);
  QJsonArray devicesArray;
  for (GList *l = devices; l != nullptr; l = l->next) {
    GstDevice *device = GST_DEVICE(l->data);
    devicesArray.append(device_to_json(device));
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
QJsonObject TelemetrySocket::device_to_json(GstDevice *device) {
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
    deviceJson["caps"] = caps_to_json(caps);
    gst_caps_unref(caps);
  }
  return deviceJson;
}

// GstDeviceMonitor caps
// ----------------------------------------------------------------------------
QJsonArray TelemetrySocket::caps_to_json(GstCaps *caps) {
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
