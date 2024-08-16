#!/bin/bash

GST_DEBUG=3 gst-launch-1.0 -v -e \
    libcamerasrc ! \
    video/x-raw,format=YUY2,width=1920,height=1080,framerate=30/1 ! \
    videoconvert ! \
    openh264enc complexity=2 bitrate=5000000 ! \
    rtph264pay config-interval=1 pt=96 ! \
    udpsink host=10.10.10.10 port=5000
