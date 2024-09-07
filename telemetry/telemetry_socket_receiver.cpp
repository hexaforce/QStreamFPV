#include "telemetry_socket_receiver.h"

TelemetrySocket::TelemetrySocket(QObject *parent)
    : QObject(parent)
    , udpSocket(new QUdpSocket(this)) {
  int telemetryPort = GlobalContext::instance().telemetryPort;
  if (!udpSocket->bind(QHostAddress::Any, telemetryPort)) {
    g_printerr(QString("Failed to bind to port: %1 %2\n").arg(telemetryPort).arg(udpSocket->errorString()).toUtf8().constData());
    return;
  }
  connect(udpSocket, &QUdpSocket::readyRead, this, &TelemetrySocket::process_pending_datagrams);
  g_print(QString("Listen telemetry port: %1\n").arg(telemetryPort).toUtf8().constData());

  if (GlobalContext::instance().networkMode == NetworkMode::AccessPointMode && GlobalContext::instance().wifiAccessPoint == WifiAccessPoint::Transmitter) {
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
      ctx.acceptConnect = true;
    } else if (message == "AcceptIPAddress") {
      ctx.acceptConnect = true;
    } else {
      emit messageReceived(message);
    }
  }
}

// Dump Message
// ----------------------------------------------------------------------------
void TelemetrySocket::dump_message(QString type, QHostAddress address, quint16 port, QString message) {
  g_print(QString("%1 | %2:%3 | %4\n").arg(type).arg(address.toString()).arg(port).arg(message).toUtf8().constData());
}
