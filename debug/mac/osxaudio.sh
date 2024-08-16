#!/bin/bash

GST_DEBUG=3 gst-launch-1.0 -v -e \
    osxaudiosrc ! \
    audioconvert ! \
    audioresample ! \
    audio/x-raw,rate=48000,channels=2 ! \
    opusenc bitrate=64000 ! \
    rtpopuspay ! \
    udpsink host=127.0.0.1 port=5001

GST_DEBUG=3 gst-launch-1.0 -v -e \
    udpsrc port=5000 caps="application/x-rtp, media=audio, encoding-name=OPUS, payload=96" ! \
    rtpopusdepay ! \
    opusdec ! \
    audioconvert ! \
    osxaudiosink
