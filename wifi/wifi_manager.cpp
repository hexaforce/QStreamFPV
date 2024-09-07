#include "wifi_manager.h"
#include <QFile>

// Launch Up Access Point
// ----------------------------------------------------------------------------
void launch_access_point() {
  auto &ctx = GlobalContext::instance();
  // ctx.cmd->run("udevadm info --query=all --path=/sys/class/net/" + ctx.networkInterface + " | grep -E 'ID_BUS|ID_OUI_FROM_DATABASE|ID_VENDOR_FROM_DATABASE|ID_MODEL_FROM_DATABASE'");

  QString hostapd_conf = R"(
country_code=%1
driver=nl80211
ssid=%2
hw_mode=a
channel=%3
ieee80211n=1
ieee80211ac=1
wmm_enabled=1
ht_capab=[HT40][SHORT-GI-20][SHORT-GI-40]
macaddr_acl=0
auth_algs=1
ignore_broadcast_ssid=0
wpa=2
wpa_passphrase=%4
wpa_key_mgmt=WPA-PSK
wpa_pairwise=CCMP
)";

  ctx.cmd->run("sudo rm -f hostapd.conf");
  QFile outFile("hostapd.conf");
  if (outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream out(&outFile);
    out << hostapd_conf.arg(ctx.wifiRegionCode).arg(ctx.wifiSsid).arg(QString::number(ctx.wifiChannel)).arg(ctx.wifiPassphrase);
    outFile.close();
  } else {
    qDebug() << "Could not open hostapd.conf for writing";
  }

  QHostAddress currentIpAddress = ctx.systemInfo->get_ip_address(ctx.networkInterface);
  // if (currentIpAddress != access_point_ip_address) {
  ctx.cmd->run("sudo ip link set " + ctx.networkInterface + " down");
  QTimer::singleShot(2000, [&ctx]() {
    ctx.cmd->run("sudo iw reg set " + ctx.wifiRegionCode);
    ctx.cmd->run("sudo iw dev " + ctx.networkInterface + " set channel " + QString::number(ctx.wifiChannel));
    ctx.cmd->run("sudo ip addr add " + ctx.accessPointIpAddress.toString() + "/10 dev " + ctx.networkInterface);
    ctx.cmd->run("sudo ip link set " + ctx.networkInterface + " up");
    QTimer::singleShot(2000, [&ctx]() {
      // ctx.cmd->run("ip addr show " + ctx.networkInterface);
      ctx.cmd->run("sudo pkill hostapd");
      QTimer::singleShot(1000, [&ctx]() {
        // 'hostapd' Service Start
        // ----------------------------------------------------------------------------
        ctx.cmd->run("sudo rm -f hostapd.log");
        ctx.cmd->run("sudo hostapd -i " + ctx.networkInterface + " hostapd.conf -B > hostapd.log");
        QTimer::singleShot(1000, []() { check_hostapd_with_start_dnsmasq(); });
      });
    });
  });
  // }
}

// Activate DHCP Service
// ----------------------------------------------------------------------------
void check_hostapd_with_start_dnsmasq() {
  auto &ctx = GlobalContext::instance();
  QString result = ctx.cmd->run("sudo cat hostapd.log");
  if (result.contains("AP-ENABLED", Qt::CaseInsensitive)) {
    ctx.cmd->run("sudo pkill dnsmasq");
    QTimer::singleShot(1000, [&ctx]() {
      // 'dnsmasq' Service Start
      // ----------------------------------------------------------------------------
      QString dnsmasq = QString("sudo dnsmasq --interface=%1 --dhcp-range=%2,%3,%4,24h --port=%5") //
                            .arg(ctx.networkInterface)                                             //
                            .arg(ctx.dhcpRangeStart)                                               //
                            .arg(ctx.dhcpRangeEnd)                                                 //
                            .arg(ctx.subnetMask)                                                   //
                            .arg(QString::number(ctx.dnsmasqServicePort));
      ctx.cmd->run(dnsmasq);
      QTimer::singleShot(1000, [&ctx]() { ctx.cmd->run("sudo netstat -tuln | grep " + QString::number(ctx.dnsmasqServicePort)); });
    });
  } else {
    QTimer::singleShot(3000, []() { check_hostapd_with_start_dnsmasq(); });
  }
}

// Connect To Access Point
// ----------------------------------------------------------------------------
void connect_access_point() {
  auto &ctx = GlobalContext::instance();
  // ctx.cmd->run("udevadm info --query=all --path=/sys/class/net/" + ctx.networkInterface + " | grep -E 'ID_BUS|ID_OUI_FROM_DATABASE|ID_VENDOR_FROM_DATABASE|ID_MODEL_FROM_DATABASE'");

  // ctx.cmd->run("sudo iwconfig " + ctx.networkInterface + " channel " + QString::number(ctx.wifiChannel));
  // ctx.cmd->run("nmcli connection delete " + ctx.wifiSsid);

  bool connected = false;
  while (!connected) {
    QString result = ctx.cmd->run("sudo nmcli device wifi connect " + ctx.wifiSsid + " password " + ctx.wifiPassphrase + " ifname " + ctx.networkInterface);
    if (result.contains("successfully activated", Qt::CaseInsensitive)) {
      ctx.setMyIpAddress(ctx.systemInfo->get_ip_address(ctx.networkInterface));
      connected = true;
    } else {
      ctx.cmd->run("sudo nmcli dev wifi rescan");
      // ctx.cmd->run("nmcli dev wifi");
    }
  }
}
