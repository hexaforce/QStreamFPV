TEMPLATE = app

QT += core network qml quick widgets

CONFIG -= no-pkg-config
CONFIG += link_pkgconfig debug_and_release
PKGCONFIG = gstreamer-1.0 gstreamer-video-1.0

DEFINES += GST_USE_UNSTABLE_API RECEIVER

INCLUDEPATH += . \
pipeline/ \
utility/ \
telemetry/ \
wifi/ 

HEADERS += \
wifi/wifi_manager.h \
pipeline/pipeline_manager.h \
pipeline/receive_video_pipelines.h \
pipeline/audio_pipelines.h \
utility/global_context.h \
utility/system_info.h \
utility/command_runner.h \
utility/settings_manager.h \
telemetry/telemetry_socket_receiver.h \

SOURCES += \
Receiver.cpp \
wifi/wifi_manager.cpp \
pipeline/pipeline_manager.cpp \
pipeline/receive_video_pipelines.cpp \
pipeline/audio_pipelines.cpp \
utility/command_runner.cpp \
utility/settings_manager.cpp \
telemetry/telemetry_socket_receiver.cpp \

resources.files = main.qml 
resources.prefix = /QStreamFPV
RESOURCES += resources

hostapd.files = wifi/hostapd.conf
hostapd.prefix = /QStreamFPV
RESOURCES += hostapd

gear.files = icons/gear.svg
gear.prefix = /QStreamFPV
RESOURCES += gear

backward.files = icons/backward.svg
backward.prefix = /QStreamFPV
RESOURCES += backward

SideMenu.files = qml/menu/SideMenu.qml
SideMenu.prefix = /QStreamFPV
RESOURCES += SideMenu

AccordionSection.files = qml/component/AccordionSection.qml
AccordionSection.prefix = /QStreamFPV
RESOURCES += AccordionSection

PipelineInfo.files = qml/component/PipelineInfo.qml
PipelineInfo.prefix = /QStreamFPV
RESOURCES += PipelineInfo

ConfigFieldBool.files = qml/ConfigFieldBool.qml
ConfigFieldBool.prefix = /QStreamFPV
RESOURCES += ConfigFieldBool

ConfigFieldInt.files = qml/ConfigFieldInt.qml
ConfigFieldInt.prefix = /QStreamFPV
RESOURCES += ConfigFieldInt

ConfigFieldSize.files = qml/ConfigFieldSize.qml
ConfigFieldSize.prefix = /QStreamFPV
RESOURCES += ConfigFieldSize

ConfigFieldString.files = qml/ConfigFieldString.qml
ConfigFieldString.prefix = /QStreamFPV
RESOURCES += ConfigFieldString

macx {
RGBA.files = RGBA.frag.qsb
RGBA.prefix = /org/freedesktop/gstreamer/qml6
RESOURCES += RGBA
vertex.files = vertex.vert.qsb
vertex.prefix = /org/freedesktop/gstreamer/qml6
RESOURCES += vertex
YUV_TRIPLANAR.files = YUV_TRIPLANAR.frag.qsb
YUV_TRIPLANAR.prefix = /org/freedesktop/gstreamer/qml6
RESOURCES += YUV_TRIPLANAR
}

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
# qnx: target.path = /tmp/$${TARGET}/bin
# else: unix:!android: target.path = /opt/$${TARGET}/bin
# !isEmpty(target.path): INSTALLS += target

OBJECTS_DIR = $$OUT_PWD/../receiver/objects
MOC_DIR = $$OUT_PWD/../receiver/moc
