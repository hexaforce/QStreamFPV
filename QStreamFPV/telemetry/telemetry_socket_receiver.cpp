#include "telemetry_socket_receiver.h"
#include "global_context.h"

TelemetrySocket::TelemetrySocket(QObject *parent)
    : QObject(parent)
    , udpSocket(new QUdpSocket(this)) {
  auto &ctx = GlobalContext::instance();
  int receiverTelemetryPort = ctx.settings->loadIntParameter("receiver/listen_telemetry_port");
  if (!udpSocket->bind(QHostAddress::Any, receiverTelemetryPort)) {
    qCritical() << "Failed to bind to port" << receiverTelemetryPort << ":" << udpSocket->errorString();
    return;
  }
  connect(udpSocket, &QUdpSocket::readyRead, this, &TelemetrySocket::processPendingDatagrams);
  qInfo() << QString("Listen telemetry port: %1").arg(receiverTelemetryPort).toUtf8().constData();
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

    if (message.contains("IPAddress:")) {
      ctx.targetIpAddress = message.split(':')[1];
      sendMessage("AcceptIPAddress");
      // sendMessage("VideoPipelineInfo");
      // if (ctx.settings->loadBoolParameter("transmitter/enable_audio")) {
      //   sendMessage("AudioPipelineInfo");
      // }
    } else {
      emit messageReceived(message);
    }
  }
}

// Send Message
// ----------------------------------------------------------------------------
void TelemetrySocket::sendMessage(const QString &message) {
  auto &ctx = GlobalContext::instance();
  int transmitterTelemetryPort = ctx.settings->loadIntParameter("transmitter/listen_telemetry_port");
  udpSocket->writeDatagram(message.toUtf8(), QHostAddress(ctx.targetIpAddress), transmitterTelemetryPort);
}
