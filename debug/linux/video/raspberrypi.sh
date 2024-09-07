#!/bin/bash

# H264 =========================================================

# Transmitter

# ------ videotestsrc
GST_DEBUG=3 gst-launch-1.0 -v -e \
 videotestsrc ! \
 v4l2h264enc ! \
 capsfilter caps=video/x-h264,level=\(string\)5.1 ! \
 rtph264pay config-interval=1 pt=96 ! \
 udpsink host=127.0.0.1 port=5000

# ------ openh264enc
GST_DEBUG=3 gst-launch-1.0 -v -e \
 libcamerasrc ! \
 capsfilter caps=video/x-raw,format=YUY2,width=1920,height=1080 ! \
 videoconvert ! \
 openh264enc complexity=2 bitrate=5000000 ! \
 rtph264pay config-interval=1 pt=96 ! \
 udpsink host=10.10.10.93 port=5000

# ------ x265enc
GST_DEBUG=3 gst-launch-1.0 -v -e \
 libcamerasrc ! \
 capsfilter caps=video/x-raw,format=YUY2,width=1920,height=1080 ! \
 videoconvert ! \
 capsfilter caps=video/x-raw,format=I420 ! \
 x264enc bitrate=5000000  ! \
 rtph264pay config-interval=1 pt=96 ! \
 udpsink host=127.0.0.1 port=5000

GST_DEBUG=3 gst-launch-1.0 -v -e \
 libcamerasrc ! \
 capsfilter caps=video/x-raw,format=YUY2,width=1920,height=1080 ! \
 videoconvert ! \
 capsfilter caps=video/x-raw,format=I420 ! \
 x265enc speed-preset=ultrafast bitrate=500 ! \
 h265parse ! \
 rtph265pay config-interval=1 pt=96 ! \
 udpsink host=127.0.0.1 port=5000

# ------ v4l2h264enc
	# 1280 x 720
GST_DEBUG=3 gst-launch-1.0 -v -e \
 libcamerasrc ! \
 capsfilter caps=video/x-raw,format=YUY2,width=1280,height=720 ! \
 videoconvert ! \
 queue ! \
 capsfilter caps=video/x-raw,format=I420 ! \
 v4l2h264enc ! \
 capsfilter caps=video/x-h264,level=\(string\)5.1 ! \
 rtph264pay config-interval=1 pt=96 ! \
 udpsink host=127.0.0.1 port=5000

	# 1920 x 1080
GST_DEBUG=3 gst-launch-1.0 -v -e \
 libcamerasrc ! \
 capsfilter caps=video/x-raw,format=YUY2,width=1920,height=1080 ! \
 videoconvert ! \
 capsfilter caps=video/x-raw,format=I420 ! \
 v4l2h264enc ! \
 capsfilter caps=video/x-h264,level=\(string\)5.1 ! \
 rtph264pay config-interval=1 pt=96 ! \
 udpsink host=127.0.0.1 port=5000


