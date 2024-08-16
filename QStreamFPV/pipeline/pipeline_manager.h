#ifndef PIPELINE_MANAGER_H
#define PIPELINE_MANAGER_H

#include "global_context.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRunnable>
#include <gst/gst.h>

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
  GstStateChangeReturn setupVideoTransmitPipeline();
  GstStateChangeReturn setupAudioTransmitPipeline();
#endif

#ifdef RECEIVER
  Qt6GLVideoItemWorker *getVideoItemWorker() const;
  GstElement *setupVideoReceivePipeline();
  GstStateChangeReturn setupAudioReceivePipeline();
#endif

  Q_INVOKABLE QString getPipelineInfo();

signals:
  void errorOccurred(const QString &error);

private:
  GstElement *m_pipeline;
  Qt6GLVideoItemWorker *m_videoitem_worker;
  QJsonObject getElementInfo(GstElement *element);
};

#endif // PIPELINE_MANAGER_H
