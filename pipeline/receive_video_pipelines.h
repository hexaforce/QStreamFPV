#ifndef RECEIVE_VIDEO_PIPELINES_H
#define RECEIVE_VIDEO_PIPELINES_H

#include "global_context.h"

GstElement *build_video_receive_pipeline_linux(VideoCodec videoCodec);
GstElement *build_video_receive_pipeline_linux_nvidia_gpu(VideoCodec videoCodec);
GstElement *build_video_receive_pipeline_linux_intel_gpu(VideoCodec videoCodec);
GstElement *build_video_receive_pipeline_mac(VideoCodec videoCodec);

#endif // RECEIVE_VIDEO_PIPELINES_H
