#ifndef TELEMETRY_SOCKET_H
#define TELEMETRY_SOCKET_H

#include "global_context.h"
#include <QUdpSocket>

class TelemetrySocket : public QObject {
  Q_OBJECT
public:
  explicit TelemetrySocket(QObject *parent = nullptr);
  ~TelemetrySocket();

  void notify_address();
  Q_INVOKABLE void send_message(const QString &message);

public slots:

signals:
  void messageReceived(const QString &message);

private slots:
  void process_pending_datagrams();

private:
  QTimer *m_timer = nullptr;
  QUdpSocket *udpSocket;
  void dump_message(QString type, QHostAddress address, quint16 port, QString message);
};

#endif // TELEMETRY_SOCKET_H
