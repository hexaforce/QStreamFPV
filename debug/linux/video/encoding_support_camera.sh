#!/bin/bash


# H264 =========================================================

# Transmitter
GST_DEBUG=3 gst-launch-1.0 -v -e \
    v4l2src device=/dev/video1 ! \
    capsfilter caps=video/x-h264,width=1920,height=1080 ! \
    h264parse ! \
    rtph264pay config-interval=1 pt=96 ! \
    udpsink host=127.0.0.1 port=5000

# Receiver
GST_DEBUG=3 gst-launch-1.0 -v -e \
    udpsrc port=5000 caps="application/x-rtp, media=video, encoding-name=H264, payload=96" ! \
    rtph264depay ! \
    h264parse ! \
    avdec_h264 ! \
    autovideosink

# H265 =========================================================

# Transmitter
GST_DEBUG=3 gst-launch-1.0 -v -e \
    v4l2src device=/dev/video0 ! \
    capsfilter caps=video/x-h265 ! \
    rtph265pay ! \
    udpsink host=127.0.0.1 port=5000

# Receiver
GST_DEBUG=3 gst-launch-1.0 -v -e \
    udpsrc port=5000 caps="application/x-rtp, media=video, encoding-name=H265, payload=96" ! \
    rtph265depay ! \
    h265parse ! \
    avdec_h265 ! \
    autovideosink

