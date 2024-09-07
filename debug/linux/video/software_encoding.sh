#!/bin/bash


# H264 =========================================================

# Transmitter
GST_DEBUG=3 gst-launch-1.0 -v -e \
    v4l2src device=/dev/video1 ! \
    capsfilter caps=video/x-raw ! \
    videoconvert ! \
    capsfilter caps=video/x-raw,format=I420 ! \
    openh264enc complexity=2 bitrate=5000000 ! \
    rtph264pay config-interval=1 pt=96 ! \
    udpsink host=127.0.0.1 port=5000

# Receiver
GST_DEBUG=3 gst-launch-1.0 -v -e \
    udpsrc port=5000 caps="application/x-rtp, media=video, encoding-name=H264, payload=96" ! \
    rtph264depay ! \
    h264parse ! \
    openh264dec ! \
    autovideosink  sync=false


# H265 =========================================================

# Transmitter
GST_DEBUG=3 gst-launch-1.0 -v -e \
    v4l2src device=/dev/video1 ! \
    videoconvert ! \
    capsfilter caps=video/x-raw,format=I420 ! \
    x265enc speed-preset=ultrafast bitrate=500 ! \
    h265parse ! \
    rtph265pay config-interval=1 pt=96 ! \
    udpsink host=127.0.0.1 port=5000

# Receiver
GST_DEBUG=3 gst-launch-1.0 -v \
    udpsrc port=5000 caps="application/x-rtp, media=video, encoding-name=H265, payload=96" ! \
    rtph265depay ! \
    h265parse ! \
    libde265dec ! \
    videoconvert ! \
    autovideosink  sync=false


# VP9 ==========================================================

# Transmitter
GST_DEBUG=3 gst-launch-1.0 -v -e \
    v4l2src device=/dev/video1 ! \
    videoconvert ! \
    capsfilter caps=video/x-raw,format=I420 ! \
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
    autovideosink


# AV1 ==========================================================

# Transmitter
GST_DEBUG=3 gst-launch-1.0 -v -e \
    v4l2src device=/dev/video1 ! \
    videoconvert ! \
    capsfilter caps=video/x-raw,format=I420 ! \
    queue ! \
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
    autovideosink
