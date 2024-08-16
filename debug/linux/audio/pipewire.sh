#!/bin/bash

GST_DEBUG=3 gst-launch-1.0 -v -e \
    pipewiresrc ! \
    audioconvert ! \
    audioresample ! \
    opusenc ! \
    rtpopuspay ! \
    udpsink host=10.10.10.10 port=5000

GST_DEBUG=3 gst-launch-1.0 -v -e \
    udpsrc port=5001 caps="application/x-rtp, media=audio, encoding-name=OPUS, payload=96" ! \
    rtpbin ! \
    rtpopusdepay ! \
    opusdec ! \
    audioconvert ! \
    audioresample ! \
    pipewiresink
