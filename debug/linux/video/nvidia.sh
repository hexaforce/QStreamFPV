#!/bin/bash

# nvtop

# H264 =========================================================

# ------ nvv4l2h264enc

# # Transmitter
# GST_DEBUG=3 gst-launch-1.0 -v -e \
#     v4l2src device=/dev/video0 ! \
#     capsfilter caps=video/x-raw,format=YUY2,width=1920,height=1080,framerate=30/1 ! \
#     videoconvert ! \
#     capsfilter caps=video/x-raw,format=I420,width=1920,height=1080,framerate=30/1 ! \
#     nvvideoconvert ! \
#     capsfilter caps=video/x-raw\(memory:NVMM\),format=I420,width=1920,height=1080,framerate=30/1 ! \
#     nvv4l2h264enc bitrate=5000000 ! \
#     rtph264pay config-interval=1 pt=96 ! \
#     udpsink host=127.0.0.1 port=5000

# # Receiver
# GST_DEBUG=3 gst-launch-1.0 -v -e \
#     udpsrc port=5000 caps="application/x-rtp, media=video, encoding-name=H264, payload=96" ! \
#     rtph264depay ! \
#     h264parse ! \
#     nvv4l2decoder ! \
#     nvvideoconvert ! \
#     glupload ! \
#     glimagesink sync=false async=false

# ------ nvh264enc

# Transmitter
GST_DEBUG=3 gst-launch-1.0 -v -e \
    v4l2src device=/dev/video1 ! \
    capsfilter caps=video/x-raw ! \
    videoconvert ! \
    nvh264enc bitrate=5000000 ! \
    rtph264pay config-interval=1 pt=96 ! \
    udpsink host=127.0.0.1 port=5000

# Receiver
GST_DEBUG=3 gst-launch-1.0 -v -e \
    udpsrc port=5000 caps="application/x-rtp, media=video, encoding-name=H264, payload=96" ! \
    queue max-size-buffers=0 max-size-time=0 max-size-bytes=0 ! \
    rtph264depay ! \
    h264parse ! \
    nvh264dec ! \
    autovideosink sync=false 

# H265 =========================================================

# ------ nvv4l2h265enc

# # Transmitter
# GST_DEBUG=3 gst-launch-1.0 -v -e \
#     v4l2src device=/dev/video0 ! \
#     capsfilter caps=video/x-raw,format=YUY2,width=1920,height=1080,framerate=30/1 ! \
#     videoconvert ! \
#     capsfilter caps=video/x-raw,format=I420,width=1920,height=1080,framerate=30/1 ! \
#     nvvideoconvert ! \
#     capsfilter caps=video/x-raw\(memory:NVMM\),format=I420,width=1920,height=1080,framerate=30/1 ! \
#     nvv4l2h265enc bitrate=5000000 ! \
#     rtph265pay config-interval=1 pt=96 ! \
#     udpsink host=127.0.0.1 port=5000

# # Receiver
# GST_DEBUG=3 gst-launch-1.0 -v -e \
#     udpsrc port=5000 caps="application/x-rtp, media=video, encoding-name=H265, payload=96" ! \
#     rtph265depay ! \
#     h265parse ! \
#     nvv4l2decoder ! \
#     nvvideoconvert ! \
#     glupload ! \
#     glimagesink sync=false async=false

# ------ nvh265enc

# Transmitter
GST_DEBUG=3 gst-launch-1.0 -v -e \
    v4l2src device=/dev/video1 ! \
    capsfilter caps=video/x-raw ! \
    videoconvert ! \
    nvh265enc bitrate=5000000 ! \
    rtph265pay config-interval=1 pt=96 ! \
    udpsink host=127.0.0.1 port=5000

# Receiver
GST_DEBUG=3 gst-launch-1.0 -v -e \
    udpsrc port=5000 caps="application/x-rtp, media=video, encoding-name=H265, payload=96" ! \
    queue max-size-buffers=0 max-size-time=0 max-size-bytes=0 ! \
    rtph265depay ! \
    h265parse ! \
    nvh265dec ! \
    autovideosink sync=false 
