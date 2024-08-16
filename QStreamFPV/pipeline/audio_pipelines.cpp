#include "audio_pipelines.h"

// Linux ALSA
// ----------------------------------------------------------------------------
GstElement *build_audio_transmit_pipeline_linux() {
  GstElement *pipeline;
  qInfo() << "Using opusenc";
  pipeline = gst_parse_launch(R"(
    alsasrc !
    audioconvert !
    audioresample !
    audio/x-raw,rate=48000,channels=2 !
    opusenc bitrate=64000 !
    rtpopuspay !
    udpsink host=10.10.10.10 port=5001 name=sink
    )",
                              nullptr);
  return pipeline;
}

// Macbook Pro OSX
// ----------------------------------------------------------------------------
GstElement *build_audio_transmit_pipeline_mac() {
  GstElement *pipeline;
  qInfo() << "Using opusenc";
  pipeline = gst_parse_launch(R"(
    osxaudiosrc !
    audioconvert !
    audioresample !
    audio/x-raw,rate=48000,channels=2 !
    opusenc bitrate=64000 !
    rtpopuspay !
    udpsink host=10.10.10.10 port=5001 name=sink
    )",
                              nullptr);
  return pipeline;
}

// Linux ALSA
// ----------------------------------------------------------------------------
GstElement *build_audio_receive_pipeline_linux() {
  GstElement *pipeline;
  qInfo() << "Using opusdec";
  pipeline = gst_parse_launch(R"(
    udpsrc port=5001 caps="application/x-rtp, media=audio, encoding-name=OPUS, payload=96" name=src !
    rtpopusdepay !
    opusdec !
    alsasink
    )",
                              nullptr);
  return pipeline;
}

// Macbook Pro OSX
// ----------------------------------------------------------------------------
GstElement *build_audio_receive_pipeline_mac() {
  GstElement *pipeline;
  qInfo() << "Using opusdec";
  pipeline = gst_parse_launch(R"(
    udpsrc port=5001 caps="application/x-rtp, media=audio, encoding-name=OPUS, payload=96" name=src !
    rtpopusdepay !
    opusdec !
    osxaudiosink
    )",
                              nullptr);
  return pipeline;
}
