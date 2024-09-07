#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include "system_info.h"
#include <QMap>
#include <QPoint>
#include <QSettings>
#include <QSize>

enum class ProcessMode {
  Transmitter, //
  Receiver     //
};

enum class NetworkMode {
  AccessPointMode, //
  RouterMode       //
};

enum class WifiAccessPoint {
  Transmitter, //
  Receiver,    //
  None
};

enum class VideoCodec {
  H264, //
  H265, //
  SupportedH264
};

enum class GPUSupport {
  Nvidia, //
  Intel,  //
  None
};

OSType get_os_type();

class SettingsManager : public QObject {
  Q_OBJECT
public:
  SettingsManager(const QString &organization, const QString &application, QObject *parent = nullptr);
  ~SettingsManager() override;

  Q_INVOKABLE void saveString(const QString &key, const QString &value);
  Q_INVOKABLE QString loadString(const QString &key) const;

  Q_INVOKABLE void saveInt(const QString &key, int value);
  Q_INVOKABLE int loadInt(const QString &key) const;

  Q_INVOKABLE void saveBool(const QString &key, bool value);
  Q_INVOKABLE bool loadBool(const QString &key) const;

  Q_INVOKABLE void saveSize(const QString &key, const QSize &value);
  Q_INVOKABLE QSize loadSize(const QString &key) const;

  const char *toProcessModeString(ProcessMode mode);
  const char *toNetworkModeString(NetworkMode mode);
  const char *toWifiAccessPointString(WifiAccessPoint point);
  const char *toVideoCodecString(VideoCodec codec);
  const char *toGPUSupportString(GPUSupport support);

private:
  QSettings settings;
  QMap<QString, QVariant> defaultValues;
  void initialize_default_values();
};

#endif // SETTINGS_MANAGER_H
