#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

#include "command_runner.h"
#include <QNetworkInterface>
#include <QObject>
#include <QSysInfo>

enum class OSType {
  Fedora40,     //
  Ubuntu24,     //
  MacBook,      //
  JetsonNano,   //
  RaspberryPi4, //
  RaspberryPi5, //
  UNKNOWN       //
};

class SystemInfo : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString buildCpuArchitecture READ buildCpuArchitecture CONSTANT)
  Q_PROPERTY(QString currentCpuArchitecture READ currentCpuArchitecture CONSTANT)
  Q_PROPERTY(QString buildAbi READ buildAbi CONSTANT)
  Q_PROPERTY(QString kernelType READ kernelType CONSTANT)
  Q_PROPERTY(QString kernelVersion READ kernelVersion CONSTANT)
  Q_PROPERTY(QString productType READ productType CONSTANT)
  Q_PROPERTY(QString productVersion READ productVersion CONSTANT)
  Q_PROPERTY(QString prettyProductName READ prettyProductName CONSTANT)
  Q_PROPERTY(QString machineHostName READ machineHostName CONSTANT)

public:
  explicit SystemInfo(QObject *parent = nullptr)
      : QObject(parent) {
  }
  OSType getOSType() {
    if ("arm64" == QSysInfo::currentCpuArchitecture()) {
      if ("ubuntu" == QSysInfo::productType()) {
        if ("18.04" == QSysInfo::productVersion()) {
          return OSType::JetsonNano;
        }
      }
      if ("debian" == QSysInfo::productType()) {
        if ("12" == QSysInfo::productVersion()) {
          return OSType::RaspberryPi5;
        }
        if ("11" == QSysInfo::productVersion()) {
          return OSType::RaspberryPi4;
        }
      }
    }
    if ("x86_64" == QSysInfo::currentCpuArchitecture()) {
      if ("macos" == QSysInfo::productType()) {
        return OSType::MacBook;
      }
      if ("fedora" == QSysInfo::productType()) {
        if ("40" == QSysInfo::productVersion()) {
          return OSType::Fedora40;
        }
      }
      if ("ubuntu" == QSysInfo::productType()) {
        if ("24.04" == QSysInfo::productVersion()) {
          return OSType::Ubuntu24;
        }
      }
    }
    return OSType::UNKNOWN;
  }

  QString getIPAddress(const QString &interfaceName) {
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface &netInterface : interfaces) {
      if (netInterface.name() == interfaceName) {
        QList<QNetworkAddressEntry> entries = netInterface.addressEntries();
        for (const QNetworkAddressEntry &entry : entries) {
          if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
            return entry.ip().toString();
          }
        }
      }
    }
    return "";
  }

  QString getInterfaceNameFromIpAddress(const QString &ipAddress) {
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface &interface : interfaces) {
      QList<QNetworkAddressEntry> entries = interface.addressEntries();
      for (const QNetworkAddressEntry &entry : entries) {
        if (entry.ip().toString() == ipAddress) {
          return interface.humanReadableName();
        }
      }
    }
    return QString();
  }

  QString getInterfaceTypeFromIpAddress(const QString &ipAddress) {
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface &interface : interfaces) {
      QList<QNetworkAddressEntry> entries = interface.addressEntries();
      for (const QNetworkAddressEntry &entry : entries) {
        if (entry.ip().toString() == ipAddress) {
          QString interfaceName = interface.humanReadableName();
          if (interfaceName.startsWith("eth") || interfaceName.startsWith("en")) {
            return "Ethernet";
          } else if (interfaceName.startsWith("wl")) {
            return "WiFi";
          } else {
            return "Other";
          }
        }
      }
    }
    return "Not found";
  }

  QString buildCpuArchitecture() const {
    return QSysInfo::buildCpuArchitecture();
  }

  QString currentCpuArchitecture() const {
    return QSysInfo::currentCpuArchitecture();
  }

  QString buildAbi() const {
    return QSysInfo::buildAbi();
  }

  QString kernelType() const {
    return QSysInfo::kernelType();
  }

  QString kernelVersion() const {
    return QSysInfo::kernelVersion();
  }

  QString productType() const {
    return QSysInfo::productType();
  }

  QString productVersion() const {
    return QSysInfo::productVersion();
  }

  QString prettyProductName() const {
    return QSysInfo::prettyProductName();
  }

  QString machineHostName() const {
    return QSysInfo::machineHostName();
  }

  QString qtVersion() const {
    return QString("Qt %1.%2.%3").arg(QT_VERSION_MAJOR).arg(QT_VERSION_MINOR).arg(QT_VERSION_PATCH);
  }
};

#endif // SYSTEM_INFO_H
