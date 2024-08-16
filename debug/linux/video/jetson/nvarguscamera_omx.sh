#!/bin/bash

GST_DEBUG=3 gst-launch-1.0 -v -e \
    nvarguscamerasrc ! \
    "video/x-raw(memory:NVMM),width=1920,height=1080,framerate=30/1" ! \
    omxh264enc bitrate=5000000 control-rate=variable ! \
    rtph264pay config-interval=1 pt=96 ! \
    udpsink host=10.10.10.10 port=5000

GST_DEBUG=3 gst-launch-1.0 -v -e \
    nvarguscamerasrc ! \
    video/x-raw\(memory:NVMM\),width=1920,height=1080,framerate=30/1 ! \
    omxh265enc bitrate=5000000 control-rate=variable ! \
    rtph265pay config-interval=1 pt=96 ! \
    udpsink host=10.10.10.10 port=5000
