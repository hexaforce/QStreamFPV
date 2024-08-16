#!/bin/bash

# sudo intel_gpu_top

GST_DEBUG=3 gst-launch-1.0 -v -e \
    v4l2src device=/dev/video1 ! \
    video/x-raw,format=YUY2,width=1280,height=720,framerate=10/1 ! \
    videoconvert ! \
    vaapih264enc! \
    rtph264pay config-interval=1 pt=96 ! \
    udpsink host=10.10.10.10 port=5000

GST_DEBUG=3 gst-launch-1.0 -v -e \
    udpsrc port=5000 caps="application/x-rtp, media=video, encoding-name=H264, payload=96" ! \
    rtph264depay ! \
    vaapih264dec ! \
    videoconvert ! \
    glupload ! \
    glimagesink sync=false async=false
