QT += core network
QT -= gui

CONFIG += c++17 cmdline

CONFIG -= no-pkg-config
CONFIG += link_pkgconfig debug_and_release
PKGCONFIG = gstreamer-1.0 gstreamer-video-1.0

DEFINES += GST_USE_UNSTABLE_API TRANSMITTER

INCLUDEPATH += . \
pipeline/ \
utility/ \
telemetry/ \
wifi/ 

HEADERS += \
streamer.h \
wifi/wifi_manager.h \
pipeline/pipeline_manager.h \
pipeline/transmit_video_pipelines.h \
pipeline/audio_pipelines.h \
utility/global_context.h \
utility/system_info.h \
utility/command_runner.h \
utility/settings_manager.h \
telemetry/telemetry_socket_transmitter.h \

SOURCES += \
Transmitter.cpp \
streamer.cpp \
wifi/wifi_manager.cpp \
pipeline/pipeline_manager.cpp \
pipeline/transmit_video_pipelines.cpp \
pipeline/audio_pipelines.cpp \
utility/command_runner.cpp \
utility/settings_manager.cpp \
telemetry/telemetry_socket_transmitter.cpp \

# Default rules for deployment.
# qnx: target.path = /tmp/$${TARGET}/bin
# else: unix:!android: target.path = /opt/$${TARGET}/bin
# !isEmpty(target.path): INSTALLS += target

OBJECTS_DIR = $$OUT_PWD/../transmitter/objects
MOC_DIR = $$OUT_PWD/../transmitter/moc
