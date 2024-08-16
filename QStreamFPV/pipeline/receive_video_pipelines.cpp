#include "receive_video_pipelines.h"

// Linux Software decoding
// ----------------------------------------------------------------------------
GstElement *build_video_receive_pipeline_linux() {

  GstElement *pipeline = nullptr;
  QString receiver_video_codec = GlobalContext::instance().settings->loadStringParameter("receiver/video_codec");
  qInfo() << "Video Codec:" << receiver_video_codec.toUtf8().constData();
  if ("H264" == receiver_video_codec) {
    qInfo() << "Using Decoder: openh264dec";
    pipeline = gst_parse_launch(R"(
      udpsrc port=5000 caps="application/x-rtp, media=video, encoding-name=H264, payload=96" name=src !
      rtph264depay !
      h264parse !
      openh264dec !
      videoconvert !
      glupload !
      qml6glsink name=sink sync=false async=false
      )",
                                nullptr);
  } else if ("H265" == receiver_video_codec) {
    qCritical() << "H265 not supported";
  }
  return pipeline;
}

// Linux With Nvidia GPU (Hardware decoding)
// ----------------------------------------------------------------------------
GstElement *build_video_receive_pipeline_linux_nvidia_gpu() {

  GstElement *pipeline = nullptr;
  QString receiver_video_codec = GlobalContext::instance().settings->loadStringParameter("receiver/video_codec");
  qInfo() << "Video Codec:" << receiver_video_codec.toUtf8().constData();
  if ("H264" == receiver_video_codec) {
    qInfo() << "Using Decoder: nvv4l2decoder";
    pipeline = gst_parse_launch(R"(
      udpsrc port=5000 caps="application/x-rtp, media=video, encoding-name=H264, payload=96" name=src !
      rtph264depay !
      h264parse !
      nvv4l2decoder !
      nvvideoconvert !
      glupload !
      qml6glsink name=sink sync=false async=false
      )",
                                nullptr);
  } else if ("H265" == receiver_video_codec) {
    qInfo() << "Using Decoder: nvv4l2decoder";
    pipeline = gst_parse_launch(R"(
      udpsrc port=5000 caps="application/x-rtp, media=video, encoding-name=H265, payload=96" name=src !
      rtph265depay !
      h265parse !
      nvv4l2decoder !
      nvvideoconvert !
      glupload !
      qml6glsink name=sink sync=false async=false
      )",
                                nullptr);
  }
  return pipeline;
}

// Linux With Intel GPU (Hardware decoding)
// ----------------------------------------------------------------------------
GstElement *build_video_receive_pipeline_linux_intel_gpu() {

  GstElement *pipeline = nullptr;
  QString receiver_video_codec = GlobalContext::instance().settings->loadStringParameter("receiver/video_codec");
  qInfo() << "Video Codec:" << receiver_video_codec.toUtf8().constData();
  if ("H264" == receiver_video_codec) {
    qInfo() << "Using Decoder: vaapih264dec";
    pipeline = gst_parse_launch(R"(
      udpsrc port=5000 caps="application/x-rtp, media=video, encoding-name=H264, payload=96" name=src !
      rtph264depay !
      h264parse !
      vaapih264dec !
      videoconvert !
      glupload !
      qml6glsink name=sink sync=false async=false
      )",
                                nullptr);
  } else if ("H265" == receiver_video_codec) {
    qInfo() << "Using Decoder: vaapih265dec";
    pipeline = gst_parse_launch(R"(
      udpsrc port=5000 caps="application/x-rtp, media=video, encoding-name=H265, payload=96" name=src !
      rtph265depay !
      vaapih265dec !
      videoconvert !
      glupload !
      qml6glsink name=sink sync=false async=false
      )",
                                nullptr);
  }
  return pipeline;
}

// Macbook Pro
// ----------------------------------------------------------------------------
GstElement *build_video_receive_pipeline_mac() {

  GstElement *pipeline = nullptr;
  QString receiver_video_codec = GlobalContext::instance().settings->loadStringParameter("receiver/video_codec");

  qInfo() << "Video Codec:" << receiver_video_codec.toUtf8().constData();
  if ("H264" == receiver_video_codec) {
    qInfo() << "Using Decoder: vtdec_hw";
    pipeline = gst_parse_launch(R"(
      udpsrc port=5000 caps="application/x-rtp, media=video, encoding-name=H264, payload=96" name=src !
      rtph264depay !
      h264parse !
      vtdec_hw !
      videoconvert !
      glupload !
      qml6glsink name=sink sync=false async=false
      )",
                                nullptr);
  } else if ("H265" == receiver_video_codec) {
    qInfo() << "Using Decoder: vtdec_hw";
    pipeline = gst_parse_launch(R"(
      udpsrc port=5000 caps="application/x-rtp, media=video, encoding-name=H265, payload=96" name=src !
      rtph265depay !
      h265parse !
      vtdec_hw !
      videoconvert !
      glupload !
      qml6glsink name=sink sync=false async=false
      )",
                                nullptr);
  }
  return pipeline;
}
