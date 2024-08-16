#ifndef RECEIVE_VIDEO_PIPELINES_H
#define RECEIVE_VIDEO_PIPELINES_H

#include "global_context.h"
#include <QDebug>
#include <QString>
#include <gst/gst.h>

GstElement *build_video_receive_pipeline_linux();
GstElement *build_video_receive_pipeline_linux_nvidia_gpu();
GstElement *build_video_receive_pipeline_linux_intel_gpu();
GstElement *build_video_receive_pipeline_mac();

#endif // RECEIVE_VIDEO_PIPELINES_H
