#include "receive_video_pipelines.h"

// Linux Software decoding
// ----------------------------------------------------------------------------
GstElement *build_video_receive_pipeline_linux(VideoCodec videoCodec) {
  QString pipeline_description;
  if (videoCodec == VideoCodec::H264 || videoCodec == VideoCodec::SupportedH264) {
    // ### H264
    pipeline_description = R"(
      udpsrc port=5000 caps="application/x-rtp, media=video, encoding-name=H264, payload=96" name=src !
      rtph264depay !
      h264parse !
      openh264dec !
      videoconvert !
      glupload !
      qml6glsink sync=false async=false name=sink
      )";
  } else if (videoCodec == VideoCodec::H265) {
    // ### H265
    pipeline_description = R"(
      udpsrc port=5000 caps="application/x-rtp, media=video, encoding-name=H265, payload=96" name=src !
      rtph265depay !
      h265parse !
      libde265dec !
      videoconvert !
      glupload !
      qml6glsink sync=false async=false name=sink
      )";
  }
  return gst_parse_launch(pipeline_description.toUtf8().constData(), nullptr);
}

// Linux With Nvidia GPU (Hardware decoding)
// ----------------------------------------------------------------------------
GstElement *build_video_receive_pipeline_linux_nvidia_gpu(VideoCodec videoCodec) {
  QString pipeline_description;
  if (videoCodec == VideoCodec::H264 || videoCodec == VideoCodec::SupportedH264) {
    // ### H264
    pipeline_description = R"(
      udpsrc port=5000 caps="application/x-rtp, media=video, encoding-name=H264, payload=96" name=src !
      queue max-size-buffers=0 max-size-time=0 max-size-bytes=0 !
      rtph264depay !
      h264parse !
      nvh264dec !
      videoconvert !
      glupload !
      qml6glsink sync=false async=false name=sink
      )";
  } else if (videoCodec == VideoCodec::H265) {
    // ### H265
    pipeline_description = R"(
      udpsrc port=5000 caps="application/x-rtp, media=video, encoding-name=H265, payload=96" name=src !
      queue max-size-buffers=0 max-size-time=0 max-size-bytes=0 !
      rtph265depay !
      h265parse !
      nvh265dec !
      videoconvert !
      glupload !
      qml6glsink sync=false async=false name=sink
      )";
  }
  return gst_parse_launch(pipeline_description.toUtf8().constData(), nullptr);
}

// Linux With Intel GPU (Hardware decoding)
// ----------------------------------------------------------------------------
GstElement *build_video_receive_pipeline_linux_intel_gpu(VideoCodec videoCodec) {
  QString pipeline_description;
  if (videoCodec == VideoCodec::H264 || videoCodec == VideoCodec::SupportedH264) {
    // ### H264
    pipeline_description = R"(
      udpsrc port=5000 caps="application/x-rtp, media=video, encoding-name=H264, payload=96" !
      rtph264depay !
      h264parse !
      vah264dec !
      videoconvert !
      glupload !
      qml6glsink sync=false async=false name=sink
      )";
  } else if (videoCodec == VideoCodec::H265) {
    // ### H265
    pipeline_description = R"(
      udpsrc port=5000 caps="application/x-rtp, media=video, encoding-name=H265, payload=96" !
      rtph265depay !
      h265parse !
      vah265dec !
      videoconvert !
      glupload !
      qml6glsink sync=false async=false name=sink
      )";
  }
  return gst_parse_launch(pipeline_description.toUtf8().constData(), nullptr);
}

// Mac
// ----------------------------------------------------------------------------
GstElement *build_video_receive_pipeline_mac(VideoCodec videoCodec) {
  QString pipeline_description;
  if (videoCodec == VideoCodec::H264 || videoCodec == VideoCodec::SupportedH264) {
    // ### H264
    pipeline_description = R"(
      udpsrc port=5000 caps="application/x-rtp, media=video, encoding-name=H264, payload=96" name=src !
      rtph264depay !
      h264parse !
      vtdec_hw !
      videoconvert !
      glupload !
      qml6glsink sync=false async=false name=sink
      )";
  } else if (videoCodec == VideoCodec::H265) {
    // ### H265
    pipeline_description = R"(
      udpsrc port=5000 caps="application/x-rtp, media=video, encoding-name=H265, payload=96" name=src !
      rtph265depay !
      h265parse !
      vtdec_hw !
      videoconvert !
      glupload !
      qml6glsinksync=false async=false name=sink 
      )";
  }
  return gst_parse_launch(pipeline_description.toUtf8().constData(), nullptr);
}
