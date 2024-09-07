import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

Item {

    Material.theme: Material.Dark

    property var pipelineInfo: null

    function convertKeyValue(properties) {
        let keys = Object.keys(properties)
        return keys.map(key => {
                            return {
                                "key": key,
                                "value": properties[key]
                            }
                        })
    }

    Text{
        text: "Not Found"
        visible: pipelineInfo == null
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 1
        property var currentItem: null

        Repeater {
            model: pipelineInfo
            delegate: AccordionSection {
                required property var modelData
                property int index: pipelineInfo.findIndex(element => element.name === modelData.name)
                title: "    ".repeat(index) + " > " + modelData.type + " (" + modelData.name + ")"
                contentItem: Rectangle {
                    anchors.fill: parent
                    color:  Material.background
                    ScrollView {
                        width: parent.width
                        height: parent.height
                        ListView {
                            model: convertKeyValue(modelData.properties)
                            delegate: RowLayout {
                                Rectangle {
                                    width: 260
                                    height: 30
                                    color:  Material.background
                                    Text {
                                        text: modelData.key
                                        anchors.centerIn: parent
                                        color:  Material.accent
                                    }
                                }
                                Text {
                                    text: modelData.value
                                    color:  Material.foreground
                                }
                            }
                        }
                    }
                }
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
