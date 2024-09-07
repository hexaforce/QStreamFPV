#!/bin/bash

# sudo intel_gpu_top

# H264 =========================================================

# va:  vah264lpenc: VA-API H.264 Low Power Encoder in Intel(R) Gen Graphics
GST_DEBUG=3 gst-launch-1.0 -v -e \
    v4l2src device=/dev/video0 ! \
    capsfilter caps=video/x-raw ! \
    videoconvert ! \
    vah264lpenc ! \
    rtph264pay config-interval=1 pt=96 ! \
    udpsink host=127.0.0.1 port=5000

# va:  vah264dec: VA-API H.264 Decoder in Intel(R) Gen Graphics
GST_DEBUG=3 gst-launch-1.0 -v -e \
    udpsrc port=5000 caps="application/x-rtp, media=video, encoding-name=H264, payload=96" ! \
    rtph264depay ! \
    h264parse ! \
    vah264dec ! \
    videoconvert ! \
    autovideosink sync=false 


# H265 =========================================================

# va:  vah265lpenc: VA-API H.265 Low Power Encoder in Intel(R) Gen Graphics
GST_DEBUG=3 gst-launch-1.0 -v -e \
    v4l2src device=/dev/video0 ! \
    capsfilter caps=video/x-raw ! \
    videoconvert ! \
    vah265lpenc ! \
    rtph265pay config-interval=1 pt=96 ! \
    udpsink host=127.0.0.1 port=5000

# va:  vah265dec: VA-API H.265 Decoder in Intel(R) Gen Graphics
GST_DEBUG=3 gst-launch-1.0 -v -e \
    udpsrc port=5000 caps="application/x-rtp, media=video, encoding-name=H265, payload=96" ! \
    rtph265depay ! \
    h265parse ! \
    vah265dec ! \
    videoconvert ! \
    autovideosink sync=false 
