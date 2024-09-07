#include "transmit_video_pipelines.h"

// Build Capture Setting
// ----------------------------------------------------------------------------
GstCaps *build_camera_capture() {
  auto &ctx = GlobalContext::instance();

  const char *format = ctx.capsFormat.toUtf8().constData();
  int width = ctx.videoResolution.width();
  int height = ctx.videoResolution.height();
  int framerate_num = ctx.videoFramerate.split("/").at(0).toInt();
  int framerate_denom = ctx.videoFramerate.split("/").at(1).toInt();

  if (ctx.videoCodec == VideoCodec::SupportedH264) {
    return gst_caps_new_simple("video/x-h264", "width", G_TYPE_INT, width, "height", G_TYPE_INT, height, "framerate", GST_TYPE_FRACTION, framerate_num, framerate_denom, NULL);
  }
  if (ctx.osType == OSType::JetsonNano) {
    return gst_caps_new_simple("video/x-raw(memory:NVMM)", "format", G_TYPE_STRING, format, "width", G_TYPE_INT, width, "height", G_TYPE_INT, height, "framerate", GST_TYPE_FRACTION, framerate_num, framerate_denom, NULL);
  }
  return gst_caps_new_simple("video/x-raw", "format", G_TYPE_STRING, format, "width", G_TYPE_INT, width, "height", G_TYPE_INT, height, "framerate", GST_TYPE_FRACTION, framerate_num, framerate_denom, NULL);
}

// H264 Compatible Camera
// ----------------------------------------------------------------------------
GstElement *build_video_transmit_pipeline_h264_compatible_camera() {
  QString pipeline_description = R"(
      v4l2src device=/dev/video0 name=src !
      capsfilter caps=video/x-h264 name=filter !
      h264parse !
      rtph264pay config-interval=1 pt=96 !
      udpsink host=127.0.0.1 port=5000 name=sink
      )";
  return gst_parse_launch(pipeline_description.toUtf8().constData(), nullptr);
}

// Linux Software encoding
// ----------------------------------------------------------------------------
GstElement *build_video_transmit_pipeline_linux(VideoCodec videoCodec) {
  QString pipeline_description;
  if (videoCodec == VideoCodec::H264) {
    // ### H264
    pipeline_description = R"(
      v4l2src device=/dev/video0 name=src !
      capsfilter caps=video/x-raw name=filter !
      videoconvert !
      capsfilter caps=video/x-raw,format=I420 !
      openh264enc complexity=2 bitrate=5000000 !
      rtph264pay config-interval=1 pt=96 !
      udpsink host=127.0.0.1 port=5000 name=sink
      )";
  } else if (videoCodec == VideoCodec::H265) {
    // ### H265
    pipeline_description = R"(
      v4l2src device=/dev/video0 name=src !
      capsfilter caps=video/x-raw name=filter !
      videoconvert !
      capsfilter caps=video/x-raw,format=I420 !
      x265enc !
      h265parse !
      rtph265pay config-interval=1 pt=96 !
      udpsink host=127.0.0.1 port=5000 name=sink
      )";
  }
  return gst_parse_launch(pipeline_description.toUtf8().constData(), nullptr);
}

// Linux With Nvidia GPU (Hardware encoding)
// ----------------------------------------------------------------------------
GstElement *build_video_transmit_pipeline_linux_nvidia_gpu(VideoCodec videoCodec) {
  QString pipeline_description;
  if (videoCodec == VideoCodec::H264) {
    // ### H264
    pipeline_description = R"(
      v4l2src device=/dev/video1 name=src !
      capsfilter caps=video/x-raw name=filter !
      videoconvert !
      nvh264enc bitrate=5000000 !
      rtph264pay config-interval=1 pt=96 !
      udpsink host=127.0.0.1 port=5000 name=sink
      )";
  } else if (videoCodec == VideoCodec::H265) {
    // ### H265
    pipeline_description = R"(
      v4l2src device=/dev/video1 name=src !
      capsfilter caps=video/x-raw name=filter !
      videoconvert !
      nvh265enc bitrate=5000000 !
      rtph265pay config-interval=1 pt=96 !
      udpsink host=127.0.0.1 port=5000 name=sink
      )";
  }
  return gst_parse_launch(pipeline_description.toUtf8().constData(), nullptr);
}

// Linux With Intel GPU (Hardware encoding)
// ----------------------------------------------------------------------------
GstElement *build_video_transmit_pipeline_linux_intel_gpu(VideoCodec videoCodec) {
  QString pipeline_description;
  if (videoCodec == VideoCodec::H264) {
    // ### H264
    pipeline_description = R"(
      v4l2src device=/dev/video1 name=src !
      capsfilter caps=video/x-raw name=filter !
      videoconvert !
      vah264lpenc !
      rtph264pay config-interval=1 pt=96 !
      udpsink host=127.0.0.1 port=5000 name=sink
      )";
  } else if (videoCodec == VideoCodec::H265) {
    // ### H265
    pipeline_description = R"(
      v4l2src device=/dev/video1 name=src !
      capsfilter caps=video/x-raw name=filter !
      videoconvert !
      vah265lpenc !
      rtph265pay config-interval=1 pt=96 !
      udpsink host=127.0.0.1 port=5000 name=sink
      )";
  }
  return gst_parse_launch(pipeline_description.toUtf8().constData(), nullptr);
}

