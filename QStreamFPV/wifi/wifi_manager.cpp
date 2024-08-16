#include "wifi_manager.h"
#include "command_runner.h"
#include <QFile>
#include <QThread>

// Launch Up Access Point
// ----------------------------------------------------------------------------
void launch_access_point() {
  auto &ctx = GlobalContext::instance();

  QString wifi_ssid = ctx.settings->loadStringParameter("wifi/ssid");
  QString wifi_passphrase = ctx.settings->loadStringParameter("wifi/passphrase");

  if (ctx.mode == "Transmitter") {
    ctx.networkInterface = ctx.settings->loadStringParameter("transmitter/network_interface");
  } else if (ctx.mode == "Receiver") {
    ctx.networkInterface = ctx.settings->loadStringParameter("receiver/network_interface");
  }

  ctx.commandRunner->runCommand("udevadm info --query=all --path=/sys/class/net/" + ctx.networkInterface + " | grep -E 'ID_BUS|ID_OUI_FROM_DATABASE|ID_VENDOR_FROM_DATABASE|ID_MODEL_FROM_DATABASE'");

  QFile file(":/QStreamFPV/wifi/hostapd.conf");
  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream in(&file);
    QString content = in.readAll();
    QFile outFile("hostapd.conf");
    if (outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
      QTextStream out(&outFile);
      out << content;
      outFile.close();
    }
  }

  QString access_point_ip_address = ctx.settings->loadStringParameter("wifi/access_point_ip_address");

  QString start_dhcp_range = ctx.settings->loadStringParameter("wifi/start_dhcp_range");
  QString end_dhcp_range = ctx.settings->loadStringParameter("wifi/end_dhcp_range");
  QString subnet_mask = ctx.settings->loadStringParameter("wifi/subnet_mask");

  int dnsmasq_port = ctx.settings->loadIntParameter("wifi/dnsmasq_port");

  ctx.commandRunner->runCommand("sudo iw reg set US");
  QString currentIpAddress = ctx.systemInfo->getIPAddress(ctx.networkInterface);
  if (currentIpAddress != access_point_ip_address) {
    ctx.commandRunner->runCommand("sudo ip link set " + ctx.networkInterface + " down");
    QThread::sleep(2);
    ctx.commandRunner->runCommand("sudo ip addr add " + access_point_ip_address + "/10 dev " + ctx.networkInterface);
    ctx.commandRunner->runCommand("sudo ip link set " + ctx.networkInterface + " up");
    QThread::sleep(2);
  }
  ctx.commandRunner->runCommand("ip addr show " + ctx.networkInterface);

  bool success = false;
  while (!success) {
    ctx.commandRunner->runCommand("sudo pkill hostapd");
    QThread::sleep(1);
    ctx.commandRunner->runCommand("sudo truncate -s 0 hostapd.log");
    ctx.commandRunner->runCommand("sudo hostapd hostapd.conf -f hostapd.log -B");
    QThread::sleep(1);
    QString result = ctx.commandRunner->runCommand("sudo cat hostapd.log");
    if (result.contains("AP-ENABLED", Qt::CaseInsensitive)) {
      success = true;
    } else {
      ctx.commandRunner->runCommand("sudo cat hostapd.conf");
      QThread::sleep(3);
    }
  }

  ctx.commandRunner->runCommand("sudo pkill dnsmasq");
  QThread::sleep(1);
  ctx.commandRunner->runCommand("sudo dnsmasq --interface=" + ctx.networkInterface + " --dhcp-range=" + start_dhcp_range + "," + end_dhcp_range + "," + subnet_mask + ",24h --port=" + QString::number(dnsmasq_port));
  QThread::sleep(1);
  ctx.commandRunner->runCommand("sudo netstat -tuln | grep " + QString::number(dnsmasq_port));

  ctx.myIpAddress = ctx.systemInfo->getIPAddress(ctx.networkInterface);
}

// Connect To Access Point
// ----------------------------------------------------------------------------
void connect_access_point() {
  auto &ctx = GlobalContext::instance();

  QString wifi_ssid = ctx.settings->loadStringParameter("wifi/ssid");
  QString wifi_passphrase = ctx.settings->loadStringParameter("wifi/passphrase");

  if (ctx.mode == "Transmitter") {
    ctx.networkInterface = ctx.settings->loadStringParameter("transmitter/network_interface");
  } else if (ctx.mode == "Receiver") {
    ctx.networkInterface = ctx.settings->loadStringParameter("receiver/network_interface");
  }

  bool connected = false;

  ctx.commandRunner->runCommand("udevadm info --query=all --path=/sys/class/net/" + ctx.networkInterface + " | grep -E 'ID_BUS|ID_OUI_FROM_DATABASE|ID_VENDOR_FROM_DATABASE|ID_MODEL_FROM_DATABASE'");

  while (!connected) {
    QString result = ctx.commandRunner->runCommand("sudo nmcli device wifi connect " + wifi_ssid + " password " + wifi_passphrase + " ifname " + ctx.networkInterface);
    if (result.contains("successfully activated", Qt::CaseInsensitive)) {
      connected = true;
    } else {
      ctx.commandRunner->runCommand("sudo nmcli dev wifi rescan");
      ctx.commandRunner->runCommand("nmcli dev wifi");
      QThread::sleep(3);
    }
  }

  ctx.myIpAddress = ctx.systemInfo->getIPAddress(ctx.networkInterface);
  ctx.targetIpAddress = ctx.settings->loadStringParameter("wifi/access_point_ip_address");
}
