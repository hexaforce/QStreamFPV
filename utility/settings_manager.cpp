#include "settings_manager.h"
#include "system_info.h"

SettingsManager::SettingsManager(const QString &organization, const QString &application, QObject *parent)
    : QObject(parent)
    , settings(organization, application) {
  initialize_default_values();
}

SettingsManager::~SettingsManager() {
}

// Initialize Default Values
// ----------------------------------------------------------------------------
void SettingsManager::initialize_default_values() {

  // Window Setting ===========================================================
  defaultValues["window/size"] = QSize(1280, 720);
  defaultValues["window/full_screen"] = false;
  defaultValues["window/frame_less"] = false;

  // Network Mode =============================================================
  defaultValues["network/mode"] = "AccessPointMode"; // RouterMode or AccessPointMode

  // AccessPoint Mode Only ====================================================
  defaultValues["wifi/access_point"] = "Transmitter"; //  Transmitter or Receiver

  defaultValues["wifi/network_interface"] = "wlan0";

  defaultValues["wifi/ssid"] = "QStreamFPV";
  defaultValues["wifi/passphrase"] = "QStreamFPV";

  defaultValues["wifi/region_code"] = "US";
  defaultValues["wifi/channel"] = 40; // 5200 MHz

  defaultValues["wifi/access_point_ip_address"] = "10.10.10.10";
  defaultValues["wifi/dhcp_range_start"] = "10.10.10.11";
  defaultValues["wifi/dhcp_range_end"] = "10.10.10.111";
  defaultValues["wifi/subnet_mask"] = "255.192.0.0"; // /10

  defaultValues["wifi/dnsmasq_service_port"] = 1053;

  // Router Mode Only =========================================================
  defaultValues["receiver/ip_address"] = "192.168.86.109";
  defaultValues["transmitter/ip_address"] = "192.168.86.106";

  // Communication ==========================================================
  defaultValues["communication/video_port"] = 5000;
  defaultValues["communication/audio_port"] = 5001;
  defaultValues["communication/telemetry_port"] = 5009;
  defaultValues["transmitter/enable_audio"] = true;

  // GStreamer ===============================================================
  defaultValues["gstreamer/video_codec"] = "H264"; // H264 or H265 or SupportedH264
  defaultValues["gstreamer/gpu_support"] = "None"; // None or Nvidia or Intel
  // GST_LEVEL_NONE = 0,
  // GST_LEVEL_ERROR = 1,
  // GST_LEVEL_WARNING = 2,
  // GST_LEVEL_FIXME = 3,
  // GST_LEVEL_INFO = 4,
  // GST_LEVEL_DEBUG = 5,
  defaultValues["gstreamer/log_level"] = 3;

  // Transmitter Video Setting ===================================================
  defaultValues["transmitter/camera_device"] = "/dev/video0";
  defaultValues["transmitter/caps_format"] = "YUY2";
  defaultValues["transmitter/video_resolution"] = QSize(1280, 720);
  defaultValues["transmitter/video_framerate"] = "10/1";
}

void SettingsManager::saveString(const QString &key, const QString &value) {
  settings.setValue(key, value);
}

QString SettingsManager::loadString(const QString &key) const {
  return settings.value(key, defaultValues.value(key).toString()).toString();
}

void SettingsManager::saveInt(const QString &key, int value) {
  settings.setValue(key, value);
}

int SettingsManager::loadInt(const QString &key) const {
  return settings.value(key, defaultValues.value(key).toInt()).toInt();
}

void SettingsManager::saveBool(const QString &key, bool value) {
  settings.setValue(key, value);
}

bool SettingsManager::loadBool(const QString &key) const {
  return settings.value(key, defaultValues.value(key).toBool()).toBool();
}

void SettingsManager::saveSize(const QString &key, const QSize &value) {
  settings.setValue(key, value);
}

QSize SettingsManager::loadSize(const QString &key) const {
  return settings.value(key, defaultValues.value(key).toSize()).toSize();
}

const char *SettingsManager::toProcessModeString(ProcessMode mode) {
  switch (mode) {
  case ProcessMode::Transmitter:
    return "Transmitter";
  case ProcessMode::Receiver:
    return "Receiver";
  default:
    return "Invalid ProcessMode";
  }
}

const char *SettingsManager::toNetworkModeString(NetworkMode mode) {
  switch (mode) {
  case NetworkMode::AccessPointMode:
    return "AccessPointMode";
  case NetworkMode::RouterMode:
    return "RouterMode";
  default:
    return "Invalid NetworkMode";
  }
}

const char *SettingsManager::toWifiAccessPointString(WifiAccessPoint point) {
  switch (point) {
  case WifiAccessPoint::Transmitter:
    return "Transmitter";
  case WifiAccessPoint::Receiver:
    return "Receiver";
  case WifiAccessPoint::None:
    return "None";
  default:
    return "Invalid WifiAccessPoint";
  }
}

const char *SettingsManager::toVideoCodecString(VideoCodec codec) {
  switch (codec) {
  case VideoCodec::H264:
    return "H264";
  case VideoCodec::H265:
    return "H265";
  case VideoCodec::SupportedH264:
    return "SupportedH264";
  default:
    return "Invalid VideoCodec";
  }
}

const char *SettingsManager::toGPUSupportString(GPUSupport support) {
  switch (support) {
  case GPUSupport::Nvidia:
    return "Nvidia";
  case GPUSupport::Intel:
    return "Intel";
  case GPUSupport::None:
    return "None";
  default:
    return "Invalid GPUSupport";
  }
}
