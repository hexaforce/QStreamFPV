#include "transmit_video_pipelines.h"

// Linux Software encoding
// ----------------------------------------------------------------------------
GstElement *build_video_transmit_pipeline_linux() {
  GstElement *pipeline = nullptr;
  QString transmitter_video_codec = GlobalContext::instance().settings->loadStringParameter("transmitter/video_codec");
  qInfo() << "Video Codec:" << transmitter_video_codec.toUtf8().constData();
  if ("H264" == transmitter_video_codec) {
    qInfo() << "Using Ecncorder: openh264enc";
    pipeline = gst_parse_launch(R"(
      v4l2src device=/dev/video0 name=src !
      video/x-raw,format=YUY2,width=1920,height=1080,framerate=30/1 !
      videoconvert !
      openh264enc complexity=2 bitrate=5000000 !
      rtph264pay config-interval=1 pt=96 !
      udpsink host=10.10.10.10 port=5000 name=sink
      )",
                                nullptr);
  } else if ("H265" == transmitter_video_codec) {
    qCritical() << "H265 not supported";
  }
  return pipeline;
}

// Linux With Nvidia GPU (Hardware encoding)
// ----------------------------------------------------------------------------
GstElement *build_video_transmit_pipeline_linux_nvidia_gpu() {
  GstElement *pipeline = nullptr;
  QString transmitter_video_codec = GlobalContext::instance().settings->loadStringParameter("transmitter/video_codec");
  qInfo() << "Video Codec:" << transmitter_video_codec.toUtf8().constData();
  if ("H264" == transmitter_video_codec) {
    qInfo() << "Using Ecncorder: nvv4l2h264enc";
    pipeline = gst_parse_launch(R"(
      v4l2src device=/dev/video0 name=src !
      video/x-raw,format=YUY2,width=1920,height=1080,framerate=30/1 !
      videoconvert !
      nvvideoconvert !
      nvv4l2h264enc bitrate=5000000 !
      rtph264pay config-interval=1 pt=96 !
      udpsink host=10.10.10.10 port=5000 name=sink
      )",
                                nullptr);
  } else if ("H265" == transmitter_video_codec) {
    qInfo() << "Using Ecncorder: nvv4l2h265enc";
    pipeline = gst_parse_launch(R"(
      v4l2src device=/dev/video0 name=src !
      video/x-raw,format=YUY2,width=1920,height=1080,framerate=30/1 !
      videoconvert !
      nvvideoconvert !
      nvv4l2h265enc bitrate=5000000 !
      rtph265pay config-interval=1 pt=96 !
      udpsink host=10.10.10.10 port=5000 name=sink
      )",
                                nullptr);
  }
  return pipeline;
}

// Linux With Intel GPU (Hardware encoding)
// ----------------------------------------------------------------------------
GstElement *build_video_transmit_pipeline_linux_intel_gpu() {
  GstElement *pipeline = nullptr;
  QString transmitter_video_codec = GlobalContext::instance().settings->loadStringParameter("transmitter/video_codec");
  qInfo() << "Video Codec:" << transmitter_video_codec.toUtf8().constData();
  if ("H264" == transmitter_video_codec) {
    qInfo() << "Using Ecncorder: vaapih264enc";
    pipeline = gst_parse_launch(R"(
      v4l2src device=/dev/video1 name=src !
      video/x-raw,format=YUY2,width=1280,height=720,framerate=10/1 !
      videoconvert !
      vaapih264enc !
      rtph264pay config-interval=1 pt=96 !
      udpsink host=10.10.10.10 port=5000 name=sink
      )",
                                nullptr);
  } else if ("H265" == transmitter_video_codec) {
    qInfo() << "Using Ecncorder: vaapih265enc";
    pipeline = gst_parse_launch(R"(
      v4l2src device=/dev/video1 name=src !
      video/x-raw,format=YUY2,width=1280,height=720,framerate=10/1 !
      videoconvert !
      vaapih265enc !
      rtph265pay config-interval=1 pt=96 !
      udpsink host=10.10.10.10 port=5000 name=sink
      )",
                                nullptr);
  }
  return pipeline;
}

