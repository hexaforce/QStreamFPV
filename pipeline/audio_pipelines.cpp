#include "audio_pipelines.h"

// Linux ALSA
// ----------------------------------------------------------------------------
GstElement *build_audio_transmit_pipeline_linux() {
  QString pipeline_description = R"(
    alsasrc !
    audioconvert !
    audioresample !
    audio/x-raw,rate=48000,channels=2 !
    opusenc bitrate=64000 !
    rtpopuspay !
    udpsink host=127.0.0.1 port=5001 name=sink
    )";
  return gst_parse_launch(pipeline_description.toUtf8().constData(), nullptr);
}

// Mac OSX
// ----------------------------------------------------------------------------
GstElement *build_audio_transmit_pipeline_mac() {
  QString pipeline_description = R"(
    osxaudiosrc !
    audioconvert !
    audioresample !
    audio/x-raw,rate=48000,channels=2 !
    opusenc bitrate=64000 !
    rtpopuspay !
    udpsink host=127.0.0.1 port=5001 name=sink
    )";
  return gst_parse_launch(pipeline_description.toUtf8().constData(), nullptr);
}

// Linux ALSA
// ----------------------------------------------------------------------------
GstElement *build_audio_receive_pipeline_linux() {
  QString pipeline_description = R"(
    udpsrc port=5001 caps="application/x-rtp, media=audio, encoding-name=OPUS, payload=96" name=src !
    rtpopusdepay !
    opusdec !
    alsasink
    )";
  return gst_parse_launch(pipeline_description.toUtf8().constData(), nullptr);
}

// Mac OSX
// ----------------------------------------------------------------------------
GstElement *build_audio_receive_pipeline_mac() {
  QString pipeline_description = R"(
    udpsrc port=5001 caps="application/x-rtp, media=audio, encoding-name=OPUS, payload=96" name=src !
    rtpopusdepay !
    opusdec !
    osxaudiosink
    )";
  return gst_parse_launch(pipeline_description.toUtf8().constData(), nullptr);
}
