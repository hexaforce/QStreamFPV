#include "settings_manager.h"
#include "system_info.h"

SettingsManager::SettingsManager(const QString &organization, const QString &application, QObject *parent)
    : QObject(parent)
    , settings(organization, application) {
  initializeDefaultValues();
}

SettingsManager::~SettingsManager() {
}

// Initialize Default Values
// ----------------------------------------------------------------------------
void SettingsManager::initializeDefaultValues() {

  // Window Setting ===========================================================
  defaultValues["window/size"] = QSize(960, 540);
  defaultValues["window/full_screen"] = false;
  defaultValues["window/frame_less"] = false;

  // Network Mode =============================================================
  defaultValues["network/mode"] = "AccessPointMode"; // RouterMode or AccessPointMode

  // AccessPoint Mode Only ====================================================
  defaultValues["wifi/ssid"] = "QStreamFPV";
  defaultValues["wifi/passphrase"] = "QStreamFPV";
  defaultValues["wifi/access_point"] = "Receiver"; // Receiver or Transmitter
  defaultValues["wifi/access_point_ip_address"] = "10.10.10.10";
  defaultValues["wifi/start_dhcp_range"] = "10.10.10.11";
  defaultValues["wifi/end_dhcp_range"] = "10.10.10.100";
  defaultValues["wifi/subnet_mask"] = "255.192.0.0"; // /10
  defaultValues["wifi/dnsmasq_port"] = 1053;
  defaultValues["receiver/network_interface"] = "wlan0";
  defaultValues["transmitter/network_interface"] = "wlan0";

  // Router Mode Only =========================================================
  defaultValues["receiver/ip_address"] = "192.168.1.1";
  defaultValues["transmitter/ip_address"] = "192.168.1.2";

  // Receiver Port ============================================================
  defaultValues["receiver/listen_video_port"] = 5000;
  defaultValues["receiver/listen_audio_port"] = 5001;
  defaultValues["receiver/listen_telemetry_port"] = 5009;

  // Transmitter Port =========================================================
  defaultValues["transmitter/listen_telemetry_port"] = 5008;

  // Transmitter Audio ========================================================
  defaultValues["transmitter/enable_audio"] = false;

  // Video Codec ==============================================================
  defaultValues["receiver/video_codec"] = "H264";    // H264 or H265
  defaultValues["transmitter/video_codec"] = "H264"; // H264 or H265

  // GPU Support (Fedora/Ubuntu Only) =========================================
  defaultValues["receiver/gpu_support"] = "None";    // None or Nvidia or Intel
  defaultValues["transmitter/gpu_support"] = "None"; // None or Nvidia or Intel

  // Transmitter Capture Resolution & Framerate  ==============================
  defaultValues["transmitter/video_resolution"] = QSize(1920, 1080);
  defaultValues["transmitter/video_framerate"] = "30/1";
}

void SettingsManager::saveStringParameter(const QString &key, const QString &value) {
  settings.setValue(key, value);
}

QString SettingsManager::loadStringParameter(const QString &key) const {
  return settings.value(key, defaultValues.value(key).toString()).toString();
}

void SettingsManager::saveIntParameter(const QString &key, int value) {
  settings.setValue(key, value);
}

int SettingsManager::loadIntParameter(const QString &key) const {
  return settings.value(key, defaultValues.value(key).toInt()).toInt();
}

void SettingsManager::saveBoolParameter(const QString &key, bool value) {
  settings.setValue(key, value);
}

bool SettingsManager::loadBoolParameter(const QString &key) const {
  return settings.value(key, defaultValues.value(key).toBool()).toBool();
}

void SettingsManager::saveSizeParameter(const QString &key, const QSize &value) {
  settings.setValue(key, value);
}

QSize SettingsManager::loadSizeParameter(const QString &key) const {
  return settings.value(key, defaultValues.value(key).toSize()).toSize();
}
