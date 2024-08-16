#include "pipeline_manager.h"
#include "audio_pipelines.h"

#ifdef RECEIVER
#include "receive_video_pipelines.h"
#endif

#ifdef TRANSMITTER
#include "transmit_video_pipelines.h"
#endif

PipelineManager::PipelineManager(QObject *parent)
    : QObject(parent)
    , m_pipeline(nullptr)
    , m_videoitem_worker(nullptr) {
}

PipelineManager::~PipelineManager() {
  if (this->m_pipeline) {
    gst_object_unref(this->m_pipeline);
  }
  if (this->m_videoitem_worker) {
    delete this->m_videoitem_worker;
  }
}

GstElement *PipelineManager::pipeline() const {
  return this->m_pipeline;
}

#ifdef TRANSMITTER

// Start Transmit Video Pipeline
// ----------------------------------------------------------------------------
GstStateChangeReturn PipelineManager::setupVideoTransmitPipeline() {
  auto &ctx = GlobalContext::instance();
  GstElement *pipeline = nullptr;

  switch (ctx.type) {
  case OSType::Fedora40:
  case OSType::Ubuntu24: {
    QString transmitter_gpu_support = ctx.settings->loadStringParameter("transmitter/gpu_support");
    qInfo() << "Use GPU setting:" << transmitter_gpu_support.toUtf8().constData();
    if ("None" == transmitter_gpu_support) {
      pipeline = build_video_transmit_pipeline_linux();
    } else {
      ctx.commandRunner->runCommand("lspci -vnn | grep VGA -A 1");
      if ("Nvidia" == transmitter_gpu_support) {
        pipeline = build_video_transmit_pipeline_linux_nvidia_gpu();
      } else if ("Intel" == transmitter_gpu_support) {
        pipeline = build_video_transmit_pipeline_linux_intel_gpu();
      }
    }
  } break;
  case OSType::MacBook:
    pipeline = build_video_transmit_pipeline_mac();
    break;
  case OSType::JetsonNano:
    pipeline = build_video_transmit_pipeline_jetson();
    break;
  case OSType::RaspberryPi4:
    pipeline = build_video_transmit_pipeline_rpi4();
    break;
  case OSType::RaspberryPi5:
    pipeline = build_video_transmit_pipeline_rpi5();
    break;
  case OSType::UNKNOWN:
  default:
    qCritical() << "Not supported";
    return GST_STATE_CHANGE_FAILURE;
  }
  g_assert(pipeline);

  GstElement *sink = gst_bin_get_by_name(GST_BIN(pipeline), "sink");
  g_assert(sink);

  int receiver_video_port = ctx.settings->loadIntParameter("receiver/listen_video_port");
  g_object_set(sink, "host", ctx.targetIpAddress.toUtf8().constData(), "port", receiver_video_port, NULL);

  this->m_pipeline = static_cast<GstElement *>(gst_object_ref(pipeline));
  GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
  if (ret != GST_STATE_CHANGE_FAILURE) {
    qInfo() << QString("Sending video stream to: %1:%2").arg(ctx.targetIpAddress).arg(receiver_video_port).toUtf8().constData();
  }
  return ret;
}

// Start Transmit Audio Pipeline
// ----------------------------------------------------------------------------
GstStateChangeReturn PipelineManager::setupAudioTransmitPipeline() {
  auto &ctx = GlobalContext::instance();
  GstElement *pipeline = nullptr;

  if (ctx.type == OSType::MacBook) {
    pipeline = build_audio_transmit_pipeline_mac();
  } else {
    pipeline = build_audio_transmit_pipeline_linux();
  }
  g_assert(pipeline);

  GstElement *sink = gst_bin_get_by_name(GST_BIN(pipeline), "sink");
  g_assert(sink);

  int receiver_audio_port = ctx.settings->loadIntParameter("receiver/listen_audio_port");
  g_object_set(sink, "host", ctx.targetIpAddress.toUtf8().constData(), "port", receiver_audio_port, NULL);

  this->m_pipeline = static_cast<GstElement *>(gst_object_ref(pipeline));
  GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
  if (ret != GST_STATE_CHANGE_FAILURE) {
    qInfo() << QString("Sending audio stream to: %1:%2").arg(ctx.targetIpAddress).arg(receiver_audio_port).toUtf8().constData();
  }
  return ret;
}

#endif

#ifdef RECEIVER

Qt6GLVideoItemWorker::Qt6GLVideoItemWorker(GstElement *pipeline)
    : m_pipeline(pipeline) {
  this->m_pipeline = pipeline ? static_cast<GstElement *>(gst_object_ref(pipeline)) : NULL;
}

Qt6GLVideoItemWorker::~Qt6GLVideoItemWorker() {
  if (this->m_pipeline)
    gst_object_unref(this->m_pipeline);
}

// Start Pipeline of Receive Video
// ----------------------------------------------------------------------------
void Qt6GLVideoItemWorker::run() {
  if (this->m_pipeline) {
    GstStateChangeReturn ret = gst_element_set_state(this->m_pipeline, GST_STATE_PLAYING);
    if (ret != GST_STATE_CHANGE_FAILURE) {
      int listen_video_port = GlobalContext::instance().settings->loadIntParameter("receiver/listen_video_port");
      qInfo() << QString("Listen video port: %1").arg(listen_video_port).toUtf8().constData();
    }
  }
}

