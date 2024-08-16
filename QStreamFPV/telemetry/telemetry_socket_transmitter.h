#ifndef TELEMETRY_SOCKET_H
#define TELEMETRY_SOCKET_H

#include "command_runner.h"
#include "streamer.h"
#include <QDebug>
#include <QObject>
#include <QUdpSocket>

class Streamer;

class TelemetrySocket : public QObject {
  Q_OBJECT

public:
  TelemetrySocket(QObject *parent = nullptr);
  ~TelemetrySocket();

  void setStreamer(Streamer *streamer);
  void sendMessage(const QString &message);
  QString listDevices(const QString &deviceType);

private slots:
  void processPendingDatagrams();

private:
  QUdpSocket *udpSocket;
  Streamer *streamer;
  CommandRunner *commandRunner;

  QJsonObject deviceToJson(GstDevice *device);
  QJsonArray capsToJson(GstCaps *caps);
};

#endif // TELEMETRY_SOCKET_H