// Mac
// ----------------------------------------------------------------------------
GstElement *build_video_transmit_pipeline_mac(VideoCodec videoCodec) {
  QString pipeline_description;
  if (videoCodec == VideoCodec::H264) {
    // ### H264
    pipeline_description = R"(
      avfvideosrc name=src !
      capsfilter caps=video/x-raw name=filter !
      vtenc_h264_hw realtime=true allow-frame-reordering=false bitrate=500000 !
      h264parse config-interval=1 !
      capsfilter caps=video/x-h265, level=\(string\)5.2 !
      rtph264pay pt=96 !
      udpsink host=127.0.0.1 port=5000 name=sink
      )";
  } else if (videoCodec == VideoCodec::H265) {
    // ### H265
    pipeline_description = R"(
      avfvideosrc name=src !
      capsfilter caps=video/x-raw name=filter !
      vtenc_h265_hw realtime=true allow-frame-reordering=false bitrate=500000 !
      h265parse config-interval=1 !
      capsfilter caps=video/x-h265, level=\(string\)5.2 !
      rtph265pay pt=96 !
      udpsink host=127.0.0.1 port=5000 name=sink
      )";
  }
  return gst_parse_launch(pipeline_description.toUtf8().constData(), nullptr);
}

// Jetson Nano
// ----------------------------------------------------------------------------
GstElement *build_video_transmit_pipeline_jetson(VideoCodec videoCodec) {
  QString pipeline_description;
  if (videoCodec == VideoCodec::H264) {
    // ### H264
    pipeline_description = R"(
      nvarguscamerasrc name=src !
      capsfilter caps=video/x-raw\(memory:NVMM\) name=filter !
      queue ! 
      nvv4l2h264enc bitrate=5000000 !
      rtph264pay config-interval=1 !
      udpsink host=127.0.0.1 port=5000 name=sink
      )";
  } else if (videoCodec == VideoCodec::H265) {
    // ### H265
    pipeline_description = R"(
      nvarguscamerasrc name=src !
      capsfilter caps=video/x-raw\(memory:NVMM\) name=filter !
      queue ! 
      nvv4l2h265enc bitrate=5000000 !
      rtph265pay config-interval=1 !
      udpsink host=127.0.0.1 port=5000 name=sink
      )";
  }
  return gst_parse_launch(pipeline_description.toUtf8().constData(), nullptr);
}

// Raspberry Pi 4
// ----------------------------------------------------------------------------
GstElement *build_video_transmit_pipeline_raspberrypi4(VideoCodec videoCodec) {
  QString pipeline_description;
  if (videoCodec == VideoCodec::H264) {
    // ### H264
    pipeline_description = R"(
        libcamerasrc name=src !
        capsfilter caps=video/x-raw,format=YUY2,width=1920,height=1080 name=filter !
        videoconvert !
        capsfilter caps=video/x-raw,format=I420 !
        queue ! 
        v4l2h264enc !
        capsfilter caps=video/x-h264,level=\(string\)5.1 !
        rtph264pay config-interval=1 pt=96 !
        udpsink host=127.0.0.1 port=5000 name=sink
        )";
  } else if (videoCodec == VideoCodec::H265) {
    // ### H265
    pipeline_description = R"(
        libcamerasrc name=src !
        capsfilter caps=video/x-raw,format=YUY2,width=1920,height=1080 name=filter !
        videoconvert !
        capsfilter caps=video/x-raw,format=I420 !
        queue ! 
        v4l2h265enc !
        capsfilter caps=video/x-h265,level=\(string\)5.1 !
        rtph265pay config-interval=1 pt=96 !
        udpsink host=127.0.0.1 port=5000 name=sink
        )";
  }
  return gst_parse_launch(pipeline_description.toUtf8().constData(), nullptr);
}

// Raspberry Pi 5
// ----------------------------------------------------------------------------
GstElement *build_video_transmit_pipeline_raspberrypi5(VideoCodec videoCodec) {
  QString pipeline_description;
  if (videoCodec == VideoCodec::H264) {
    // ### H264
    pipeline_description = R"(
        libcamerasrc name=src !
        capsfilter caps=video/x-raw,format=YUY2,width=1920,height=1080 name=filter !
        videoconvert !
        capsfilter caps=video/x-raw,format=I420 !
        queue ! 
        openh264enc complexity=2 bitrate=5000000 !
        rtph264pay config-interval=1 pt=96 !
        udpsink host=127.0.0.1 port=5000 name=sink
        )";
  } else if (videoCodec == VideoCodec::H265) {
    // ### H265
    pipeline_description = R"(
        libcamerasrc name=src !
        capsfilter caps=video/x-raw,format=YUY2,width=1920,height=1080 name=filter !
        videoconvert !
        capsfilter caps=video/x-raw,format=I420 !
        queue ! 
        x265enc speed-preset=ultrafast bitrate=500 !
        h265parse !
        rtph265pay config-interval=1 pt=96 !
        udpsink host=127.0.0.1 port=5000 name=sink
        )";
  }
  return gst_parse_launch(pipeline_description.toUtf8().constData(), nullptr);
}
