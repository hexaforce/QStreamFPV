#ifndef GLOBAL_CONTEXT_H
#define GLOBAL_CONTEXT_H

#include "settings_manager.h"
#include <QDebug>
#include <QHostAddress>
#include <QTimer>

class GlobalContext {
public:
  static GlobalContext &instance() {
    static GlobalContext instance;
    return instance;
  }

  SettingsManager *settings;
  SystemInfo *systemInfo;
  CommandRunner *cmd;

  OSType osType;
  ProcessMode processMode;
  NetworkMode networkMode;
  WifiAccessPoint wifiAccessPoint;
  VideoCodec videoCodec;
  GPUSupport gpuSupport;

  QString networkInterface;
  QString interfaceType;

  QHostAddress accessPointIpAddress;
  QString wifiRegionCode;
  int wifiChannel;
  QString wifiSsid;
  QString wifiPassphrase;

  QString dhcpRangeStart;
  QString dhcpRangeEnd;
  QString subnetMask;
  int dnsmasqServicePort;

  QString product;
  bool acceptConnect;

  int videoPort;
  int audioPort;
  int telemetryPort;
  bool enableAudio;

  QString cameraDevice;
  QString capsFormat;
  QSize videoResolution;
  QString videoFramerate;

  // Setup Context
  // ----------------------------------------------------------------------------
  bool initialize(ProcessMode mode) {

    processMode = mode;
    settings = new SettingsManager("QStreamerFPV", "settings");
    systemInfo = new SystemInfo();
    osType = systemInfo->get_os_type();
    if (osType == OSType::UNKNOWN) {
      g_printerr("Not supported 'OS'\n");
      return false;
    }
    g_print("Current Process Mode: %s\n", settings->toProcessModeString(processMode));
    cmd = new CommandRunner();

    // Encode/Decode Setting
    // ===========================================
    QString codec = settings->loadString("gstreamer/video_codec");
    if (codec == "H264") {
      videoCodec = VideoCodec::H264;
    } else if (codec == "H265") {
      videoCodec = VideoCodec::H265;
    } else if (codec == "SupportedH264") {
      videoCodec = VideoCodec::SupportedH264;
    }
    g_print("Current Video Codec: %s\n", settings->toVideoCodecString(videoCodec));

    // ### GPU Support Setting
    // ===========================================
    QString gpu = settings->loadString("gstreamer/gpu_support");
    if (osType == OSType::LinuxX86) {
      if (gpu == "Nvidia") {
        gpuSupport = GPUSupport::Nvidia;
      } else if (gpu == "Intel") {
        gpuSupport = GPUSupport::Intel;
      } else if (gpu == "None") {
        gpuSupport = GPUSupport::None;
      }
      g_print("Current GPU Support: %s\n", settings->toGPUSupportString(gpuSupport));
    }

    // ### Network Setting
    // ===========================================
    QString network = settings->loadString("network/mode");
    if (network == "RouterMode") {
      networkMode = NetworkMode::RouterMode;
      wifiAccessPoint = WifiAccessPoint::None;
    } else if (network == "AccessPointMode") {
      networkMode = NetworkMode::AccessPointMode;
      QString accessPoint = settings->loadString("wifi/access_point");
      if (accessPoint == "Transmitter") {
        wifiAccessPoint = WifiAccessPoint::Transmitter;
      } else if (accessPoint == "Receiver") {
        wifiAccessPoint = WifiAccessPoint::Receiver;
      }
      g_print("Current Wifi Access Point: %s\n", settings->toWifiAccessPointString(wifiAccessPoint));
      wifiRegionCode = settings->loadString("wifi/region_code");
      wifiChannel = settings->loadInt("wifi/channel");
      wifiSsid = settings->loadString("wifi/ssid");
      wifiPassphrase = settings->loadString("wifi/passphrase");
    }
    g_print("Current Network Mode: %s\n", settings->toNetworkModeString(networkMode));

    // ### Connect Setting
    // ===========================================
    if (networkMode == NetworkMode::RouterMode) {
      if (processMode == ProcessMode::Transmitter) {
        // Transmitter RouterMode
        setMyIpAddress(QHostAddress(settings->loadString("transmitter/ip_address")));
        setTargetIpAddress(QHostAddress(settings->loadString("receiver/ip_address")));
      } else if (processMode == ProcessMode::Receiver) {
        // Receiver RouterMode
        setMyIpAddress(QHostAddress(settings->loadString("receiver/ip_address")));
        setTargetIpAddress(QHostAddress(settings->loadString("transmitter/ip_address")));
      }
      networkInterface = systemInfo->get_interface_name_from_ip_address(myIpAddress());
      acceptConnect = true;

      g_print("My Network Interface: %s\n", networkInterface.toUtf8().constData());
      g_print("My IP Address: %s\n", myIpAddress().toString() .toUtf8().constData());
      g_print("Target IP Address: %s\n", targetIpAddress().toString() .toUtf8().constData());

    } else if (networkMode == NetworkMode::AccessPointMode) {
      networkInterface = settings->loadString("wifi/network_interface");
      accessPointIpAddress = QHostAddress(settings->loadString("wifi/access_point_ip_address"));
      if ((processMode == ProcessMode::Transmitter && wifiAccessPoint == WifiAccessPoint::Transmitter) || //
          (processMode == ProcessMode::Receiver && wifiAccessPoint == WifiAccessPoint::Receiver)) {
        //  AccessPoint Setting
        setMyIpAddress(accessPointIpAddress);
        dhcpRangeStart = settings->loadString("wifi/dhcp_range_start");
        dhcpRangeEnd = settings->loadString("wifi/dhcp_range_end");
        subnetMask = settings->loadString("wifi/subnet_mask");
        dnsmasqServicePort = settings->loadInt("wifi/dnsmasq_service_port");
      } else {
        setTargetIpAddress(accessPointIpAddress);
      }
    }

    // ### Port Setting
    // ===========================================
    videoPort = settings->loadInt("communication/video_port");
    audioPort = settings->loadInt("communication/audio_port");
    telemetryPort = settings->loadInt("communication/telemetry_port");

    // ### Property
    // ===========================================
    if (processMode == ProcessMode::Transmitter) {
      // Transmitter Properties
      if (osType == OSType::JetsonNano) {
        product = cmd->run("sudo cat /etc/nv_tegra_release");
      } else if (osType == OSType::RaspberryPi) {
        product = cmd->run("cat /proc/device-tree/model");
        g_print("\n");
      }
      cameraDevice = settings->loadString("transmitter/camera_device");
      capsFormat = settings->loadString("transmitter/caps_format");
      videoResolution = settings->loadSize("transmitter/video_resolution");
      videoFramerate = settings->loadString("transmitter/video_framerate");
      enableAudio = settings->loadBool("transmitter/enable_audio");
    } else if (processMode == ProcessMode::Receiver) {
      // Receiver Properties
      if (!systemInfo->isGstElementAvailable("qml6glsink")) {
        g_printerr("Not supported 'qml6glsink'\n");
        return false;
      }
    }

    // Network interfaceType
    // ===========================================
    if (networkInterface.startsWith("eth") || networkInterface.startsWith("en")) {
      interfaceType = "Ethernet";
    } else if (networkInterface.startsWith("wl")) {
      interfaceType = "WiFi";
    } else {
      interfaceType = "Other";
    }

    return true;
  }

  QHostAddress myIpAddress() const {
    return m_myIpAddress;
  }

  void setMyIpAddress(const QHostAddress &address) {
    if (m_myIpAddress != address) {
      m_myIpAddress = address;
    }
  }

  QHostAddress targetIpAddress() const {
    return m_targetIpAddress;
  }

  void setTargetIpAddress(const QHostAddress &address) {
    if (m_targetIpAddress != address) {
      m_targetIpAddress = address;
    }
  }

  bool is_network_ready() const {
    return !m_myIpAddress.isNull() && !m_targetIpAddress.isNull();
  }

private:
  GlobalContext()
      : settings(nullptr)
      , systemInfo(nullptr)
      , cmd(nullptr) {
  }
  QHostAddress m_myIpAddress;
  QHostAddress m_targetIpAddress;
};

#endif // GLOBAL_CONTEXT_H
