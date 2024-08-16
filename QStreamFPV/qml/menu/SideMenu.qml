import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

Column{
    Material.theme: Material.Dark

    property Component componentId: null

    // spacing: 10

    Button {
        text: qsTr("Go Back")
        onClicked: stack.pop(stack.find(function (item) {
            return item.id === componentId
        }))
        implicitWidth: 130
        implicitHeight: 50
        icon.source: "../../icons/backward.svg"
    }
    Button {
        text: qsTr("Go Network")
        enabled: componentId !== networkPage
        onClicked: stack.push(networkPage)
        implicitWidth: 130
        implicitHeight: 50
    }
    Button {
        text: qsTr("Go Video")
        enabled: componentId !== videoPage
        onClicked: stack.push(videoPage)
        implicitWidth: 130
        implicitHeight: 50
    }
    Button {
        text: qsTr("Go Audio")
        enabled: componentId !== audioPage
        onClicked: stack.push(audioPage)
        implicitWidth: 130
        implicitHeight: 50
    }
    Button {
        text: qsTr("Go Config")
        enabled: componentId !== configPage
        onClicked: stack.push(configPage)
        implicitWidth: 130
        implicitHeight: 50
    }
}
