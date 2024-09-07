#ifndef TRANSMIT_VIDEO_PIPELINES_H
#define TRANSMIT_VIDEO_PIPELINES_H

#include "global_context.h"

GstCaps *build_camera_capture();
GstElement *build_video_transmit_pipeline_h264_compatible_camera();
GstElement *build_video_transmit_pipeline_linux(VideoCodec videoCodec);
GstElement *build_video_transmit_pipeline_linux_nvidia_gpu(VideoCodec videoCodec);
GstElement *build_video_transmit_pipeline_linux_intel_gpu(VideoCodec videoCodec);
GstElement *build_video_transmit_pipeline_mac(VideoCodec videoCodec);
GstElement *build_video_transmit_pipeline_jetson(VideoCodec videoCodec);
GstElement *build_video_transmit_pipeline_raspberrypi4(VideoCodec videoCodec);
GstElement *build_video_transmit_pipeline_raspberrypi5(VideoCodec videoCodec);

#endif // TRANSMIT_VIDEO_PIPELINES_H
