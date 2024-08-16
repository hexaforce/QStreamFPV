#include "command_runner.h"
#include <QDebug>

CommandRunner::CommandRunner(QObject *parent)
    : QObject(parent)
    , process(new QProcess(this)) {
}

// Run Command
// ----------------------------------------------------------------------------
QString CommandRunner::runCommand(const QString &command) {
  QProcess process;
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("LANG", "en_US.UTF-8");
  process.setProcessEnvironment(env);

  process.start("bash", QStringList() << "-c" << command);
  if (!process.waitForFinished(-1)) {
    qWarning() << "Error: Command timed out.";
    return "Error: Command timed out.";
  }

  QString output = process.readAllStandardOutput();
  QString error = process.readAllStandardError();
  if (!error.isEmpty()) {
    qWarning() << error.toUtf8().constData();
  }
  if (!output.isEmpty()) {
    qInfo() << output.toUtf8().constData();
  }
  return output;
}
