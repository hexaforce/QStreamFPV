#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include <QMap>
#include <QObject>
#include <QPoint>
#include <QSettings>
#include <QSize>
#include <QString>

#include "system_info.h"

OSType getOSType();

class SettingsManager : public QObject {
  Q_OBJECT
public:
  SettingsManager(const QString &organization, const QString &application, QObject *parent = nullptr);
  ~SettingsManager() override;

  Q_INVOKABLE void saveStringParameter(const QString &key, const QString &value);
  Q_INVOKABLE QString loadStringParameter(const QString &key) const;

  Q_INVOKABLE void saveIntParameter(const QString &key, int value);
  Q_INVOKABLE int loadIntParameter(const QString &key) const;

  Q_INVOKABLE void saveBoolParameter(const QString &key, bool value);
  Q_INVOKABLE bool loadBoolParameter(const QString &key) const;

  Q_INVOKABLE void saveSizeParameter(const QString &key, const QSize &value);
  Q_INVOKABLE QSize loadSizeParameter(const QString &key) const;

private:
  QSettings settings;
  QMap<QString, QVariant> defaultValues;
  void initializeDefaultValues();
};

#endif // SETTINGS_MANAGER_H
