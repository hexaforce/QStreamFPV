import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Dialogs

import org.freedesktop.gstreamer.Qt6GLVideoItem 1.0
import TelemetrySocket 1.0

import "./qml"
import "./qml/menu"
import "./qml/component"

ApplicationWindow {
  id: window
  visible: true
  width: settings.loadSizeParameter("window/size").width
  height: settings.loadSizeParameter("window/size").height
  // color: "transparent"
  x: (Screen.width - width) / 2
  y: (Screen.height - height) / 2

  Material.theme: Material.Dark

  Item {
    anchors.fill: parent
    GstGLQt6VideoItem {
      id: video
      objectName: "videoItem"
      anchors.centerIn: parent
      width: parent.width
      height: parent.height
    }
  }

  Component.onCompleted: {
    if (settings.loadBoolParameter("window/frame_less")) {
      window.flags |= Qt.FramelessWindowHint
    }
    if (settings.loadBoolParameter("window/full_screen")) {
      window.visibility = Window.FullScreen
    }
  }

  signal handleVideoPipelineInfo(var msg)
  signal handleAudioPipelineInfo(var msg)

  signal handleNetworkInterfaceInfo(var msg)

  signal handleVideoSourceList(var msg)
  signal handleAudioSourceList(var msg)
  signal handleAudioSinkList(var msg)

  signal handleV4l2CtlInfo(var msg)
  signal handleV4l2CtlListFormats(var msg)
  signal handleV4l2CtlListFormatsExt(var msg)
  signal handleV4l2CtlListCtrls(var msg)

  TelemetrySocket {
    id: telemetrySocket
    onMessageReceived: function (message){
      // console.log(JSON.stringify(message, null, 2))

      if (message.startsWith("VideoPipelineInfo")) {
        handleVideoPipelineInfo(JSON.parse(message.substring(17)))
      } else if (message.startsWith("AudioPipelineInfo")) {
        handleAudioPipelineInfo(JSON.parse(message.substring(17)))

      } else if (message.startsWith("NetworkInterfaceInfo")) {
          console.log("NetworkInterfaceInfo")
        handleNetworkInterfaceInfo(message.substring(20))

      } else if (message.startsWith("Video/Source")) {
        handleVideoSourceList(JSON.parse(message.substring(12)))
      } else if (message.startsWith("Audio/Source")) {
        handleAudioSourceList(JSON.parse(message.substring(12)))
      } else if (message.startsWith("Audio/Sink")) {
        handleAudioSinkList(JSON.parse(message.substring(10)))

      } else if (message.startsWith("V4l2CtlInfo")) {
        handleV4l2CtlInfo(message.substring(11))
      } else if (message.startsWith("V4l2CtlListFormats")) {
        handleV4l2CtlListFormats(message.substring(18))
      } else if (message.startsWith("V4l2CtlListFormatsExt")) {
        handleV4l2CtlListFormatsExt(message.substring(21))
      } else if (message.startsWith("V4l2CtlCtrls")) {
        handleV4l2CtlListCtrls(message.substring(12))
      }
    }
  }

  property var transmitterVideoPipelineInfo: null
  property var transmitterAudioPipelineInfo: null

  property var networkInterfaceInfo: null

  property var videoSourceList: null
  property var audioSourceList: null
  property var audioSinkList: null

  property var v4l2CtlInfo: null
  property var v4l2CtlListFormats: null
  property var v4l2CtlListFormatsExt: null
  property var v4l2CtlCtrls: null

  Connections {
    target: window

    function onHandleVideoPipelineInfo(msg) {transmitterVideoPipelineInfo = msg.elements.reverse()}
    function onHandleAudioPipelineInfo(msg) {transmitterAudioPipelineInfo = msg.elements.reverse()}

    function onHandleNetworkInterfaceInfo(msg) {networkInterfaceInfo = msg}

    function onHandleVideoSourceList(msg) {videoSourceList = msg}
    function onHandleAudioSourceList(msg) {audioSourceList = msg}
    function onHandleAudioSinkList(msg) {audioSinkList = msg}

    function onHandleV4l2CtlInfo(msg) {v4l2CtlInfo = msg}
    function onHandleV4l2CtlListFormats(msg) {v4l2CtlListFormats = msg}
    function onHandleV4l2CtlListFormatsExt(msg) {v4l2CtlListFormatsExt = msg}
    function onHandleV4l2CtlListCtrls(msg) {v4l2CtlCtrls = msg}
  }

  StackView {
    id: stack
    initialItem: mainView
    anchors.fill: parent
    pushEnter: Transition {
      id: pushEnter
      ParallelAnimation {
        NumberAnimation {
          property: "opacity"
          from: 0
          to: 1
          duration: 400
          easing.type: Easing.OutCubic
        }
      }
    }
  }

  Component {
    id: mainView
    Item {
      Image {
        layer.enabled: true
        x: 10
        y: 10
        source: "icons/gear.svg"
        MouseArea {
          anchors.fill: parent
          onClicked: {
            stack.push(networkPage)
          }
        }
      }
    }
  }

  Component {
    id: networkPage
    Row {
      SideMenu{
        id: sideMenu
        componentId: networkPage
      }
      Item {
        visible: true
        width: window.width - sideMenu.width
        height: window.height
        TabBar {
          id: bar
          width: parent.width
          // opacity: 0.8
          TabButton {
            text: qsTr("Receiver Network Interface")
          }
          TabButton {
            text: qsTr("Transmitter Network Interface")
          }
        }
        StackLayout {
          y: bar.height
          width: parent.width
          height: parent.height - bar.height
          currentIndex: bar.currentIndex
          Item {
            ScrollView {
              width: parent.width
              height: parent.height
              Text {
                color: Material.foreground
                //text: interfaceType == "Ethernet" ? commandRunner.runCommand("sudo ethtool " + networkInterface) : commandRunner.runCommand("iw phy$(iw dev " + networkInterface + " info | grep wiphy | awk '{print $2}') info")
                text: interfaceType === "Ethernet" ? commandRunner.runCommand("sudo ethtool " + networkInterface) : commandRunner.runCommand("iw dev " + networkInterface + " info")
              }
            }
          }
          Item {
            ScrollView {
              width: parent.width
              height: parent.height
              Text {
                color: Material.foreground
                text: networkInterfaceInfo
                onVisibleChanged: {
                  if (visible) {
                    telemetrySocket.sendMessage("NetworkInterfaceInfo")
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  Component {
    id: videoPage
    Row {
      SideMenu{
        id: sideMenu
        componentId: videoPage
      }
      Item {
        visible: true
        width: window.width - sideMenu.width
        height: window.height
        TabBar {
          id: bar
          width: parent.width
          // opacity: 0.8
          TabButton {
            text: qsTr("Receiver Video Pipeline")
          }
          TabButton {
            text: qsTr("Transmitter Video Pipeline")
          }
        }

        StackLayout {
          y: bar.height
          width: parent.width
          height: parent.height - bar.height
          currentIndex: bar.currentIndex
          PipelineInfo {
            pipelineInfo: JSON.parse(videoPipelineManager.getPipelineInfo()).elements.reverse()
          }
          PipelineInfo {
            pipelineInfo: transmitterVideoPipelineInfo
            onVisibleChanged: {
              if (visible) {
                telemetrySocket.sendMessage("VideoPipelineInfo")
              }
            }
          }
        }
      }
    }
  }

  Component {
    id: audioPage
    Row {
      SideMenu{
        id: sideMenu
        componentId: audioPage
      }
      Item {
        visible: true
        width: window.width - sideMenu.width
        height: window.height
        TabBar {
          id: bar
          width: parent.width
          // opacity: 0.8
          TabButton {
            text: qsTr("Receiver Audio Pipeline")
          }
          TabButton {
            text: qsTr("Transmitter Audio Pipeline")
          }
        }
        StackLayout {
          y: bar.height
          width: parent.width
          height: parent.height - bar.height
          currentIndex: bar.currentIndex
          PipelineInfo {
            pipelineInfo: audioPipelineManager ? JSON.parse(audioPipelineManager.getPipelineInfo()).elements.reverse() : null
          }
          PipelineInfo {
            pipelineInfo: transmitterAudioPipelineInfo
            onVisibleChanged: {
              if (visible) {
                telemetrySocket.sendMessage("AudioPipelineInfo")
              }
            }
          }
        }
      }
    }
  }

  Component {
    id: configPage
    Row {
      SideMenu{
        id: sideMenu
        componentId: configPage
      }
      Item {
        visible: true
        width: window.width - sideMenu.width
        height: window.height
        TabBar {
          id: bar
          width: parent.width
          // opacity: 0.8
          TabButton {
            text: qsTr("Window")
          }
          TabButton {
            text: qsTr("Network")
          }
          TabButton {
            text: qsTr("Wifi")
          }
          TabButton {
            text: qsTr("Port")
          }
          TabButton {
            text: qsTr("Streaming")
          }
        }
        StackLayout {
          y: bar.height
          width: parent.width
          height: parent.height - bar.height
          currentIndex: bar.currentIndex
          Item {
            Column{
              spacing: 10
              ConfigFieldSize {
                key: "window/size"
              }
              ConfigFieldBool {
                key: "window/full_screen"
              }
              ConfigFieldBool {
                key: "window/frame_less"
              }
            }
          }
          Item {
            Column{
              spacing: 10
              ConfigFieldString {
                key: "network/mode"
              }
              ConfigFieldString {
                key: "receiver/network_interface"
              }
              ConfigFieldString {
                key: "transmitter/network_interface"
              }
              ConfigFieldString {
                key: "receiver/ip_address"
              }
              ConfigFieldString {
                key: "transmitter/ip_address"
              }
            }
          }
          Item {
            Column{
              spacing: 10
              ConfigFieldString {
                key: "wifi/ssid"
              }
              ConfigFieldString {
                key: "wifi/passphrase"
              }
              ConfigFieldString {
                key: "wifi/access_point"
              }
              ConfigFieldString {
                key: "wifi/access_point_ip_address"
              }
              ConfigFieldString {
                key: "wifi/start_dhcp_range"
              }
              ConfigFieldString {
                key: "wifi/end_dhcp_range"
              }
              ConfigFieldString {
                key: "wifi/subnet_mask"
              }
              ConfigFieldInt {
                key: "wifi/dnsmasq_port"
              }
            }
          }
          Item {
            Column{
              spacing: 10
              ConfigFieldInt {
                key: "receiver/listen_video_port"
              }
              ConfigFieldInt {
                key: "receiver/listen_audio_port"
              }
              ConfigFieldInt {
                key: "receiver/listen_telemetry_port"
              }
              ConfigFieldInt {
                key: "transmitter/listen_telemetry_port"
              }
            }
          }
          Item {
            Column{
              spacing: 10
              ConfigFieldBool {
                key: "transmitter/enable_audio"
              }
              ConfigFieldString {
                key: "receiver/video_codec"
              }
              ConfigFieldString {
                key: "transmitter/video_codec"
              }
              ConfigFieldString {
                key: "receiver/gpu_support"
              }
              ConfigFieldString {
                key: "transmitter/gpu_support"
              }
              ConfigFieldSize {
                key: "transmitter/video_resolution"
              }
              ConfigFieldString {
                key: "transmitter/video_framerate"
              }
            }
          }
        }
      }
    }
  }

}
