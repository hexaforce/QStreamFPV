#ifndef TRANSMIT_VIDEO_PIPELINES_H
#define TRANSMIT_VIDEO_PIPELINES_H

#include "global_context.h"
#include <QDebug>
#include <QString>
#include <gst/gst.h>

GstElement *build_video_transmit_pipeline_linux();
GstElement *build_video_transmit_pipeline_linux_nvidia_gpu();
GstElement *build_video_transmit_pipeline_linux_intel_gpu();

GstElement *build_video_transmit_pipeline_mac();

GstElement *build_video_transmit_pipeline_jetson();
GstElement *build_video_transmit_pipeline_rpi4();
GstElement *build_video_transmit_pipeline_rpi5();

#endif // TRANSMIT_VIDEO_PIPELINES_H
