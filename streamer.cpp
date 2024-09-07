#include "streamer.h"

Streamer::Streamer(TelemetrySocket *socket, QObject *parent)
    : QObject(parent)
    , socket(socket)
    , videoPipelineManager(new PipelineManager())
    , audioPipelineManager(new PipelineManager()) {
  socket->setStreamer(this);
}

Streamer::~Streamer() {
  delete videoPipelineManager;
  delete audioPipelineManager;
}

// Start Streaming
// ----------------------------------------------------------------------------
void Streamer::start_streaming() {
  g_print("start_streaming\n");
  auto &ctx = GlobalContext::instance();
  videoPipelineManager->setup_video_transmit_pipeline();
  if (ctx.enableAudio) {
    audioPipelineManager->setup_audio_transmit_pipeline();
  }
  QTimer *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, [&]() {
    handle_gst_message(videoPipelineManager->pipeline(), "Video");
    if (ctx.enableAudio) {
      handle_gst_message(audioPipelineManager->pipeline(), "Audio");
    }
  });
  timer->start(100);
}

// Handling Telemetry Message
// ----------------------------------------------------------------------------
void Streamer::handle_telemetry_message(const QString &message) {
  g_print("handle_telemetry_message: %s\n", message.toUtf8().constData());
  // if (message == "AcceptIPAddress") {
  //   start_streaming();
  // } else {
  //   qDebug() << "message received: " << message;
  // }
}

// Get Video Pipeline Info
// ----------------------------------------------------------------------------
QString Streamer::get_video_pipeline_info() const {
  return videoPipelineManager ? videoPipelineManager->get_pipeline_info() : QString();
}

// Get Audio Pipeline Info
// ----------------------------------------------------------------------------
QString Streamer::get_audio_pipeline_info() const {
  return audioPipelineManager ? audioPipelineManager->get_pipeline_info() : QString();
}

// Get Type Mame
// ----------------------------------------------------------------------------
const gchar *get_stream_status_type_name(GstStreamStatusType type) {
  GEnumClass *enum_class = static_cast<GEnumClass *>(g_type_class_ref(gst_stream_status_type_get_type()));
  const gchar *type_name = g_enum_get_value(enum_class, type)->value_nick;
  g_type_class_unref(enum_class);
  return type_name;
}

// Bus Message
// ----------------------------------------------------------------------------
void Streamer::handle_gst_message(GstElement *pipeline, const QString &pipelineType) {
  GstBus *bus = gst_element_get_bus(pipeline);
  GstMessage *message = gst_bus_pop_filtered(bus, GST_MESSAGE_ANY);
  if (message) {
    switch (GST_MESSAGE_TYPE(message)) {
    case GST_MESSAGE_ERROR: {
      GError *error = nullptr;
      gst_message_parse_error(message, &error, nullptr);
      QString errorMsg = QString("%1 pipeline error: %2").arg(pipelineType, error->message);
      socket->send_message(errorMsg);
      emit errorOccurred(errorMsg);
      g_clear_error(&error);
      break;
    }
    case GST_MESSAGE_WARNING: {
      GError *warning = nullptr;
      gst_message_parse_warning(message, &warning, nullptr);
      QString warningMsg = QString("%1 pipeline warning: %2").arg(pipelineType, warning->message);
      socket->send_message(warningMsg);
      emit errorOccurred(warningMsg);
      g_clear_error(&warning);
      break;
    }
    case GST_MESSAGE_EOS: {
      QString eosMsg = QString("%1 pipeline reached end of stream.").arg(pipelineType);
      socket->send_message(eosMsg);
      emit errorOccurred(eosMsg);
      break;
    }
    case GST_MESSAGE_STATE_CHANGED: {
      GstState oldState, newState, pendingState;
      gst_message_parse_state_changed(message, &oldState, &newState, &pendingState);
      if (GST_MESSAGE_SRC(message) == GST_OBJECT(pipeline)) {
        QString stateChangeMsg = QString("%1 pipeline state changed from %2 to %3.").arg(pipelineType, gst_element_state_get_name(oldState), gst_element_state_get_name(newState));
        socket->send_message(stateChangeMsg);
      }
      break;
    }
    case GST_MESSAGE_BUFFERING: {
      gint percent = 0;
      gst_message_parse_buffering(message, &percent);
      QString bufferingMsg = QString("%1 pipeline buffering: %2%%").arg(pipelineType).arg(percent);
      socket->send_message(bufferingMsg);
      break;
    }
    case GST_MESSAGE_STREAM_STATUS: {
      GstStreamStatusType streamStatus;
      GstElement *owner = nullptr;
      gst_message_parse_stream_status(message, &streamStatus, &owner);
      const gchar *statusTypeName = get_stream_status_type_name(streamStatus);
      QString streamStatusMsg = QString("%1 stream status: %2 for element %3.").arg(pipelineType, statusTypeName, GST_ELEMENT_NAME(owner));
      socket->send_message(streamStatusMsg);
      break;
    }
    case GST_MESSAGE_NEW_CLOCK: {
      GstClock *newClock = nullptr;
      gst_message_parse_new_clock(message, &newClock);
      QString newClockMsg = QString("%1 new clock: %2.").arg(pipelineType, gst_object_get_name(GST_OBJECT(newClock)));
      socket->send_message(newClockMsg);
      break;
    }
    case GST_MESSAGE_TAG: {
      GstTagList *tags = nullptr;
      gst_message_parse_tag(message, &tags);
      gchar *tagsStr = gst_tag_list_to_string(tags);
      QString tagMsg = QString("%1 tags: %2.").arg(pipelineType).arg(tagsStr);
      socket->send_message(tagMsg);
      g_free(tagsStr);
      gst_tag_list_unref(tags);
      break;
    }
    case GST_MESSAGE_LATENCY: {
      QString latencyMsg = QString("%1 latency message received.").arg(pipelineType);
      socket->send_message(latencyMsg);
      break;
    }
    case GST_MESSAGE_ASYNC_DONE: {
      QString asyncDoneMsg = QString("%1 async done message received.").arg(pipelineType);
      socket->send_message(asyncDoneMsg);
      break;
    }
    case GST_MESSAGE_STREAM_START: {
      QString streamStartMsg = QString("%1 stream start message received.").arg(pipelineType);
      socket->send_message(streamStartMsg);
      break;
    }
    case GST_MESSAGE_QOS: {
      GstFormat format;
      guint64 processed, dropped;
      gst_message_parse_qos_stats(message, &format, &processed, &dropped);
      QString qosMsg = QString("%1 QOS message received: processed=%2, dropped=%3.").arg(pipelineType).arg(processed).arg(dropped);
      socket->send_message(qosMsg);
      break;
    }
    default:
      qDebug() << "Unhandled message type: " << GST_MESSAGE_TYPE_NAME(message);
      break;
    }
    gst_message_unref(message);
  }
  gst_object_unref(bus);
}
