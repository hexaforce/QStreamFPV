import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

Item {
    id: root
    Layout.fillWidth: true
    Layout.fillHeight: current
    height: 30

    Material.theme: Material.Dark

    default property var contentItem: null
    property string title: "panel"
    property bool current: false

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            id: bar
            Layout.fillWidth: true
            height: 30
            color:  Material.background
            // opacity: 0.8
            Text {
                anchors.fill: parent
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                text: root.title
                color:  root.current ? Material.accent : Material.foreground
            }

            Text {
                anchors{
                    right: parent.right
                    top: parent.top
                    bottom: parent.bottom
                }
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: "^"
                rotation: root.current ? 180 : 0
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    root.current = !root.current
                    if(root.parent.currentItem !== null && root.title !== root.parent.currentItem.title)
                        root.parent.currentItem.current = false
                    root.parent.currentItem = root
                }
            }
        }

        Rectangle {
            id: container
            Layout.fillWidth: true
            implicitHeight: root.height - bar.height
            clip: true
            color:  Material.background
            // opacity: 0.8
            Behavior on implicitHeight {
                SequentialAnimation {
                    NumberAnimation { duration: 150 }
                    PropertyAction { property: "opacity"; value: root.current ? 1 : 0 }
                }
            }
        }

        Component.onCompleted: {
            if(root.contentItem !== null)
                root.contentItem.parent = container
        }

    }
}
