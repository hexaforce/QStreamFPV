import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

Column{
    Material.theme: Material.Dark
  property string key: ""
  Label {
    text: key
    Layout.preferredWidth: 240
  }
  TextField {
    Layout.preferredHeight: 30
    id: sizeFieldWidth
    text: settings.loadSize(key).width.toString()
    placeholderText: "Width"
    onTextChanged: {
      if (text.match(/^\d+$/)) {
        settings.saveSize(key, Qt.size(parseInt(text), parseInt(sizeFieldHeight.text)))
      }
    }
  }
  TextField {
    Layout.preferredHeight: 30
    id: sizeFieldHeight
    text: settings.loadSize(key).height.toString()
    placeholderText: "Height"
    onTextChanged: {
      if (text.match(/^\d+$/)) {
        settings.saveSize(key, Qt.size(parseInt(sizeFieldWidth.text), parseInt(text)))
      }
    }
  }
}
