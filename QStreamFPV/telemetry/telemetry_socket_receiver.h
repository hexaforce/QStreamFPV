#ifndef TELEMETRY_SOCKET_H
#define TELEMETRY_SOCKET_H

#include <QDebug>
#include <QObject>
#include <QUdpSocket>

class TelemetrySocket : public QObject {
  Q_OBJECT
public:
  explicit TelemetrySocket(QObject *parent = nullptr);
  ~TelemetrySocket();

  Q_INVOKABLE void sendMessage(const QString &message);

public slots:

signals:
  void messageReceived(const QString &message);

private slots:
  void processPendingDatagrams();

private:
  QUdpSocket *udpSocket;
};

#endif // TELEMETRY_SOCKET_H
