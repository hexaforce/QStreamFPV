#ifndef AUDIO_PIPELINES_H
#define AUDIO_PIPELINES_H

#include "global_context.h"

GstElement *build_audio_transmit_pipeline_linux();
GstElement *build_audio_transmit_pipeline_mac();
GstElement *build_audio_receive_pipeline_linux();
GstElement *build_audio_receive_pipeline_mac();

#endif // AUDIO_PIPELINES_H
