#!/bin/bash

# H264 =========================================================

# Transmitter

# ------ nvv4l2h264enc
GST_DEBUG=3 gst-launch-1.0 -v -e \
    nvarguscamerasrc ! \
    capsfilter caps=video/x-raw\(memory:NVMM\)  ! \
    nvv4l2h264enc bitrate=5000000 ! \
    rtph264pay config-interval=1 ! \
    udpsink host=192.168.86.109 port=5000

# ------ omxh264enc
GST_DEBUG=3 gst-launch-1.0 -v -e \
    nvarguscamerasrc ! \
    capsfilter caps=video/x-raw\(memory:NVMM\)  ! \
    omxh264enc bitrate=5000000 ! \
    rtph264pay config-interval=1 ! \
    udpsink host=127.0.0.1 port=5000


# H265 =========================================================

# Transmitter

# ------ nvv4l2h265enc
GST_DEBUG=3 gst-launch-1.0 -v -e \
    nvarguscamerasrc ! \
    capsfilter caps=video/x-raw\(memory:NVMM\)  ! \
    nvv4l2h265enc bitrate=5000000 ! \
    rtph265pay config-interval=1 ! \
    udpsink host=127.0.0.1 port=5000

# ------ omxh265enc
GST_DEBUG=3 gst-launch-1.0 -v -e \
    nvarguscamerasrc ! \
    capsfilter caps=video/x-raw\(memory:NVMM\)  ! \
    omxh265enc bitrate=5000000 ! \
    rtph265pay config-interval=1 ! \
    udpsink host=127.0.0.1 port=5000

# VP8 ==========================================================

# Transmitter
GST_DEBUG=3 gst-launch-1.0 -v -e \
    nvarguscamerasrc ! \
    capsfilter caps=video/x-raw\(memory:NVMM\)  ! \
    nvv4l2vp8enc ! \
    rtpvp8pay ! \
    udpsink host=127.0.0.1 port=5000
    
# VP9 ==========================================================

# Jetson Nano doesn't support VP9

# Transmitter
# GST_DEBUG=3 gst-launch-1.0 -v -e \
#     nvarguscamerasrc ! \
#     video/x-raw\(memory:NVMM\) ! \
#     nvv4l2vp9enc ! \
#     rtpvp9pay ! \
#     udpsink host=10.10.10.10 port=5000

# GST_DEBUG=3 gst-launch-1.0 -v -e \
#     nvarguscamerasrc ! \
#     video/x-raw\(memory:NVMM\) ! \
#     omxvp9enc ! \
#     rtpvp9pay ! \
#     udpsink host=10.10.10.10 port=5000
