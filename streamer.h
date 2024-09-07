#ifndef STREAMER_H
#define STREAMER_H

#include "pipeline_manager.h"
#include "system_info.h"
#include "telemetry_socket_transmitter.h"

class TelemetrySocket;

class Streamer : public QObject {
  Q_OBJECT
public:
  explicit Streamer(TelemetrySocket *socket, QObject *parent = nullptr);
  ~Streamer();

  void start_streaming();

  void handle_telemetry_message(const QString &message);

  QString get_video_pipeline_info() const;
  QString get_audio_pipeline_info() const;

signals:
  void errorOccurred(const QString &error);

private:
  TelemetrySocket *socket;
  PipelineManager *videoPipelineManager;
  PipelineManager *audioPipelineManager;

  void handle_gst_message(GstElement *pipeline, const QString &pipelineType);
};

#endif // STREAMER_H
