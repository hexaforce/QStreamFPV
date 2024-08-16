#!/bin/bash

# Level 6.1 is not supported
# https://github.com/GStreamer/gst-plugins-bad/blob/master/sys/applemedia/vtdec.c#L1028

GST_DEBUG=3 gst-launch-1.0 -v -e \
    avfvideosrc ! \
    vtenc_h264_hw realtime=true allow-frame-reordering=false bitrate=500000 ! \
    h264parse config-interval=1 ! \
    video/x-h264, level=\(string\)5.2 ! \
    rtph264pay pt=96 ! \
    udpsink host=127.0.0.1 port=5000

GST_DEBUG=3 gst-launch-1.0 -v -e \
    udpsrc port=5000 caps="application/x-rtp, media=video, encoding-name=H264, payload=96" ! \
    rtph264depay ! \
    h264parse ! \
    vtdec_hw ! \
    videoconvert ! \
    glupload ! \
    qml6glsink sync=false async=false