Qt6GLVideoItemWorker *PipelineManager::getVideoItemWorker() const {
  return this->m_videoitem_worker;
}

// Setup Receive Video Pipeline
// ----------------------------------------------------------------------------
GstElement *PipelineManager::setupVideoReceivePipeline() {
  auto &ctx = GlobalContext::instance();
  GstElement *pipeline = nullptr;

  if (ctx.type == OSType::MacBook) {
    pipeline = build_video_receive_pipeline_mac();
  } else {
    QString receiver_gpu_support = ctx.settings->loadStringParameter("receiver/gpu_support");
    qInfo() << "Use GPU setting:" << receiver_gpu_support.toUtf8().constData();
    if ("None" == receiver_gpu_support) {
      pipeline = build_video_receive_pipeline_linux();
    } else {
      ctx.commandRunner->runCommand("lspci -vnn | grep VGA -A 1");
      if ("Nvidia" == receiver_gpu_support) {
        pipeline = build_video_receive_pipeline_linux_nvidia_gpu();
      } else if ("Intel" == receiver_gpu_support) {
        pipeline = build_video_receive_pipeline_linux_intel_gpu();
      }
    }
  }
  g_assert(pipeline);

  GstElement *src = gst_bin_get_by_name(GST_BIN(pipeline), "src");
  g_assert(src);

  int listen_video_port = ctx.settings->loadIntParameter("receiver/listen_video_port");
  g_object_set(src, "port", listen_video_port, NULL);

  GstElement *sink = gst_bin_get_by_name(GST_BIN(pipeline), "sink");
  g_assert(sink);

  this->m_pipeline = static_cast<GstElement *>(gst_object_ref(pipeline));
  this->m_videoitem_worker = new Qt6GLVideoItemWorker(pipeline);

  return sink;
}

// Start Receive Audio Pipeline
// ----------------------------------------------------------------------------
GstStateChangeReturn PipelineManager::setupAudioReceivePipeline() {
  auto &ctx = GlobalContext::instance();
  GstElement *pipeline = nullptr;

  if (ctx.type == OSType::MacBook) {
    pipeline = build_audio_receive_pipeline_mac();
  } else {
    pipeline = build_audio_receive_pipeline_linux();
  }
  g_assert(pipeline);
  GstElement *src = gst_bin_get_by_name(GST_BIN(pipeline), "src");
  g_assert(src);

  int listen_audio_port = ctx.settings->loadIntParameter("receiver/listen_audio_port");
  g_object_set(src, "port", listen_audio_port, NULL);

  this->m_pipeline = static_cast<GstElement *>(gst_object_ref(pipeline));
  GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
  if (ret != GST_STATE_CHANGE_FAILURE) {
    qInfo() << QString("Listen audio port: %1").arg(listen_audio_port).toUtf8().constData();
  }
  return ret;
}

#endif

// Get Pipeline Info
// ----------------------------------------------------------------------------
QString PipelineManager::getPipelineInfo() {
  GstIterator *iter = gst_bin_iterate_elements(GST_BIN(this->m_pipeline));
  GValue item = G_VALUE_INIT;
  gboolean done = FALSE;
  QJsonArray elementsArray;

  while (!done) {
    switch (gst_iterator_next(iter, &item)) {
    case GST_ITERATOR_OK: {
      GstElement *element = GST_ELEMENT(g_value_get_object(&item));
      QJsonObject elementInfo = getElementInfo(element);
      elementsArray.append(elementInfo);
      g_value_reset(&item);
      break;
    }
    case GST_ITERATOR_RESYNC:
      gst_iterator_resync(iter);
      break;
    case GST_ITERATOR_ERROR:
    case GST_ITERATOR_DONE:
      done = TRUE;
      break;
    }
  }

  gst_iterator_free(iter);

  QJsonObject pipelineInfo;
  pipelineInfo["elements"] = elementsArray;

  QJsonDocument doc(pipelineInfo);
  QString jsonString = doc.toJson(QJsonDocument::Compact);
  return jsonString;
}

// Get Element Info
// ----------------------------------------------------------------------------
QJsonObject PipelineManager::getElementInfo(GstElement *element) {
  QJsonObject elementInfo;
  const gchar *element_name = gst_element_get_name(element);
  const gchar *element_type = gst_element_factory_get_longname(gst_element_get_factory(element));

  elementInfo["name"] = element_name;
  elementInfo["type"] = element_type;

  GParamSpec **properties;
  guint num_properties;
  properties = g_object_class_list_properties(G_OBJECT_GET_CLASS(element), &num_properties);

  QJsonObject propertiesInfo;
  for (guint i = 0; i < num_properties; i++) {
    GParamSpec *property = properties[i];
    gchar *name = g_strdup(property->name);
    GValue value = G_VALUE_INIT;
    g_value_init(&value, property->value_type);
    g_object_get_property(G_OBJECT(element), name, &value);
    gchar *value_str = g_strdup_value_contents(&value);
    propertiesInfo[name] = value_str;
    g_free(name);
    g_free(value_str);
    g_value_unset(&value);
  }
  g_free(properties);

  elementInfo["properties"] = propertiesInfo;

  return elementInfo;
}
