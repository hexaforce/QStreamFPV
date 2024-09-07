#include "command_runner.h"
#include <glib.h>

CommandRunner::CommandRunner(QObject *parent)
    : QObject(parent)
    , process(new QProcess(this)) {
}

// Run Command
// ----------------------------------------------------------------------------
QString CommandRunner::run(const QString &command) {

  QProcess process;
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

  env.insert("LANG", "en_US.UTF-8");
  process.setProcessEnvironment(env);

  g_print("command: %s\n", command.toUtf8().constData());

  process.start("bash", QStringList() << "-c" << command);
  if (!process.waitForFinished(-1)) {
    g_printerr("Error: Command timed out.\n");
    return "Error: Command timed out.";
  }

  QString output = process.readAllStandardOutput();
  QString error = process.readAllStandardError();
  
  if (!error.isEmpty()) {
    g_printerr("Error: %s\n", error.toUtf8().constData());
  }

  if (!output.isEmpty()) {
    g_print(output.toUtf8().constData());
  }

  return output;
}