// Macbook Pro
// ----------------------------------------------------------------------------
GstElement *build_video_transmit_pipeline_mac() {
  GstElement *pipeline = nullptr;
  QString transmitter_video_codec = GlobalContext::instance().settings->loadStringParameter("transmitter/video_codec");
  qInfo() << "Video Codec:" << transmitter_video_codec.toUtf8().constData();
  if ("H264" == transmitter_video_codec) {
    qInfo() << "Using Ecncorder: vtenc_h264_hw";
    pipeline = gst_parse_launch(R"(
      avfvideosrc name=src !
      vtenc_h264_hw realtime=true allow-frame-reordering=false bitrate=500000 !
      h264parse config-interval=1 !
      video/x-h264, level=\(string\)5.2 !
      rtph264pay pt=96 !
      udpsink host=10.10.10.10 port=5000 name=sink
      )",
                                nullptr);
  } else if ("H265" == transmitter_video_codec) {
    qInfo() << "Using Ecncorder: vtenc_h265_hw";
    pipeline = gst_parse_launch(R"(
      avfvideosrc name=src !
      vtenc_h265_hw realtime=true allow-frame-reordering=false bitrate=500000 !
      h265parse config-interval=1 !
      video/x-h265, level=\(string\)5.2 !
      rtph265pay pt=96 !
      udpsink host=10.10.10.10 port=5000 name=sink
      )",
                                nullptr);
  }
  return pipeline;
}

// Jetson Nano
// ----------------------------------------------------------------------------
GstElement *build_video_transmit_pipeline_jetson() {
  GstElement *pipeline = nullptr;
  QString transmitter_video_codec = GlobalContext::instance().settings->loadStringParameter("transmitter/video_codec");
  qInfo() << "Video Codec:" << transmitter_video_codec.toUtf8().constData();
  if ("H264" == transmitter_video_codec) {
    qInfo() << "Using Ecncorder: omxh264enc";
    pipeline = gst_parse_launch(R"(
      nvarguscamerasrc name=src !
      video/x-raw\(memory:NVMM\),width=1920,height=1080,framerate=30/1 !
      omxh264enc bitrate=5000000 control-rate=variable !
      rtph264pay config-interval=1 pt=96 !
      udpsink host=10.10.10.10 port=5000 name=sink
      )",
                                nullptr);
  } else if ("H265" == transmitter_video_codec) {
    qInfo() << "Using Ecncorder: omxh265enc";
    pipeline = gst_parse_launch(R"(
      nvarguscamerasrc name=src !
      video/x-raw\(memory:NVMM\),width=1920,height=1080,framerate=30/1 !
      omxh265enc bitrate=5000000 control-rate=variable !
      rtph265pay config-interval=1 pt=96 !
      udpsink host=10.10.10.10 port=5000 name=sink
      )",
                                nullptr);
  }
  return pipeline;
}

// Raspberry Pi 4
// ----------------------------------------------------------------------------
GstElement *build_video_transmit_pipeline_rpi4() {
  GstElement *pipeline = nullptr;
  QString transmitter_video_codec = GlobalContext::instance().settings->loadStringParameter("transmitter/video_codec");
  qInfo() << "Video Codec:" << transmitter_video_codec.toUtf8().constData();
  if ("H264" == transmitter_video_codec) {
    qInfo() << "Using Ecncorder: v4l2h264enc";
    pipeline = gst_parse_launch(R"(
      v4l2src device=/dev/video0 name=src !
      video/x-raw,format=YUY2,width=1920,height=1080,framerate=30/1 !
      v4l2convert !
      v4l2h264enc bitrate=5000000 !
      rtph264pay config-interval=1 pt=96 !
      udpsink host=10.10.10.10 port=5000 name=sink
      )",
                                nullptr);
  } else if ("H265" == transmitter_video_codec) {
    qInfo() << "Using Ecncorder: v4l2h265enc";
    pipeline = gst_parse_launch(R"(
      v4l2src device=/dev/video0 name=src !
      video/x-raw,format=YUY2,width=1920,height=1080,framerate=30/1 !
      v4l2convert !
      v4l2h265enc bitrate=5000000 !
      rtph265pay config-interval=1 pt=96 !
      udpsink host=10.10.10.10 port=5000 name=sink
      )",
                                nullptr);
  }
  return pipeline;
}

// Raspberry Pi 5
// ----------------------------------------------------------------------------
GstElement *build_video_transmit_pipeline_rpi5() {
  GstElement *pipeline = nullptr;
  QString transmitter_video_codec = GlobalContext::instance().settings->loadStringParameter("transmitter/video_codec");
  qInfo() << "Video Codec:" << transmitter_video_codec.toUtf8().constData();
  if ("H264" == transmitter_video_codec) {
    qInfo() << "Using Ecncorder: openh264enc";
    pipeline = gst_parse_launch(R"(
      libcamerasrc name=src !
      video/x-raw,format=YUY2,width=1920,height=1080,framerate=30/1 !
      videoconvert !
      openh264enc complexity=2 bitrate=5000000 !
      rtph264pay config-interval=1 pt=96 !
      udpsink host=10.10.10.10 port=5000 name=sink
      )",
                                nullptr);
  } else if ("H265" == transmitter_video_codec) {
    qCritical() << "H265 not supported";
  }
  return pipeline;
}
