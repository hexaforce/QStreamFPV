#ifndef PIPELINE_MANAGER_H
#define PIPELINE_MANAGER_H

#include "global_context.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRunnable>

class Qt6GLVideoItemWorker : public QRunnable {
public:
  Qt6GLVideoItemWorker(GstElement *pipeline);
  ~Qt6GLVideoItemWorker();

  void run() override;

private:
  GstElement *m_pipeline;
};

class PipelineManager : public QObject {
  Q_OBJECT

public:
  explicit PipelineManager(QObject *parent = nullptr);
  ~PipelineManager();

  GstElement *pipeline() const;

#ifdef TRANSMITTER
  GstStateChangeReturn setup_video_transmit_pipeline();
  GstStateChangeReturn setup_audio_transmit_pipeline();
#endif

#ifdef RECEIVER
  Qt6GLVideoItemWorker *get_video_item_worker() const;
  GstElement *setup_video_receive_pipeline();
  GstStateChangeReturn setup_audio_receive_pipeline();
#endif

  Q_INVOKABLE QString get_pipeline_info();

signals:
  void errorOccurred(const QString &error);

private:
  GstElement *m_pipeline;
  Qt6GLVideoItemWorker *m_videoitem_worker;
  QJsonObject get_element_info(GstElement *element);
};

#endif // PIPELINE_MANAGER_H
