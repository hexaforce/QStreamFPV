#!/bin/bash

# H264 =========================================================

# Transmitter
GST_DEBUG=3 gst-launch-1.0 -v -e \
    avfvideosrc ! \
    vtenc_h264_hw realtime=true allow-frame-reordering=false bitrate=500000 ! \
    h264parse config-interval=1 ! \
    video/x-h264, level=\(string\)5.2 ! \
    rtph264pay pt=96 ! \
    udpsink host=127.0.0.1 port=5000

# Level 6.1 is not supported
# https://github.com/GStreamer/gst-plugins-bad/blob/master/sys/applemedia/vtdec.c#L1028

# Receiver
GST_DEBUG=3 gst-launch-1.0 -v -e \
    udpsrc port=5000 caps="application/x-rtp, media=video, encoding-name=H264, payload=96" ! \
    rtph264depay ! \
    h264parse ! \
    vtdec_hw ! \
    videoconvert ! \
    osxvideosink sync=false async=false


# H265 =========================================================

# Transmitter
GST_DEBUG=3 gst-launch-1.0 -v -e \
    avfvideosrc ! \
    vtenc_h265_hw realtime=true allow-frame-reordering=false bitrate=500000 ! \
    h265parse config-interval=1 ! \
    video/x-h265, level=\(string\)5.2 ! \
    rtph265pay pt=96 ! \
    udpsink host=127.0.0.1 port=5000

# Receiver
GST_DEBUG=3 gst-launch-1.0 -v -e \
    udpsrc port=5000 caps="application/x-rtp, media=video, encoding-name=H265, payload=96" ! \
    rtph265depay ! \
    h265parse ! \
    vtdec_hw ! \
    videoconvert ! \
    osxvideosink sync=false async=false


# VP9 ==========================================================

# Transmitter
GST_DEBUG=3 gst-launch-1.0 -v -e \
    avfvideosrc ! \
    videoconvert ! \
    video/x-raw,format=I420 ! \
    queue ! \
    vp9enc noise-sensitivity=1 token-partitions=2 keyframe-max-dist=60 target-bitrate=500000 threads=4 cpu-used=8 deadline=1 buffer-initial-size=1000 buffer-optimal-size=2000 buffer-size=3000 ! \
    rtpvp9pay mtu=1200 ! \
    udpsink host=127.0.0.1 port=5000

# Receiver
GST_DEBUG=3 gst-launch-1.0 -v -e \
    udpsrc port=5000 caps="application/x-rtp, media=video, encoding-name=VP9, payload=96" ! \
    rtpvp9depay ! \
    vp9dec ! \
    videoconvert ! \
    glupload ! \
    glimagesink sync=false async=false


# AV1 ==========================================================

# Transmitter
GST_DEBUG=3 gst-launch-1.0 -v -e \
    avfvideosrc ! \
    videoconvert ! \
    video/x-raw,format=I420 ! \
    av1enc cpu-used=4 target-bitrate=2000 threads=4 keyframe-max-dist=30 ! \
    av1parse ! \
    rtpav1pay ! \
    udpsink host=127.0.0.1 port=5000

# Receiver
GST_DEBUG=3 gst-launch-1.0 -v -e \
    udpsrc port=5000 caps="application/x-rtp, media=video, encoding-name=AV1, payload=96" ! \
    rtpav1depay ! \
    av1parse ! \
    av1dec ! \
    videoconvert ! \
    osxvideosink sync=false async=false
