#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

#include "command_runner.h"
#include <QNetworkInterface>
#include <QSysInfo>
#include <gst/gst.h>

enum class OSType {
  LinuxX86,    //
  MacIntel,    //
  MacApple,    //
  JetsonNano,  //
  RaspberryPi, //
  Android32,   //
  Android64,   //
  iOS,         //
  UNKNOWN      //
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

  OSType get_os_type() {
    std::string output = std::string(prettyProductName().toUtf8().constData()) + "[" + currentCpuArchitecture().toUtf8().constData() + "]";
    g_print("%s\n", output.c_str());
    g_print("%s\n", qtVersion().toUtf8().constData());
    g_print("%s\n", gstVersion().toUtf8().constData());

    if ("macos" == productType()) {
      if ("x86_64" == currentCpuArchitecture()) {
        return OSType::MacIntel;
      } else if ("arm64" == currentCpuArchitecture()) {
        return OSType::MacApple;
      }
    } else if ("android" == productType()) {
      if ("armeabi-v7a" == currentCpuArchitecture()) {
        return OSType::Android32;
      } else if ("arm64" == currentCpuArchitecture()) {
        return OSType::Android64;
      }
    } else if ("ios" == productType()) {
      return OSType::iOS;
    }

    if ("x86_64" == currentCpuArchitecture()) {
      if ("fedora" == productType() || "ubuntu" == productType() || "debian" == productType()) {
        return OSType::LinuxX86;
      }
    } else if ("arm64" == currentCpuArchitecture()) {
      if ("ubuntu" == productType()) {
        return OSType::JetsonNano;
      } else if ("debian" == productType()) {
        return OSType::RaspberryPi;
      }
    }
    return OSType::UNKNOWN;
  }

  QHostAddress get_ip_address(const QString &interfaceName) {
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface &netInterface : interfaces) {
      if (netInterface.name() == interfaceName) {
        QList<QNetworkAddressEntry> entries = netInterface.addressEntries();
        for (const QNetworkAddressEntry &entry : entries) {
          if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
            return entry.ip();
          }
        }
      }
    }
    return QHostAddress::Any;
  }

  QString get_interface_name_from_ip_address(const QHostAddress &ipAddress) {
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface &interface : interfaces) {
      QList<QNetworkAddressEntry> entries = interface.addressEntries();
      for (const QNetworkAddressEntry &entry : entries) {
        if (entry.ip() == ipAddress) {
          return interface.humanReadableName();
        }
      }
    }
    return QString();
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

  QString gstVersion() const {
    guint major, minor, micro, nano;
    gst_version(&major, &minor, &micro, &nano);
    return QString("GStreamer %1.%2.%3.%4").arg(major).arg(minor).arg(micro).arg(nano);
  }

  bool isGstElementAvailable(const gchar *name) {
    GstElementFactory *factory = gst_element_factory_find(name);
    if (factory) {
      gst_object_unref(factory);
      return true;
    }
    return false;
  }
};

#endif // SYSTEM_INFO_H
