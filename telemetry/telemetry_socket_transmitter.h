#ifndef TELEMETRY_SOCKET_H
#define TELEMETRY_SOCKET_H

#include "global_context.h"
#include "streamer.h"
#include <QUdpSocket>

class Streamer;

class TelemetrySocket : public QObject {
  Q_OBJECT

public:
  TelemetrySocket(QObject *parent = nullptr);
  ~TelemetrySocket();

  void notify_address();
  void send_message(const QString &message);

  QString list_devices(const QString &deviceType);

  Streamer *streamer() const;
  void setStreamer(Streamer *streamer);

private slots:
  void process_pending_datagrams();

private:
  QTimer *m_timer = nullptr;
  QUdpSocket *udpSocket;
  Streamer *m_streamer;

  void dump_message(QString type, QHostAddress address, quint16 port, QString message);

  QJsonObject device_to_json(GstDevice *device);
  QJsonArray caps_to_json(GstCaps *caps);
};

#endif // TELEMETRY_SOCKET_H
