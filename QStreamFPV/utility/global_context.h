#ifndef GLOBAL_CONTEXT_H
#define GLOBAL_CONTEXT_H

#include "settings_manager.h"
#include <QObject>

class GlobalContext {
public:
  static GlobalContext &instance() {
    static GlobalContext instance;
    return instance;
  }

  QString mode;
  QString myIpAddress;
  QString targetIpAddress;
  QString networkInterface;
  QString interfaceType;
  bool connectNetwork;
  SettingsManager *settings;
  SystemInfo *systemInfo;
  OSType type;
  CommandRunner *commandRunner;

private:
  GlobalContext()
      : settings(nullptr) {
  }
};

#endif // GLOBAL_CONTEXT_H
