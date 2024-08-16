#ifndef STREAMER_H
#define STREAMER_H

#include "pipeline_manager.h"
#include "system_info.h"
#include "telemetry_socket_transmitter.h"
#include <QObject>
#include <QString>
#include <gst/gst.h>

class TelemetrySocket;

class Streamer : public QObject {
  Q_OBJECT
public:
  explicit Streamer(TelemetrySocket *socket, QObject *parent = nullptr);
  ~Streamer();

  void startStreaming();

  void handleTelemetryMessage(const QString &message);

  QString getVideoPipelineInfo() const;
  QString getAudioPipelineInfo() const;

signals:
  void errorOccurred(const QString &error);

private:
  TelemetrySocket *socket;
  PipelineManager *videoPipelineManager;
  PipelineManager *audioPipelineManager;

  void handleGstMessage(GstElement *pipeline, const QString &pipelineType);
};

#endif // STREAMER_H
