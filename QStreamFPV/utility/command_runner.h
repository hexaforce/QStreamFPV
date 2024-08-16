#ifndef COMMAND_RUNNER_H
#define COMMAND_RUNNER_H

#include <QObject>
#include <QProcess>
#include <QString>

class CommandRunner : public QObject {
  Q_OBJECT
public:
  explicit CommandRunner(QObject *parent = nullptr);

  Q_INVOKABLE QString runCommand(const QString &command);

private:
  QProcess *process;
};

#endif // COMMAND_RUNNER_H
